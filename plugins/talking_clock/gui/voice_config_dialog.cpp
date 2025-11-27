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
  ui->engine->addItem(tr("Default"), QString("default"));
  const auto avail_engines = QTextToSpeech::availableEngines();
  for (const auto& engine : avail_engines)
    ui->engine->addItem(engine, engine);

  connect(ui->speakButton, &QPushButton::clicked, this, &VoiceConfigDialog::speak);
  connect(ui->pitch, &QSlider::valueChanged, this, &VoiceConfigDialog::setPitch);
  connect(ui->rate, &QSlider::valueChanged, this, &VoiceConfigDialog::setRate);
  connect(ui->volume, &QSlider::valueChanged, this, &VoiceConfigDialog::setVolume);
  connect(ui->engine, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &VoiceConfigDialog::engineSelected);
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
  ui->volume->setToolTip(QString::number(volume));
  if (m_speech) m_speech->setVolume(volume / 100.0);
}

void VoiceConfigDialog::setRate(int rate)
{
  ui->rate->setValue(rate);
  ui->rate->setToolTip(QString::number(rate));
  if (m_speech) m_speech->setRate(rate / 100.0);
}

void VoiceConfigDialog::setPitch(int pitch)
{
  ui->pitch->setValue(pitch);
  ui->pitch->setToolTip(QString::number(pitch));
  if (m_speech) m_speech->setPitch(pitch / 100.0);
}

void VoiceConfigDialog::setEngine(const QString& engine)
{
  int eng_idx = ui->engine->findData(engine);
  if (eng_idx == -1) eng_idx = 0;

  ui->engine->setCurrentIndex(eng_idx);
  // engineSelected(eng_idx);
}

void VoiceConfigDialog::setLanguage(int lang_idx)
{
  if (lang_idx >= 0 && lang_idx < ui->language->count())
    ui->language->setCurrentIndex(lang_idx);
}

void VoiceConfigDialog::setVoice(int voice_idx)
{
  if (voice_idx >= 0 && voice_idx < ui->voice->count())
    ui->voice->setCurrentIndex(voice_idx);
}

void VoiceConfigDialog::speak()
{
  m_speech->say(ui->plainTextEdit->toPlainText());
}
void VoiceConfigDialog::stop()
{
  m_speech->stop();
}

void VoiceConfigDialog::stateChanged(QTextToSpeech::State state)
{
  ui->pauseButton->setEnabled(state == QTextToSpeech::Speaking);
  ui->resumeButton->setEnabled(state == QTextToSpeech::Paused);
  ui->stopButton->setEnabled(state == QTextToSpeech::Speaking || state == QTextToSpeech::Paused);
}

void VoiceConfigDialog::engineSelected(int index)
{
  QString engineName = ui->engine->itemData(index).toString();
  delete m_speech;
  if (engineName == "default")
    m_speech = new QTextToSpeech(this);
  else
    m_speech = new QTextToSpeech(engineName, this);

  disconnect(ui->language, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &VoiceConfigDialog::languageSelected);

  ui->language->clear();
  // Populate the languages combobox before connecting its signal.
  QVector<QLocale> locales = m_speech->availableLocales();
  QLocale current = m_speech->locale();
  for (const QLocale& locale : std::as_const(locales)) {
    QString name(QString("%1 (%2)")
                 .arg(QLocale::languageToString(locale.language()))
                 .arg(QLocale::territoryToString(locale.territory())));
    QVariant localeVariant(locale);
    ui->language->addItem(name, localeVariant);
    if (locale.name() == current.name())
      current = locale;
  }
  m_speech->setVolume(volume() / 100.0);
  m_speech->setRate(rate() / 100.0);
  m_speech->setPitch(pitch() / 100.0);
  m_speech->setLocale(ui->language->currentData().toLocale());
  m_speech->setVoice(m_voices.at(ui->voice->currentIndex()));

  connect(ui->stopButton, &QPushButton::clicked, m_speech, [this]() { m_speech->stop(); });
  connect(ui->pauseButton, &QPushButton::clicked, m_speech, [this]() { m_speech->pause(); });
  connect(ui->resumeButton, &QPushButton::clicked, m_speech, &QTextToSpeech::resume);

  connect(m_speech, &QTextToSpeech::stateChanged, this, &VoiceConfigDialog::stateChanged);
  connect(m_speech, &QTextToSpeech::localeChanged, this, &VoiceConfigDialog::localeChanged);

  connect(ui->language, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &VoiceConfigDialog::languageSelected);
  localeChanged(current);
}

void VoiceConfigDialog::languageSelected(int language)
{
  QLocale locale = ui->language->itemData(language).toLocale();
  if (m_speech) m_speech->setLocale(locale);
}

void VoiceConfigDialog::voiceSelected(int index)
{
  if (m_speech) m_speech->setVoice(m_voices.at(index));
}

void VoiceConfigDialog::localeChanged(const QLocale& locale)
{
  QVariant localeVariant(locale);
  ui->language->setCurrentIndex(ui->language->findData(localeVariant));

  disconnect(ui->voice, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &VoiceConfigDialog::voiceSelected);
  ui->voice->clear();

  m_voices = m_speech->availableVoices();
  QVoice currentVoice = m_speech->voice();
  for (const QVoice& voice : std::as_const(m_voices)) {
    ui->voice->addItem(QString("%1 - %2 - %3").arg(voice.name())
                       .arg(QVoice::genderName(voice.gender()))
                       .arg(QVoice::ageName(voice.age())));
    if (voice.name() == currentVoice.name())
      ui->voice->setCurrentIndex(ui->voice->count() - 1);
  }
  connect(ui->voice, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &VoiceConfigDialog::voiceSelected);
}

} // namespace talking_clock
