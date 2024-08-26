/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin_config.hpp"

namespace plugin_impl {

template<class C>
class AnyZoomPluginSettings : public PluginConfig<C> {
public:
  explicit AnyZoomPluginSettings(C& c) : PluginConfig<C>(c) {}

  PLG_CONFIG_OPTION_Q(int, ZoomX, 125)
  PLG_CONFIG_OPTION_Q(int, ZoomY, 125)
};

} // namespace plugin_impl
