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
  updateIcon();
  auto timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &ClockTrayIcon::updateIcon);
  timer->start(1000);
}

void ClockTrayIcon::updateIcon()
{
  auto now = QTime::currentTime();

  if (compareTime(now, m_last_update))
    return;

  QIcon tray_icon(new ClockIconEngine);
#ifdef Q_OS_MACOS
  tray_icon.setIsMask(true);
#endif
  setIcon(tray_icon);

  auto tstr = QLocale::system().toString(now, QLocale::ShortFormat);
  auto dstr = QLocale::system().toString(QDate::currentDate());
  setToolTip(QString("%1\n%2").arg(tstr, dstr));

  m_last_update = std::move(now);
}
