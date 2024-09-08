/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "config/settings_storage.hpp"

namespace plugin_impl {

class AnyZoomPluginSettings : public SettingsStorageClient {
public:
  explicit AnyZoomPluginSettings(ISettingsStorage& st)
      : SettingsStorageClient("AnyZoom", st) {}

  CONFIG_OPTION_Q(int, ZoomX, 125)
  CONFIG_OPTION_Q(int, ZoomY, 125)
};

} // namespace plugin_impl
