/*
 * SPDX-FileCopyrightText: 2017-2020 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QString>

namespace countdown_timer {

struct fmt_options {
  int days_threshold = -1;
  bool hide_hours = false;
  bool leading_zero = false;
};

QString format_time(qint64 time_left, const fmt_options& opt);

} // namespace countdown_timer
