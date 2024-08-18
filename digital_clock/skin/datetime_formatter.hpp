/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDateTime>
#include <QStringView>

class DateTimeStringBuilder {
public:
  virtual ~DateTimeStringBuilder() = default;

  virtual void addCharacter(char32_t) {}
  virtual void addSeparator(char32_t) {}

  virtual void tokenStart(QStringView token) {}
  virtual void tokenEnd(QStringView token) {}
};

// in format string only ':' is considered as separator
void FormatDateTime(const QDateTime& dt, QStringView fmt,
                    DateTimeStringBuilder& str_builder);
