// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "skin_engine_global.hpp"

#include <QtGui/QColor>
#include <QtGui/QGradient>
#include <QtGui/QPixmap>

SKIN_ENGINE_EXPORT
QGradient sample_conical_gradient();
SKIN_ENGINE_EXPORT
QGradient sample_linear_gradient();
SKIN_ENGINE_EXPORT
QPixmap sample_pattern();

SKIN_ENGINE_EXPORT
QPixmap solid_color_texture(const QColor& c);
SKIN_ENGINE_EXPORT
QPixmap gradient_texture(const QGradient& g, const QSize& sz);
inline QPixmap gradient_texture(const QGradient &g, int sz = 128)
{ return gradient_texture(g, QSize(sz, sz)); }

SKIN_ENGINE_EXPORT
QPixmap pattern_from_file(const QString& filename);
