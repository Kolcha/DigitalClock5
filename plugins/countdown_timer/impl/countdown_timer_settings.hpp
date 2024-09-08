/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "config/settings_storage.hpp"

#include <QDateTime>

namespace plugin_impl {

QDateTime default_target_date();

class CountdownTimerPluginSettings : public SettingsStorageClient {
public:
  explicit CountdownTimerPluginSettings(ISettingsStorage& st)
      : SettingsStorageClient("CountdownTimer", st) {}

  CONFIG_OPTION_Q(int, IntervalHours, 0)
  CONFIG_OPTION_Q(int, IntervalMinutes, 0)
  CONFIG_OPTION_Q(int, IntervalSeconds, 0)

  CONFIG_OPTION_Q(bool, UseTargetDateTime, true)
  CONFIG_OPTION_Q(QDateTime, TargetDateTime, default_target_date())

  CONFIG_OPTION_Q(bool, ChimeOnTimeout, false)
  CONFIG_OPTION_Q(QString, ChimeSoundFile, QString())

  CONFIG_OPTION_Q(bool, ShowMessage, false)
  CONFIG_OPTION_Q(QString, MessageText, QString())

  CONFIG_OPTION_Q(int, HideDaysThreshold, 0)
  CONFIG_OPTION_Q(bool, AlsoHideHours, false)

  CONFIG_OPTION_Q(bool, RestartOnDblClick, false)
  CONFIG_OPTION_Q(bool, RestartOnTimeout, false)

  CONFIG_OPTION_Q(QString, PauseHotkey, QString())
  CONFIG_OPTION_Q(QString, RestartHotkey, QString())

  CONFIG_OPTION_Q(bool, HideInactive, false)
  CONFIG_OPTION_Q(bool, ReverseCounting, false)
};

} // namespace plugin_impl
