/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <memory>
#include <QTranslator>

std::unique_ptr<QTranslator> findTranslation(QStringView module);
std::unique_ptr<QTranslator> loadTranslation(QStringView module, QStringView locale);
