/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "skin.hpp"

namespace {

class GlyphDecoratorBase : public Skin::Glyph {
public:
  explicit GlyphDecoratorBase(std::shared_ptr<Glyph> g)
      : _g(std::move(g))
  {
    Q_ASSERT(_g);
  }

  std::shared_ptr<Glyph> inner() const { return _g; }

  QRectF rect() const override { return _g->rect(); }
  QPointF advance() const override { return _g->advance(); }

  void draw(QPainter* p) const override { _g->draw(p); }

private:
  std::shared_ptr<Glyph> _g;
};

} // namespace

class Skin::DecoratedGlyph : public GlyphDecoratorBase {
public:
  using GlyphDecoratorBase::GlyphDecoratorBase;

  QMarginsF margins() const { return _mgs; }

  void setMargins(QMarginsF mgs)
  {
    if (_mgs == mgs) return;
    _mgs = std::move(mgs);
    notify(&GeometryChangeListener::onGeometryChanged);
  }

  QRectF rect() const override { return inner()->rect().marginsAdded(_mgs); }

private:
  QMarginsF _mgs;
};


std::shared_ptr<Skin::Glyph> Skin::glyph(char32_t c) const
{
  if (auto iter = _cache.find(c); iter != _cache.end())
    return iter.value();

  std::shared_ptr<DecoratedGlyph> cached_glyph;
  if (auto g = create(c)) {
    cached_glyph = std::make_shared<DecoratedGlyph>(std::move(g));
    cached_glyph->setMargins(_mgs);
    _cache[c] = cached_glyph;
  }
  return cached_glyph;
}

void Skin::setMargins(QMarginsF mgs)
{
  if (_mgs == mgs) return;
  for (auto& g : std::as_const(_cache)) {
    Q_ASSERT(g);
    g->setMargins(mgs);
  }
  _mgs = std::move(mgs);
}
