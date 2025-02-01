/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QSystemTrayIcon>

#include <QTime>
#include <QTimer>

#ifdef Q_OS_WINDOWS
#include "platform/win/system_theme_tracker.hpp"
#endif

class ClockTrayIcon : public QSystemTrayIcon
{
  Q_OBJECT
public:
  explicit ClockTrayIcon(QObject* parent = nullptr);
  ~ClockTrayIcon();

public slots:
  // generally, these methods are not required,
  // but because plugins have access to the tray icon
  // and may also update it, it is necessary to stop
  // time-based icon updates to allow plugin control the icon
  // updates are enabled by default
  void stopUpdating();
  void resumeUpdating();

private slots:
  void updateIcon();
#ifdef Q_OS_WINDOWS
  void onSystemThemeChanged(bool is_light);
#endif

private:
  void repaintIcon();

private:
  QTimer m_timer;
  QTime m_last_update;
#ifdef Q_OS_WINDOWS
  SystemThemeTracker m_sys_theme_tracker;
#endif
  QColor m_icon_color = QColor(101, 99, 255);
};
