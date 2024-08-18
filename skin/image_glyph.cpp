/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "image_glyph.hpp"

#include <QPainter>

void RasterGlyph::setDevicePixelRatio(qreal dpr)
{
  _pixmap.setDevicePixelRatio(dpr);
  setRect({{0, 0}, _pixmap.deviceIndependentSize()});
  setAdvance(advance()/dpr);
}

void RasterGlyph::draw(QPainter* p) const
{
  p->drawPixmap(rect().toRect(), _pixmap);
}

void VectorGlyph::draw(QPainter* p) const
{
  _renderer->render(p, rect());
}
