/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "config/config_base.hpp"

namespace any_zoom {

class AnyZoomPluginInstanceConfig : public ConfigBase
{
public:
  using ConfigBase::ConfigBase;

  CONFIG_OPTION(ZoomX, "zoom_x", int, 125)
  CONFIG_OPTION(ZoomY, "zoom_y", int, 125)
};

} // namespace any_zoom
