/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "widget_plugin_base.hpp"

#include "impl/countdown_timer_plugin_impl.hpp"

class QMediaPlayer;

class CountdownTimerPlugin : public WidgetPluginBase
{
  Q_OBJECT

public:
  CountdownTimerPlugin();
  ~CountdownTimerPlugin();

  void saveState(StateClient& st) const override;
  void loadState(const StateClient& st) override;

  void initSettings(const SettingsClient& st) override;

  void accepted() override;
  void rejected() override;

public slots:
  void init() override;
  void shutdown() override;

protected:
  QList<QWidget*> customConfigure(SettingsClient& s, StateClient& t) override;

  std::shared_ptr<GraphicsWidgetBase> createWidget() override;
  void destroyWidget() override;

private slots:
  void onWidgetClicked();
  void onWidgetDblclicked();

  void onUpdate();
  void onTimeout();

private:
  std::unique_ptr<CountdownTimerPluginImpl> _impl;
  std::unique_ptr<QMediaPlayer> _player;
};


class CountdownTimerPluginFactory : public ClockPluginFactory
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPluginFactory_iid FILE "countdown_timer.json")
  Q_INTERFACES(ClockPluginFactory)

public:
  std::unique_ptr<ClockPluginBase> create() const override;

  QString title() const override { return tr("Countdown timer"); }
  QString description() const override;

  bool perClockInstance() const noexcept override { return true; }
};
