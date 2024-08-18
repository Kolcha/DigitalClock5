/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QtCore/qglobal.h>

#if defined(SKIN_ENGINE_LIBRARY)
#define SKIN_ENGINE_EXPORT Q_DECL_EXPORT
#else
#define SKIN_ENGINE_EXPORT Q_DECL_IMPORT
#endif
