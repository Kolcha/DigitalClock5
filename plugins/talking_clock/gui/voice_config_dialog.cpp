/*
 * SPDX-FileCopyrightText: 2016-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "voice_config_dialog.hpp"
#include "ui_voice_config_dialog.h"

namespace talking_clock {

VoiceConfigDialog::VoiceConfigDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::VoiceConfigDialog)
  , m_speech(nullptr)
{
  ui->setupUi(this);

  // Populate engine selection list
  QSignalBlocker _(ui->engine);
  ui->engine->addItem(tr("Default"), QString("default"));
  const auto avail_engines = QTextToSpeech::availableEngines();
  for (const auto& engine : std::as_const(avail_engines))
    ui->engine->addItem(engine, QVariant::fromValue(engine));

  connect(ui->volume, &QSlider::valueChanged, this, &VoiceConfigDialog::setVolumeImpl);
  connect(ui->rate, &QSlider::valueChanged, this, &VoiceConfigDialog::setRateImpl);
  connect(ui->pitch, &QSlider::valueChanged, this, &VoiceConfigDialog::setPitchImpl);
  connect(ui->engine, &QComboBox::currentIndexChanged, this, &VoiceConfigDialog::engineSelected);
  connect(ui->language, &QComboBox::currentIndexChanged, this, &VoiceConfigDialog::languageSelected);
  connect(ui->voice, &QComboBox::currentIndexChanged, this, &VoiceConfigDialog::voiceSelected);
}

VoiceConfigDialog::~VoiceConfigDialog()
{
  delete ui;
}

int VoiceConfigDialog::volume() const
{
  return ui->volume->value();
}

int VoiceConfigDialog::rate() const
{
  return ui->rate->value();
}

int VoiceConfigDialog::pitch() const
{
  return ui->pitch->value();
}

QString VoiceConfigDialog::engine() const
{
  return ui->engine->currentData().toString();
}

int VoiceConfigDialog::language() const
{
  return ui->language->currentIndex();
}

int VoiceConfigDialog::voice() const
{
  return ui->voice->currentIndex();
}

void VoiceConfigDialog::setVolume(int volume)
{
  ui->volume->setValue(volume);
}

void VoiceConfigDialog::setRate(int rate)
{
  ui->rate->setValue(rate);
}

void VoiceConfigDialog::setPitch(int pitch)
{
  ui->pitch->setValue(pitch);
}

void VoiceConfigDialog::setEngine(const QString& engine)
{
  int eng_idx = ui->engine->findData(engine);
  if (eng_idx == -1) eng_idx = 0;

  ui->engine->setCurrentIndex(eng_idx);
  engineSelected(eng_idx);
}

void VoiceConfigDialog::setLanguage(int lang_idx)
{
  Q_ASSERT(m_speech);
  if (lang_idx >= 0 && lang_idx < ui->language->count())
    ui->language->setCurrentIndex(lang_idx);
}

void VoiceConfigDialog::setVoice(int voice_idx)
{
  Q_ASSERT(m_speech);
  if (voice_idx >= 0 && voice_idx < ui->voice->count())
    ui->voice->setCurrentIndex(voice_idx);
}

void VoiceConfigDialog::setVolumeImpl(int volume)
{
  ui->volume->setToolTip(QString::number(volume));
  if (m_speech) m_speech->setVolume(volume / 100.0);
}

void VoiceConfigDialog::setRateImpl(int rate)
{
  ui->rate->setToolTip(QString::number(rate));
  if (m_speech) m_speech->setRate(rate / 100.0);
}

void VoiceConfigDialog::setPitchImpl(int pitch)
{
  ui->pitch->setToolTip(QString::number(pitch));
  if (m_speech) m_speech->setPitch(pitch / 100.0);
}

void VoiceConfigDialog::stateChanged(QTextToSpeech::State state)
{
  ui->pauseButton->setEnabled(state == QTextToSpeech::Speaking);
  ui->resumeButton->setEnabled(state == QTextToSpeech::Paused);
  ui->stopButton->setEnabled(state == QTextToSpeech::Speaking || state == QTextToSpeech::Paused);
}

void VoiceConfigDialog::engineSelected(int index)
{
  delete m_speech;

  const auto engine_name = ui->engine->itemData(index).toString();
  if (engine_name == "default")
    m_speech = new QTextToSpeech(this);
  else
    m_speech = new QTextToSpeech(engine_name, this);

  QSignalBlocker _(ui->language);
  ui->language->clear();
  // Populate the languages combobox
  const auto locales = m_speech->availableLocales();
  QLocale current = m_speech->locale();
  for (const auto& locale : std::as_const(locales)) {
    auto name = QString("%1 (%2)").arg(
          QLocale::languageToString(locale.language()),
          QLocale::territoryToString(locale.territory()));
    ui->language->addItem(name, QVariant::fromValue(locale));
    if (locale.name() == current.name())
      current = locale;
  }

  m_speech->setVolume(volume() / 100.0);
  m_speech->setRate(rate() / 100.0);
  m_speech->setPitch(pitch() / 100.0);

  connect(ui->speakButton, &QPushButton::clicked, m_speech, [this]() { m_speech->say(ui->plainTextEdit->toPlainText()); });
  connect(ui->stopButton, &QPushButton::clicked, m_speech, [this]() { m_speech->stop(); });
  connect(ui->pauseButton, &QPushButton::clicked, m_speech, [this]() { m_speech->pause(); });
  connect(ui->resumeButton, &QPushButton::clicked, m_speech, &QTextToSpeech::resume);

  connect(m_speech, &QTextToSpeech::stateChanged, this, &VoiceConfigDialog::stateChanged);
  connect(m_speech, &QTextToSpeech::localeChanged, this, &VoiceConfigDialog::localeChanged);

  localeChanged(current);
}

void VoiceConfigDialog::languageSelected(int language)
{
  m_speech->setLocale(ui->language->itemData(language).toLocale());
}

void VoiceConfigDialog::voiceSelected(int index)
{
  m_speech->setVoice(ui->voice->itemData(index).value<QVoice>());
}

void VoiceConfigDialog::localeChanged(const QLocale& locale)
{
  ui->language->setCurrentIndex(ui->language->findData(QVariant::fromValue(locale)));

  QSignalBlocker _(ui->voice);
  ui->voice->clear();
  // Populate the voices combobox
  const auto voices = m_speech->availableVoices();
  QVoice current = m_speech->voice();
  for (const auto& voice : std::as_const(voices)) {
    auto name = QString("%1 - %2 - %3").arg(
          voice.name(),
          QVoice::genderName(voice.gender()),
          QVoice::ageName(voice.age()));
    ui->voice->addItem(name, QVariant::fromValue(voice));
    if (voice.name() == current.name())
      ui->voice->setCurrentIndex(ui->voice->count() - 1);
  }
}
} // namespace talking_clock
