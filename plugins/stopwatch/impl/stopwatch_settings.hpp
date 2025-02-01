/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/text/plugin_config.hpp"

namespace timetracker {

Q_NAMESPACE

enum Options {
  HideInactive,

  PauseHotkey,
  RestartHotkey,
};

Q_ENUM_NS(Options)

#define PLUGIN_CONFIG_OPTION(type, name)                                                        \
  void set##name(const type& v) { ConfigBase::setValue(key(name), v); }                         \
  type get##name() const { return ConfigBase::value(key(name), def_value(name).value<type>()); }

class StopwatchInstanceConfig : public plugin::text::PluginInstanceConfig
{
  Q_OBJECT

public:
  using PluginInstanceConfig::PluginInstanceConfig;

  bool getUseClockSkinDefault() const noexcept override { return true; }

  PLUGIN_CONFIG_OPTION(bool, HideInactive)

  PLUGIN_CONFIG_OPTION(QString, PauseHotkey)
  PLUGIN_CONFIG_OPTION(QString, RestartHotkey)

private:
  static QString key(Options o);
  static QVariant def_value(Options o);
};

#undef PLUGIN_CONFIG_OPTION


class StopwatchPluginConfig : public plugin::text::PluginConfig
{
  Q_OBJECT

public:
  using PluginConfig::PluginConfig;

protected:
  std::unique_ptr<plugin::text::PluginInstanceConfig>
  createInstanceImpl(std::unique_ptr<SettingsStorage> st) const override
  {
    return std::make_unique<StopwatchInstanceConfig>(std::move(st));
  }
};

} // namespace timetracker
