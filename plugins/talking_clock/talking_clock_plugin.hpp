/*
 * SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/clock_plugin.hpp"

#include "impl/talking_clock_settings.hpp"

using talking_clock::TalkingClockPluginConfig;
class QTextToSpeech;

class TalkingClockPlugin : public ClockPluginInstance
{
  Q_OBJECT

public:
  TalkingClockPlugin(const TalkingClockPluginConfig& cfg);
  ~TalkingClockPlugin();

  QTimeZone tz() const { return _time_zone; }

public slots:
  void startup() override;
  void shutdown() override;

  void update(const QDateTime& dt) override;

  void initSpeechEngine();

private:
  const TalkingClockPluginConfig& _cfg;

  std::unique_ptr<QTextToSpeech> _speech;
  bool _playback_allowed = false;
  QTimeZone _time_zone;
};


class TalkingClockPluginFactory : public ClockPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "talking_clock.json")
  Q_INTERFACES(ClockPlugin)

public:
  void init(Context&& ctx) override;

  QString name() const override { return tr("Talking clock"); }
  QString description() const override;

  ClockPluginInstance* instance(size_t idx) override;

public slots:
  void configure(QWidget* parent, size_t idx) override;

private:
  size_t _first_idx = 0;
  std::unique_ptr<TalkingClockPluginConfig> _cfg;
  std::unique_ptr<TalkingClockPlugin> _inst;
};
