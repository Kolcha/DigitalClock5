// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "clock_common_global.hpp"

#include <QtCore/QObject>

class QSystemTrayIcon;

class CLOCK_COMMON_EXPORT TrayIconManager : public QObject
{
  Q_OBJECT

public:
  explicit TrayIconManager(QObject* parent = nullptr)
    : QObject(parent)
  {}

  virtual QSystemTrayIcon* rawTrayIcon() = 0;

public slots:
  virtual void saveTrayIconState() = 0;
  virtual void restoreTrayIconState() = 0;

signals:
  void trayIconRestored();
};
