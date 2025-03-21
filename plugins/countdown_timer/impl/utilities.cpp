/*
 * SPDX-FileCopyrightText: 2017-2020 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "utilities.hpp"

namespace countdown_timer {

namespace {

constexpr qint64 int_pow(qint64 v, int p) noexcept
{
  while (p-- > 0) v *= v;
  return v;
}

} // namespace

QString format_time(qint64 time_left, const fmt_options& opt)
{
  qint64 h = time_left / 3600;
  qint64 m = (time_left - h * 3600) / 60;
  qint64 s = time_left - (h * 3600 + m * 60);

  qint64 days_left = h / 24;

  constexpr int fw = 2;             // field width
  constexpr int b = 10;             // base
  constexpr QLatin1Char ch('0');    // filler

  auto maybe_pad = [&](qint64 v) -> QString {
    if (opt.leading_zero && v < int_pow(b, fw))
      return QString("%1").arg(v, fw, b, ch);
    return QString::number(v, b);
  };

  if (days_left < opt.days_threshold || opt.days_threshold == -1) {
    if (opt.hide_hours)
      return QString("%1:%2").arg(maybe_pad(m + h * 60)).arg(s, fw, b, ch);
    else
      return QString("%1:%2:%3").arg(maybe_pad(h)).arg(m, fw, b, ch).arg(s, fw, b, ch);
  }

  return QString("%1:%2:%3:%4")
         .arg(maybe_pad(days_left))
         .arg(h - days_left * 24, fw, b, ch)
         .arg(m, fw, b, ch)
         .arg(s, fw, b, ch);
}

} // namespace countdown_timer
