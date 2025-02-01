// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "common/tray_icon_manager.hpp"

#include "gui/clock_tray_icon.hpp"

class TrayIconManagerImpl : public TrayIconManager
{
  Q_OBJECT

public:
  explicit TrayIconManagerImpl(ClockTrayIcon& t, QObject* parent = nullptr);

  QSystemTrayIcon* rawTrayIcon() noexcept override { return &_tray_icon; }

public slots:
  void saveTrayIconState() override;
  void restoreTrayIconState() override;

private:
  ClockTrayIcon& _tray_icon;
  int _counter = 0;
};
