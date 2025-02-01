/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/clock_plugin.hpp"

#include "impl/chime_settings.hpp"

using chime::ChimePluginConfig;
class QMediaPlayer;

class ChimePlugin : public ClockPluginInstance
{
  Q_OBJECT

public:
  ChimePlugin(const ChimePluginConfig& cfg);
  ~ChimePlugin();

public slots:
  void startup() override;
  void shutdown() override;

  void update(const QDateTime& dt) override;

private:
  bool isQuietTime(const QTime& t) const;

private:
  std::unique_ptr<QMediaPlayer> _player;
  const ChimePluginConfig& _cfg;

  bool _playback_allowed = false;
};


class ChimePluginFactory : public ClockPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "chime.json")
  Q_INTERFACES(ClockPlugin)

public:
  void init(Context&& ctx) override;

  QString name() const override { return tr("Chime"); }
  QString description() const override;

  ClockPluginInstance* instance(size_t idx) override;

public slots:
  void configure(QWidget* parent, size_t idx) override;

private:
  size_t _first_idx = 0;
  std::unique_ptr<ChimePluginConfig> _cfg;
  std::unique_ptr<SettingsStorage> _state;
  std::unique_ptr<ChimePlugin> _inst;
};
