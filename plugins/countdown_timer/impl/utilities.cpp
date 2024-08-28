/*
 * SPDX-FileCopyrightText: 2017-2020 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "utilities.hpp"

namespace countdown_timer {

QString format_time(qint64 time_left, int days_threshold, bool hide_hours)
{
  qint64 h = time_left / 3600;
  qint64 m = (time_left - h * 3600) / 60;
  qint64 s = time_left - (h * 3600 + m * 60);

  qint64 days_left = h / 24;

  int fw = 2;             // field width
  int b = 10;             // base
  QLatin1Char ch('0');    // filler

  if (days_left < days_threshold || days_threshold == -1) {
    if (hide_hours)
      return QString("%1:%2").arg(m + h * 60).arg(s, fw, b, ch);
    else
      return QString("%1:%2:%3").arg(h).arg(m, fw, b, ch).arg(s, fw, b, ch);
  }

  return QString("%1:%2:%3:%4")
         .arg(days_left)
         .arg(h - days_left * 24, fw, b, ch)
         .arg(m, fw, b, ch)
         .arg(s, fw, b, ch);
}

} // namespace countdown_timer
