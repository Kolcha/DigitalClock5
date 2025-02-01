// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "renderable.hpp"

#include <algorithm>
#include <functional>
#include <utility>

static QRectF calculate_rect(const CompositeRenderable::ContainerType& c)
{
  QRectF rect;
  if (!c.empty()) {
    // *INDENT-OFF*
    rect = std::transform_reduce(c.begin(), c.end(),
                                 c.front()->geometry(),
                                 std::bit_or{},
                                 [](const auto& r) { return r->geometry(); });
    // *INDENT-ON*
  }
  return rect;
}

CompositeRenderable::CompositeRenderable(ContainerType&& c)
  : _c(std::move(c))
  , _rect(calculate_rect(_c))
  , _dirty(false)
{}

QRectF CompositeRenderable::geometry() const
{
  if (_dirty) {
    _rect = calculate_rect(_c);
    _dirty = false;
  }
  return _rect;
}

void CompositeRenderable::draw(QPainter* p) const
{
  std::ranges::for_each(_c, [p](const auto& r) { r->draw(p); });
}

void CompositeRenderable::add(std::unique_ptr<Renderable> r)
{
  Q_ASSERT(r);
  _c.push_back(std::move(r));
  _dirty = true;
}
