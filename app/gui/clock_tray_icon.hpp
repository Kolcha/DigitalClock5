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
  static constexpr const QColor def_icon_color = QColor(101, 99, 255);

  explicit ClockTrayIcon(QObject* parent = nullptr);
  ~ClockTrayIcon();

  QColor currentColor() const { return m_icon_color; }

  bool usesCustomColor() const noexcept { return m_use_custom_color; }
  bool followsSystemTheme() const noexcept { return !usesCustomColor(); }

public slots:
  // generally, these methods are not required,
  // but because plugins have access to the tray icon
  // and may also update it, it is necessary to stop
  // time-based icon updates to allow plugin control the icon
  // updates are enabled by default
  void stopUpdating();
  void resumeUpdating();

  void useCustomColor(bool enable);
  void setCustomColor(QColor color);

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
  QColor m_icon_color = def_icon_color;
  QColor m_last_custom_color = def_icon_color;
  bool m_use_custom_color = false;
};
