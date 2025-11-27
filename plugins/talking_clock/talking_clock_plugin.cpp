/*
 * SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "talking_clock_plugin.hpp"

#include <QTextToSpeech>

#include "gui/settings_dialog.hpp"

TalkingClockPlugin::TalkingClockPlugin(const TalkingClockPluginConfig& cfg)
  : ClockPluginInstance()
  , _cfg(cfg)
{
  _time_zone = QTimeZone::systemTimeZone();
}

TalkingClockPlugin::~TalkingClockPlugin() = default;

void TalkingClockPlugin::startup()
{
  initSpeechEngine();
}

void TalkingClockPlugin::shutdown()
{
  _speech->stop();
  _speech.reset();
}

void TalkingClockPlugin::update(const QDateTime& dt)
{
  if (!_speech || _speech->state() == QTextToSpeech::Speaking)
    return;

  if (_time_zone != dt.timeZone())
    _time_zone = dt.timeZone();

  auto cur_time = dt.time();

  if (cur_time.minute() == 0 &&   // hour
      _playback_allowed && _cfg.getEveryHourEnabled()) {
    _speech->say(_speech->locale().toString(cur_time, _cfg.getEveryHourFormat()));
    _playback_allowed = false;
  }

  if (cur_time.minute() % 15 == 0 && cur_time.minute() != 0 &&    // quarter
      _playback_allowed && _cfg.getQuarterHourEnabled()) {
    _speech->say(_speech->locale().toString(cur_time, _cfg.getQuarterHourFormat()));
    _playback_allowed = false;
  }

  _playback_allowed = (cur_time.minute() % 15 != 0);
}

void TalkingClockPlugin::initSpeechEngine()
{
  QString engine_name = _cfg.getSynthesisEngine();
  if (engine_name == "default" || !QTextToSpeech::availableEngines().contains(engine_name))
    _speech = std::make_unique<QTextToSpeech>();
  else
    _speech = std::make_unique<QTextToSpeech>(engine_name);

  _speech->setVolume(_cfg.getVolume() / 100.0);
  _speech->setRate(_cfg.getSynthesisRate() / 100.0);
  _speech->setPitch(_cfg.getSynthesisPitch() / 100.0);

  const auto locales = _speech->availableLocales();
  const auto locale_index = _cfg.getSynthesisLanguage();
  if (locale_index >= 0 && locale_index < locales.size())
    _speech->setLocale(locales.at(locale_index));

  const auto voices = _speech->availableVoices();
  const auto voice_index = _cfg.getSynthesisVoice();
  if (voice_index >= 0 && voice_index < voices.size())
    _speech->setVoice(voices.at(voice_index));
}


void TalkingClockPluginFactory::init(Context&& ctx)
{
  Q_ASSERT(!ctx.active_instances.empty());
  _first_idx = ctx.active_instances.front();
  _cfg = std::make_unique<TalkingClockPluginConfig>(std::move(ctx.settings));
}

QString TalkingClockPluginFactory::description() const
{
  return tr("Announces time with selected period.");
}

ClockPluginInstance* TalkingClockPluginFactory::instance(size_t idx)
{
  if (idx != _first_idx)
    return nullptr;

  if (!_inst)
    _inst = std::make_unique<TalkingClockPlugin>(std::ref(*_cfg));

  return _inst.get();
}

void TalkingClockPluginFactory::configure(QWidget* parent, size_t idx)
{
  Q_UNUSED(idx);
  using talking_clock::SettingsDialog;
  auto tz = _inst ? _inst->tz() : QTimeZone::systemTimeZone();
  auto d = new SettingsDialog(*_cfg, tz, parent);

  connect(d, &SettingsDialog::accepted, _cfg.get(), &TalkingClockPluginConfig::commit);
  connect(d, &SettingsDialog::rejected, _cfg.get(), &TalkingClockPluginConfig::discard);

  if (_inst) {
    connect(d, &SettingsDialog::accepted, _inst.get(), &TalkingClockPlugin::initSpeechEngine);
    connect(d, &SettingsDialog::rejected, _inst.get(), &TalkingClockPlugin::initSpeechEngine);
  }

  connect(d, &SettingsDialog::finished, d, &QObject::deleteLater);

  d->show();
}
