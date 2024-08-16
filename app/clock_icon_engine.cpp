/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "clock_icon_engine.hpp"

#include <QtCore/QTime>
#include <QtGui/QPainter>

void ClockIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
  Q_UNUSED(mode);
  Q_UNUSED(state);

  qreal pw = rect.width() / 16.;
  qreal mw = 0.6 * rect.width() / 2.;
  qreal hw = 0.4 * rect.width() / 2.;
  QLineF ml(0, 0, 0, -mw);
  QLineF hl(0, 0, 0, -hw);
  QTime now = QTime::currentTime();

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(QPen(QColor(101, 99, 255), pw));

  painter->drawEllipse(QRectF(rect).adjusted(pw, pw, -pw, -pw));
  painter->translate(rect.center());
  {
    painter->save();
    painter->rotate(360 / 60 * now.minute());
    painter->drawLine(ml);
    painter->restore();
  }
  {
    qreal h = now.hour();
    if (h > 12) h -= 12;
    h += now.minute() / 60.;
    painter->save();
    painter->rotate(360 / 12 * h);
    painter->drawLine(hl);
    painter->restore();
  }
  painter->restore();
}

QPixmap ClockIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
  QPixmap pm(size);
  pm.fill(Qt::transparent);
  {
    QPainter p(&pm);
    paint(&p, QRect(QPoint(0,0),size), mode, state);
  }
  return pm;
}

QIconEngine* ClockIconEngine::clone() const
{
  return new ClockIconEngine;
}
