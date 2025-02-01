// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tray_icon_manager_impl.hpp"

TrayIconManagerImpl::TrayIconManagerImpl(ClockTrayIcon& t, QObject* parent)
  : TrayIconManager(parent)
  , _tray_icon(t)
{
}

void TrayIconManagerImpl::saveTrayIconState()
{
  if (_counter++ == 0)
    _tray_icon.stopUpdating();
}

void TrayIconManagerImpl::restoreTrayIconState()
{
  if (--_counter == 0)
    _tray_icon.resumeUpdating();
  emit trayIconRestored();
}
