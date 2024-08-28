/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "widget_plugin_base.hpp"

#include "impl/stopwatch_plugin_impl.hpp"

class StopwatchPlugin : public WidgetPluginBase
{
  Q_OBJECT

public:
  StopwatchPlugin();
  ~StopwatchPlugin();

  void saveState(StateClient& st) const override;
  void loadState(const StateClient& st) override;

  void initSettings(const SettingsClient& st) override;

public slots:
  void init() override;
  void shutdown() override;

  void tick() override;

protected:
  QList<QWidget*> customConfigure(SettingsClient& s, StateClient& t) override;

  std::shared_ptr<GraphicsWidgetBase> createWidget() override;
  void destroyWidget() override;

private slots:
  void onWidgetClicked();

private:
  std::unique_ptr<StopwatchPluginImpl> _impl;
};


class StopwatchPluginFactory : public ClockPluginFactory
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPluginFactory_iid FILE "stopwatch.json")
  Q_INTERFACES(ClockPluginFactory)

public:
  std::unique_ptr<ClockPluginBase> create() const override;

  QString title() const override { return tr("Stopwatch"); }
  QString description() const override;

  bool perClockInstance() const noexcept override { return true; }
};
