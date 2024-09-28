/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QString>
#include <QVariant>

namespace dc5 {

void exportToFile(const QVariantHash& settings, const QString& filename);
void importFromFile(QVariantHash& settings, const QString& filename);

} // namespace dc5
