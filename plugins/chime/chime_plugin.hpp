/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "clock_plugin.hpp"

class ChimePluginImpl;
class QMediaPlayer;

class ChimePlugin : public ClockPluginBase, public ConfigurablePlugin
{
  Q_OBJECT

public:
  ChimePlugin();
  ~ChimePlugin();

  void initSettings(PluginSettingsStorage& st) override;

  QWidget* configure(PluginSettingsStorage& s, StateClient& t) override;

public slots:
  void init() override;
  void shutdown() override;

  void tick() override;

private:
  std::unique_ptr<ChimePluginImpl> _impl;
  std::unique_ptr<QMediaPlayer> _player;
};


class ChimePluginFactory : public ClockPluginFactory
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPluginFactory_iid FILE "chime.json")
  Q_INTERFACES(ClockPluginFactory)

public:
  std::unique_ptr<ClockPluginBase> create() const override;

  QString title() const override { return tr("Chime"); }
  QString description() const override;

  bool perClockInstance() const noexcept override { return false; }
};
