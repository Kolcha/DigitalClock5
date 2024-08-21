/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QtCore/qobjectdefs.h>
#include "plugin_core_global.hpp"

namespace clock_settings {

Q_NAMESPACE_EXPORT(PLUGIN_CORE_EXPORT)

enum SharedConfigKeys {
  Opacity,

  ScalingH,
  ScalingV,

  SpacingH,
  SpacingV,

  ApplyColorization,
  ColorizationStrength,
  ColorizationColor,

  UseFontInsteadOfSkin,
  Font,
  Skin,

  SeparatorFlashes,
  UseCustomSeparators,
  CustomSeparators,

  TimeFormat,
  SecondsScaleFactor,

  Texture,
  TextureStretch,
  TexturePerCharacter,

  Background,
  BackgroundStretch,
  BackgroundPerCharacter,
};

Q_ENUM_NS(SharedConfigKeys)

} // namespace clock_settings

namespace cs = clock_settings;
