/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin_config.hpp"

namespace plugin_impl {

class StopwatchPluginSettings : public SettingsStorageClient {
public:
  explicit StopwatchPluginSettings(ISettingsStorage& st)
      : SettingsStorageClient("Stopwatch", st) {}

  CONFIG_OPTION_Q(bool, HideInactive, false)

  CONFIG_OPTION_Q(QString, PauseHotkey, QString())
  CONFIG_OPTION_Q(QString, RestartHotkey, QString())
};

} // namespace plugin_impl
