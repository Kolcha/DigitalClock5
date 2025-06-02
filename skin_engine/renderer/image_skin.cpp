// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "image_skin.hpp"

#include <QtGui/QPainter>

#include "item.hpp"

GlyphBase::GlyphBase(const QRectF& r) noexcept
  : _rect(r)
  , _adv(_rect.width(), -_rect.height())
{}


RasterGlyph::RasterGlyph(const QString& filename)
  : RasterGlyph(QPixmap(filename))
{}

RasterGlyph::RasterGlyph(QPixmap pxm) noexcept
  : GlyphBase(pxm.rect())
  , _pixmap(std::move(pxm))
{}

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


VectorGlyph::VectorGlyph(const QString& filename)
  : VectorGlyph(std::make_unique<QSvgRenderer>(filename))
{}

VectorGlyph::VectorGlyph(std::unique_ptr<QSvgRenderer> rr)
  : GlyphBase({{0, 0}, rr->defaultSize()})
  , _renderer(std::move(rr))
{}

void VectorGlyph::draw(QPainter* p) const
{
  _renderer->render(p, rect());
}


namespace {

class GlyphDecorator : public Glyph
{
public:
  explicit GlyphDecorator(std::shared_ptr<Glyph> g) noexcept
    : _g(std::move(g))
  {}

  QRectF rect() const override { return _g->rect(); }
  QPointF advance() const override { return _g->advance(); }

  void draw(QPainter* p) const override { return _g->draw(p); }

protected:
  const auto& inner() const noexcept { return _g; }

private:
  std::shared_ptr<Glyph> _g;
};


class CachedGlyph : public GlyphDecorator
{
public:
  using GlyphDecorator::GlyphDecorator;

  void draw(QPainter* p) const override;

private:
  mutable QPixmap _cache;
};


void CachedGlyph::draw(QPainter* p) const
{
  const qreal dpr = p->device()->devicePixelRatio();
  const auto& t = p->transform();
  const qreal k = std::max(t.m11(), t.m22()) * dpr;
  const auto sz = k * inner()->rect().size();
  const auto r_sz = QSize(qCeil(sz.width()), qCeil(sz.height()));
  const auto c_sz = _cache.size();

  if (r_sz.width() > c_sz.width() || r_sz.height() > c_sz.height()) {
    _cache = QPixmap(r_sz);
    _cache.fill(Qt::transparent);
    QPainter pp(&_cache);
    pp.scale(k, k);
    pp.translate(-rect().topLeft());
    pp.setRenderHints(p->renderHints());
    inner()->draw(&pp);
  }

  p->drawPixmap(rect().toRect(), _cache);
}


class GlyphRenderable : public Renderable
{
public:
  explicit GlyphRenderable(std::shared_ptr<Glyph> g) noexcept
    : _g(std::move(g))
  {
    Q_ASSERT(_g);
  }

  QRectF geometry() const override { return _g->rect(); }
  void draw(QPainter* p) const override { _g->draw(p); }

private:
  std::shared_ptr<Glyph> _g;
};

} // namespace

void ImageSkin::insertGlyph(char32_t c, std::shared_ptr<Glyph> g)
{
  _glyphs[c] = std::make_shared<CachedGlyph>(std::move(g));
}

std::shared_ptr<Glyph> ImageSkin::glyph(char32_t c) const noexcept
{
  return _glyphs.value(c, nullptr);
}

std::unique_ptr<Renderable> ImageSkin::draw(const QString& str) const
{
  auto r = std::make_unique<CompositeRenderable>();

  const auto cps = str.toUcs4();

  qreal x = 0;
  for (const auto c : cps) {
    auto g = glyph(static_cast<char32_t>(c));
    if (!g) continue;

    auto adv = g->advance().x();
    auto sym = std::make_unique<GlyphRenderable>(std::move(g));

    auto item = std::make_unique<Item>(maybeCustomize(std::move(sym), perCharRendering()));
    item->setMargins(charMargins());
    item->setPos({x, 0});
    x += adv + charSpacing();

    r->add(std::move(item));
  }

  return maybeCustomize(std::move(r), !perCharRendering());
}

Skin::Metrics ImageSkin::metrics() const
{
  Metrics m{};

  if (!_glyphs.empty()) {
    auto first_iter = _glyphs.begin();
    const auto& first = first_iter.value();

    auto ymin = first->rect().top();
    auto ymax = first->rect().bottom();

    auto yadv = qAbs(first->advance().y());

    for (auto iter = std::next(first_iter); iter != _glyphs.end(); ++iter) {
      const auto& glyph = iter.value();
      ymin = std::min(ymin, glyph->rect().top());
      ymax = std::max(ymax, glyph->rect().bottom());
      yadv = std::max(yadv, qAbs(glyph->advance().y()));
    }

    m.ascent = -ymin;
    m.descent = ymax;
    m.line_spacing = yadv;
  }

  return m;
}

std::unique_ptr<Renderable> ImageSkin::maybeCustomize(std::unique_ptr<Renderable> r,
                                                      bool enable) const
{
  return enable ? customize(std::move(r)) : std::move(r);
}

std::unique_ptr<Renderable> ImageSkin::customize(std::unique_ptr<Renderable> r) const
{
  auto sr = std::make_unique<SkinRenderable>(std::move(r));
  sr->setTexture(texture(), textureRules());
  sr->setBackground(background(), backgroundRules());
  return sr;
}
