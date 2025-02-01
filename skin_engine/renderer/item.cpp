// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "item.hpp"

#include "painter_guard.hpp"

Item::Item(std::unique_ptr<Renderable> r, QPointF p) noexcept
  : _r(std::move(r))
  , _pos(std::move(p))
{}

void Item::draw(QPainter* p) const
{
  if (!_visible)
    return;

  PainterGuard _(*p);
  p->translate(_pos);
  p->setTransform(_transform, true);
  _r->draw(p);
}

QRectF Item::rect() const
{
  return _transform.mapRect(_r->geometry().marginsAdded(_margins));
}
