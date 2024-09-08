/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sample_brushes.hpp"

#include <QPainter>

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
  pxm.fill(QColor(160, 0, 160));
  QPainter p(&pxm);
  p.fillRect(0, 0, 4, 4, QColor(224, 0, 224));
  p.fillRect(4, 4, 8, 8, QColor(224, 0, 224));
  return pxm;
}
