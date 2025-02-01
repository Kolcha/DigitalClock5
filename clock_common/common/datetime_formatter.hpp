/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "clock_common_global.hpp"

#include <QtCore/QDateTime>
#include <QtCore/QString>

class CLOCK_COMMON_EXPORT DateTimeStringBuilder
{
public:
  virtual ~DateTimeStringBuilder() = default;

  virtual void addCharacter(char32_t ch) {}
  virtual void addSeparator(char32_t ch) {}

  virtual void tokenStart(const QString& token) {}
  virtual void tokenEnd(const QString& token) {}
};

CLOCK_COMMON_EXPORT
// in format string only ':' is considered as separator
void FormatDateTime(const QDateTime& dt, const QString& fmt,
                    DateTimeStringBuilder& str_builder);
