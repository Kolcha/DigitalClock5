/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "build_date.hpp"

QDate build_date()
{
  QString build_date = QString::fromLatin1(__DATE__);
  // date is space-padded instead of zero-padded
  if (build_date[4].isSpace()) build_date[4] = QLatin1Char('0');
  return QDate::fromString(build_date, QLatin1String("MMM dd yyyy"));
}
