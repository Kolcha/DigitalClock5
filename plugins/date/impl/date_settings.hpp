/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/text/plugin_config.hpp"

#include <QLocale>

namespace plugin::date {

class DatePluginInstanceConfig : public text::PluginInstanceConfig
{
  Q_OBJECT

public:
  using PluginInstanceConfig::PluginInstanceConfig;

  enum FormatType {System, Custom};
  Q_ENUM(FormatType);

  CONFIG_OPTION(FormatType, "format_type", FormatType, System)
  CONFIG_OPTION(FormatSys, "format_sys", QLocale::FormatType, QLocale::LongFormat)
  CONFIG_OPTION(FormatStr, "format_str", QString, QLocale().dateFormat())
};


class DatePluginConfig : public text::PluginConfig
{
  Q_OBJECT

public:
  using PluginConfig::PluginConfig;

protected:
  std::unique_ptr<text::PluginInstanceConfig>
  createInstanceImpl(std::unique_ptr<SettingsStorage> st) const override
  {
    return std::make_unique<DatePluginInstanceConfig>(std::move(st));
  }
};

} // namespace plugin::date
