/*
 * SPDX-FileCopyrightText: 2017-2020 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QString>

namespace countdown_timer {

QString format_time(qint64 time_left, int days_threshold = -1, bool hide_hours = false);

} // namespace countdown_timer
