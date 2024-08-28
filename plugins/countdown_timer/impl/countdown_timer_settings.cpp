/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "countdown_timer_settings.hpp"

namespace plugin_impl {

QDateTime default_target_date()
{
  return QDateTime(QDate(QDate::currentDate().year() + 1, 1, 1), QTime(0, 0, 0));
}

} // namespace plugin_impl
