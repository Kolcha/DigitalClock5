/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "clock_tray_icon.hpp"

#include <QLocale>
#include <QTimer>

#include "clock_icon_engine.hpp"

inline bool compareTime(const QTime& t1, const QTime& t2) noexcept
{
  return t1.hour() == t2.hour() && t1.minute() == t2.minute();
}

ClockTrayIcon::ClockTrayIcon(QObject* parent)
  : QSystemTrayIcon(parent)
{
#ifdef Q_OS_WINDOWS
  m_sys_theme_tracker.start();
  onSystemThemeChanged(m_sys_theme_tracker.isLightTheme());
  connect(&m_sys_theme_tracker, &SystemThemeTracker::themeChanged,
          this, &ClockTrayIcon::onSystemThemeChanged);
#endif
  updateIcon();
  auto timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &ClockTrayIcon::updateIcon);
  timer->start(1000);
}

ClockTrayIcon::~ClockTrayIcon()
{
#ifdef Q_OS_WINDOWS
  m_sys_theme_tracker.stop();
  while (!m_sys_theme_tracker.isFinished()) QThread::msleep(1);
#endif
}

void ClockTrayIcon::updateIcon()
{
  auto now = QTime::currentTime();

  if (compareTime(now, m_last_update))
    return;

  repaintIcon();

  auto tstr = QLocale::system().toString(now, QLocale::ShortFormat);
  auto dstr = QLocale::system().toString(QDate::currentDate());
  setToolTip(QString("%1\n%2").arg(tstr, dstr));

  m_last_update = std::move(now);
}
#ifdef Q_OS_WINDOWS
void ClockTrayIcon::onSystemThemeChanged(bool is_light)
{
  m_icon_color = is_light ? QColor(24, 25, 26) : QColor(255, 255, 255);
  repaintIcon();
}
#endif
void ClockTrayIcon::repaintIcon()
{
  QIcon tray_icon(new ClockIconEngine(m_icon_color));
#ifdef Q_OS_MACOS
  tray_icon.setIsMask(true);
#endif
  setIcon(tray_icon);
}
