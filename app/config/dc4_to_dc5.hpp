/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QString>
#include <QVariant>

namespace dc4 {

void importFromFile(QVariantMap& settings, const QString& filename);

} // namespace dc4

namespace compat {

QVariantHash mapFromDC4(const QVariantMap& settings, int idx);
void importFromFile(QVariantHash& settings, const QString& filename, int idx);

} // namespace compat
