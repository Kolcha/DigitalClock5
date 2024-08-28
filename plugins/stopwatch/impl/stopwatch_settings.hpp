/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin_config.hpp"

namespace plugin_impl {

template<class C>
class StopwatchPluginSettings : public PluginConfig<C> {
public:
  explicit StopwatchPluginSettings(C& c) : PluginConfig<C>(c) {}

  PLG_CONFIG_OPTION_Q(bool, HideInactive, false)

  PLG_CONFIG_OPTION_Q(QString, PauseHotkey, QString())
  PLG_CONFIG_OPTION_Q(QString, RestartHotkey, QString())
};

} // namespace plugin_impl
