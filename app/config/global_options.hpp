// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtCore/QMetaObject>

namespace opt {
inline namespace app {

Q_NAMESPACE

enum TrayIconAction {
  NoAction,
  OpenSettings,
  ShowHideClock,
  ToggleStayOnTop,
};

Q_ENUM_NS(TrayIconAction)

enum GlobalOptions {
  // general
  Locale,

  ActiveInstances,
  RefInstance,
  OptionsSharing,

  Plugins,

  // behavior
  StayOnTop,
  FullscreenDetect,
  TransparentForMouse,

  SnapToEdge,
  SnapThreshold,
  PreventOutOfScreen,

  ChangeCursorOnHover,

  TransparentOnHover,
  OpacityOnHover,

  TrayIconSingleClickAction,
  TrayIconDoubleClickAction,

  // updater
  CheckForUpdates,
  CheckForBetaVersion,
  UpdatePeriodDays,

  // limits
  SkinBaseSize,
};

Q_ENUM_NS(GlobalOptions)

} // namespace app
} // namespace opt
