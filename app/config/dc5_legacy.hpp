// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QSettings>
#include <QVariantHash>

namespace compat {

QVariantHash mapFromDC5Legacy(const QVariantHash& settings);
void migrateConfig(QSettings& settings);

} // namespace compat
