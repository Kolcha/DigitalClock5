/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "date_plugin.hpp"

#include "gui/settings_widget.hpp"
#include "impl/date_plugin_impl.hpp"

DatePlugin::DatePlugin()
    : _impl(std::make_unique<DatePluginImpl>())
{
}

DatePlugin::~DatePlugin() = default;

void DatePlugin::saveState(StateClient& st) const
{
  Q_UNUSED(st);
}

void DatePlugin::loadState(const StateClient& st)
{
  Q_UNUSED(st);
}

void DatePlugin::initSettings(const SettingsClient& st)
{
  _impl->initSettings(st);
  _impl->updateWidget();
  WidgetPluginBase::initSettings(st);
}

void DatePlugin::onTimeChanged(const QDateTime& dt)
{
  _impl->last_dt = dt;
  if (!_impl->widget)
    return;
  _impl->widget->setDateTime(dt);
}

QList<QWidget*> DatePlugin::customConfigure(SettingsClient& s, StateClient& t)
{
  return { new SettingsWidget(s, t, _impl.get()) };
}

std::shared_ptr<GraphicsWidgetBase> DatePlugin::createWidget()
{
  _impl->widget = std::make_shared<GraphicsDateTimeWidget>();
  _impl->widget->setDateTime(_impl->last_dt);
  _impl->updateWidget();
  return _impl->widget;
}

void DatePlugin::destroyWidget()
{
  _impl->widget.reset();
}


std::unique_ptr<ClockPluginBase> DatePluginFactory::create() const
{
  return std::make_unique<DatePlugin>();
}

QString DatePluginFactory::description() const
{
  return tr("Allows to display current date under the clock.");
}
