/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin_config.hpp"

namespace plugin_impl {

QDateTime default_target_date();

template<class C>
class CountdownTimerPluginSettings : public PluginConfig<C> {
public:
  explicit CountdownTimerPluginSettings(C& c) : PluginConfig<C>(c) {}

  PLG_CONFIG_OPTION_Q(int, IntervalHours, 0)
  PLG_CONFIG_OPTION_Q(int, IntervalMinutes, 0)
  PLG_CONFIG_OPTION_Q(int, IntervalSeconds, 0)

  PLG_CONFIG_OPTION_Q(bool, UseTargetDateTime, true)
  PLG_CONFIG_OPTION_Q(QDateTime, TargetDateTime, default_target_date())

  PLG_CONFIG_OPTION_Q(bool, ChimeOnTimeout, false)
  PLG_CONFIG_OPTION_Q(QString, ChimeSoundFile, QString())

  PLG_CONFIG_OPTION_Q(bool, ShowMessage, false)
  PLG_CONFIG_OPTION_Q(QString, MessageText, QString())

  PLG_CONFIG_OPTION_Q(int, HideDaysThreshold, 0)
  PLG_CONFIG_OPTION_Q(bool, AlsoHideHours, false)

  PLG_CONFIG_OPTION_Q(bool, RestartOnDblClick, false)
  PLG_CONFIG_OPTION_Q(bool, RestartOnTimeout, false)

  PLG_CONFIG_OPTION_Q(QString, PauseHotkey, QString())
  PLG_CONFIG_OPTION_Q(QString, RestartHotkey, QString())

  PLG_CONFIG_OPTION_Q(bool, HideInactive, false)
  PLG_CONFIG_OPTION_Q(bool, ReverseCounting, false)
};

} // namespace plugin_impl
