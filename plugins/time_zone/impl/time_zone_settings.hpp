/*
 * SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/text/plugin_config.hpp"

namespace plugin::time_zone {

class TimeZonePluginInstanceConfig : public text::PluginInstanceConfig
{
  Q_OBJECT

public:
  using PluginInstanceConfig::PluginInstanceConfig;

  CONFIG_OPTION(FormatStr, "format_str", QString, QString("n"))
};


class TimeZonePluginConfig : public text::PluginConfig
{
  Q_OBJECT

public:
  using PluginConfig::PluginConfig;

protected:
  std::unique_ptr<text::PluginInstanceConfig>
  createInstanceImpl(std::unique_ptr<SettingsStorage> st) const override
  {
    return std::make_unique<TimeZonePluginInstanceConfig>(std::move(st));
  }
};

} // namespace plugin::time_zone
