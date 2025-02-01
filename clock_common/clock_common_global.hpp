/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QtCore/QtCompilerDetection>

#if defined(CLOCK_COMMON_LIBRARY)
#define CLOCK_COMMON_EXPORT Q_DECL_EXPORT
#else
#define CLOCK_COMMON_EXPORT Q_DECL_IMPORT
#endif
