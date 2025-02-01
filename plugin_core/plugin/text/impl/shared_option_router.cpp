// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "shared_option_router.hpp"

namespace plugin::text::impl {

bool SharedOptionRouter::should_route(opt::InstanceOptions o) const
{
  static const QSet<opt::InstanceOptions> always_route_opts = {
    opt::Scaling,
  };

  static const QSet<opt::InstanceOptions> never_route_opts = {
    opt::TruePerCharRendering,
    opt::RespectLineSpacing,
    opt::CharSpacing,
    opt::LineSpacing,
    opt::CharMargins,
    opt::WidgetMargins,
    opt::LayoutConfig,
  };

  static const QSet<opt::InstanceOptions> skin_opts = {
    opt::UseSkin,
    opt::Skin,
    opt::Font,
  };

  if (always_route_opts.contains(o))
    return true;

  if (never_route_opts.contains(o))
    return false;

  if (_plugin_settings.getFollowClockAppearance() && !skin_opts.contains(o))
    return true;

  if (_plugin_settings.getUseClockSkin() && skin_opts.contains(o))
    return true;

  return false;
}

} // namespace plugin::text::impl
