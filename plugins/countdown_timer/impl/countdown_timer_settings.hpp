/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/text/plugin_config.hpp"

#include <QDateTime>

namespace countdown_timer {

Q_NAMESPACE

enum Options {
  IntervalHours,
  IntervalMinutes,
  IntervalSeconds,

  UseTargetDateTime,
  TargetDateTime,

  ChimeOnTimeout,
  ChimeSoundFile,

  ShowMessage,
  MessageText,

  HideDaysThreshold,
  AlsoHideHours,

  RestartOnDblClick,
  RestartOnTimeout,

  HideInactive,

  ReverseCounting,

  PauseHotkey,
  RestartHotkey,
};

Q_ENUM_NS(Options)

QDateTime default_target_date();

#define PLUGIN_CONFIG_OPTION(type, name)                                                        \
  void set##name(const type& v) { ConfigBase::setValue(key(name), v); }                         \
  type get##name() const { return ConfigBase::value(key(name), def_value(name).value<type>()); }

class CountdownTimerInstanceConfig : public plugin::text::PluginInstanceConfig
{
  Q_OBJECT

public:
  using PluginInstanceConfig::PluginInstanceConfig;

  bool getUseClockSkinDefault() const noexcept override { return true; }

  PLUGIN_CONFIG_OPTION(int, IntervalHours)
  PLUGIN_CONFIG_OPTION(int, IntervalMinutes)
  PLUGIN_CONFIG_OPTION(int, IntervalSeconds)

  PLUGIN_CONFIG_OPTION(bool, UseTargetDateTime)
  PLUGIN_CONFIG_OPTION(QDateTime, TargetDateTime)

  PLUGIN_CONFIG_OPTION(bool, ChimeOnTimeout)
  PLUGIN_CONFIG_OPTION(QString, ChimeSoundFile)

  PLUGIN_CONFIG_OPTION(bool, ShowMessage)
  PLUGIN_CONFIG_OPTION(QString, MessageText)

  PLUGIN_CONFIG_OPTION(int, HideDaysThreshold)
  PLUGIN_CONFIG_OPTION(bool, AlsoHideHours)

  PLUGIN_CONFIG_OPTION(bool, RestartOnDblClick)
  PLUGIN_CONFIG_OPTION(bool, RestartOnTimeout)

  PLUGIN_CONFIG_OPTION(bool, HideInactive)

  PLUGIN_CONFIG_OPTION(bool, ReverseCounting)

  PLUGIN_CONFIG_OPTION(QString, PauseHotkey)
  PLUGIN_CONFIG_OPTION(QString, RestartHotkey)

private:
  static QString key(Options o);
  static QVariant def_value(Options o);
};

#undef PLUGIN_CONFIG_OPTION


class CountdownTimerPluginConfig : public plugin::text::PluginConfig
{
  Q_OBJECT

public:
  using PluginConfig::PluginConfig;

protected:
  std::unique_ptr<plugin::text::PluginInstanceConfig>
  createInstanceImpl(std::unique_ptr<SettingsStorage> st) const override
  {
    return std::make_unique<CountdownTimerInstanceConfig>(std::move(st));
  }
};

} // namespace countdown_timer
