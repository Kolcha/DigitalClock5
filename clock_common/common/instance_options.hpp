// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "clock_common_global.hpp"

#include <QtCore/QMetaObject>

namespace tx {

Q_NAMESPACE_EXPORT(CLOCK_COMMON_EXPORT)

// used as value for TextureType/BackgroundType
enum TextureType {
  None,
  Color,
  Gradient,
  Pattern,
};

Q_ENUM_NS(TextureType)

} // namespace tx

namespace opt {

Q_NAMESPACE_EXPORT(CLOCK_COMMON_EXPORT)

enum InstanceOptions {
  // appearance options
  UseSkin,
  Skin,
  Font,

  Scaling,

  Opacity,

  TextureType,              // none/color/gradient/pattern
  TextureCustomization,     // none/char/line/text
  TextureColor,
  TextureColorUseTheme,
  TextureGradient,
  TexturePattern,           // path or optionally blob
  TexturePatternTile,

  BackgroundType,           // none/color/gradient/pattern
  BackgroundCustomization,  // none/char/line/text
  BackgroundColor,
  BackgroundColorUseTheme,
  BackgroundGradient,
  BackgroundPattern,        // path or optionally blob
  BackgroundPatternTile,

  ApplyBackgroundToWindow,  // just an idea for now

  TruePerCharRendering,

  ColorizationEnabled,
  ColorizationColor,
  ColorizationStrength,

  // geometry options
  LayoutSpacing,            // main window layout
  CharSpacing,
  LineSpacing,
  RespectLineSpacing,

  CharMargins,
  WidgetMargins,
  LayoutMargins,

  // date/time options
  TimeFormat,
  LayoutConfig,

  SmallerSeconds,
  SecondsSize,

  ShowLocalTime,
  TimeZone,

  FlashingSeparator,
  UseCustomSeparators,
  CustomSeparators,
};

Q_ENUM_NS(InstanceOptions)

} // namespace opt
