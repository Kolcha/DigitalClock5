/*
 * SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/text/skinned_text_plugin_base.hpp"

#include "impl/time_zone_settings.hpp"

using namespace plugin::text;
using plugin::time_zone::TimeZonePluginInstanceConfig;

class TimeZonePlugin : public TextPluginInstanceBase
{
  Q_OBJECT

public:
  TimeZonePlugin(const TimeZonePluginInstanceConfig* cfg);

public slots:
  void startup() override;

  void update(const QDateTime& dt) override;

  void onTimeZoneFormatChanged();

protected:
  QString text() const override { return _tz_str; }

  void pluginReloadConfig() override;

private:
  void updateTimeZoneFmt();
  void updateTimeZoneStr();

private:
  const TimeZonePluginInstanceConfig* _cfg;
  QDateTime _last_date;
  QTimeZone _last_tz;
  QString _tz_fmt;
  QString _tz_str;
};


class TimeZonePluginFactory : public TextPluginBase
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "time_zone.json")
  Q_INTERFACES(ClockPlugin)

public:
  QString name() const override { return tr("Time Zone"); }
  QString description() const override;

protected:
  Instance createInstance(size_t idx) const override
  {
    auto cfg = qobject_cast<TimeZonePluginInstanceConfig*>(instanceConfig(idx));
    Q_ASSERT(cfg);
    return std::make_unique<TimeZonePlugin>(cfg);
  }

  std::unique_ptr<PluginConfig> createConfig(
    std::unique_ptr<PluginSettingsBackend> b) const override
  {
    return std::make_unique<plugin::time_zone::TimeZonePluginConfig>(std::move(b));
  }

  QVector<QWidget*> configPagesBeforeCommonPages() override;
};
