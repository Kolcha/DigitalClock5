/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QSystemTrayIcon>

#include <QTime>

#ifdef Q_OS_WINDOWS
#include "platform/win/system_theme_tracker.hpp"
#endif

class ClockTrayIcon : public QSystemTrayIcon
{
  Q_OBJECT
public:
  explicit ClockTrayIcon(QObject* parent = nullptr);
  ~ClockTrayIcon();

private slots:
  void updateIcon();
#ifdef Q_OS_WINDOWS
  void onSystemThemeChanged(bool is_light);
#endif

private:
  void repaintIcon();

private:
  QTime m_last_update;
#ifdef Q_OS_WINDOWS
  SystemThemeTracker m_sys_theme_tracker;
#endif
  QColor m_icon_color = QColor(101, 99, 255);
};
