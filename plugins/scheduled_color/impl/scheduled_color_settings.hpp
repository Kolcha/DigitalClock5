/*
 * SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QColor>
#include <QTime>

namespace scheduled_color {

struct TimeColorPoint {
  QTime time;
  QColor color;

  enum DurationMode {
    FixedDuration = 0,
    UntilTime = 1,
    UntilNextPoint = 2
  };

  DurationMode mode = UntilNextPoint;
  QTime until_time;
  int duration_seconds = 0;

  bool operator<(const TimeColorPoint& other) const
  {
    return time < other.time;
  }
};

} // namespace scheduled_color
