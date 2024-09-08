/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "widget_plugin_base.hpp"

class DatePluginImpl;

class DatePlugin : public WidgetPluginBase
{
  Q_OBJECT

public:
  DatePlugin();
  ~DatePlugin();

  void saveState(StateClient& st) const override;
  void loadState(const StateClient& st) override;

  void initSettings(PluginSettingsStorage& st) override;

public slots:
  void onTimeChanged(const QDateTime& dt) override;

protected:
  QList<QWidget*> customConfigure(PluginSettingsStorage& s, StateClient& t) override;

  std::shared_ptr<GraphicsWidgetBase> createWidget() override;
  void destroyWidget() override;

private:
  std::unique_ptr<DatePluginImpl> _impl;
};


class DatePluginFactory : public ClockPluginFactory
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPluginFactory_iid FILE "date.json")
  Q_INTERFACES(ClockPluginFactory)

public:
  std::unique_ptr<ClockPluginBase> create() const override;

  QString title() const override { return tr("Date"); }
  QString description() const override;

  bool perClockInstance() const noexcept override { return true; }
};
