/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "overlay_widget.hpp"

#include <QPainter>

OverlayWidget::OverlayWidget(QWidget* parent)
    : QWidget(parent)
{
}

void OverlayWidget::setFrameVisible(bool vis)
{
  _frame_visible = vis;
  update();
}

void OverlayWidget::paintEvent(QPaintEvent* event)
{
  QWidget::paintEvent(event);

  if (_frame_visible) {
    QPainter p(this);
    p.setPen(QPen(Qt::red, 2, Qt::DotLine, Qt::SquareCap, Qt::MiterJoin));
    p.drawRect(rect().adjusted(1, 1, -1, -1));
  }
}
