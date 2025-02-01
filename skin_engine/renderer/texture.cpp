// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "texture.hpp"

#include <QtGui/QPainter>

QGradient sample_conical_gradient()
{
  QConicalGradient g(0.5, 0.5, 45.0);
  g.setStops({
    {0.00, {170,   0,   0}},  // #aa0000
    {0.20, {  0,  85, 255}},  // #0055ff
    {0.45, {  0, 170,   0}},  // #00aa00
    {0.65, {255, 255,   0}},  // #ffff00
    {1.00, {170,   0,   0}},  // #aa0000
  });
  g.setCoordinateMode(QGradient::ObjectMode);
  return g;
}

QGradient sample_linear_gradient()
{
  QLinearGradient g(0., 0., 0., 1.);
  g.setColorAt(0.0, Qt::transparent);
  g.setColorAt(0.6, Qt::transparent);
  g.setColorAt(1.0, QColor(85, 0, 255));
  g.setCoordinateMode(QGradient::ObjectMode);
  return g;
}

QPixmap sample_pattern()
{
  QPixmap pxm(8, 8);
  pxm.fill(QColor(0x4C, 0x4A, 0xBF));
  QPainter p(&pxm);
  p.fillRect(0, 0, 4, 4, QColor(0x8C, 0x8A, 0xFF));
  p.fillRect(4, 4, 8, 8, QColor(0x8C, 0x8A, 0xFF));
  return pxm;
}

QPixmap solid_color_texture(const QColor& c)
{
  QPixmap pxm(8, 8);
  pxm.fill(c);
  return pxm;
}

QPixmap gradient_texture(const QGradient& g, const QSize& sz)
{
  QPixmap pxm(sz);
  pxm.fill(Qt::transparent);
  QPainter p(&pxm);
  p.fillRect(pxm.rect(), g);
  return pxm;
}

QPixmap pattern_from_file(const QString& filename)
{
  if (filename.isEmpty())
    return sample_pattern();

  QPixmap pxm(filename);

  if (pxm.isNull() || pxm.rect().isEmpty())
    return sample_pattern();

  return pxm;
}
