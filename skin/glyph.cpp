/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "glyph.hpp"

#include <QPainter>
#include <QPixmap>

void Appearance::setBackground(QBrush b, bool stretch)
{
  _bg = std::move(b);
  _bg_s = stretch;
}

void Appearance::setTexture(QBrush b, bool stretch)
{
  _tx = std::move(b);
  _tx_s = stretch;
}


Glyph::Glyph(std::shared_ptr<Skin::Glyph> g)
    : _g(std::move(g))
    , _t(std::make_shared<Transform>())
    , _a(std::make_shared<Appearance>())
{
  updateGeometry();
}

void Glyph::setGlyph(std::shared_ptr<Skin::Glyph> g)
{
  _g = std::move(g);
  updateGeometry();
  dropCachedData();
}

void Glyph::setPos(QPointF p)
{
  _pos = std::move(p);
  _gr = _br.translated(_pos);
}

void Glyph::setTransform(std::shared_ptr<Transform> t)
{
  _t = std::move(t);
  updateGeometry();
  dropCachedData();
}

void Glyph::setAppearance(std::shared_ptr<Appearance> a)
{
  _a = std::move(a);
  dropCachedData();
}


void maybeDrawStretched(QPainter* p, const QRectF& r, const QBrush& b, bool stretch)
{
  if (auto tx = b.texture(); !tx.isNull() && stretch) {
    p->drawPixmap(r, tx, tx.rect());
  } else {
    p->fillRect(r, b);
  }
}


void Glyph::draw(QPainter* p) const
{
  if (!_g) return;

  if (!_vis) return;

  if (_g->rect().isEmpty()) return;

  // drop cached data if world transform changes
  if (auto h = qHash(p->transform()); _ht != h) {
    QPixmapCache::remove(_ck);
    _ht = h;
  }

  p->save();
  p->translate(_pos);
  p->setTransform(_t->transform(), true);
  QRectF br = p->transform().mapRect(_g->rect());

  QPixmap pxm;
  if (!QPixmapCache::find(_ck, &pxm)) {
    qreal dpr = p->device()->devicePixelRatioF();
    pxm = QPixmap((br.size() * dpr).toSize());
    pxm.setDevicePixelRatio(dpr);
    pxm.fill(Qt::transparent);
    {
      QPainter pp(&pxm);
      pp.setPen(p->pen());
      pp.setBrush(p->brush());
      pp.setRenderHints(p->renderHints());
      pp.translate(-br.topLeft());
      pp.setTransform(p->transform(), true);
      _g->draw(&pp);
      // do texturing
      pp.setCompositionMode(QPainter::CompositionMode_SourceIn);
      maybeDrawStretched(&pp, _g->rect(), _a->texture(), _a->textureStretch());
      // apply backgound
      pp.setCompositionMode(QPainter::CompositionMode_DestinationOver);
      maybeDrawStretched(&pp, _g->rect(), _a->background(), _a->backgroundStretch());
    }

    if (_caching) _ck = QPixmapCache::insert(pxm);
  }

  p->resetTransform();
  p->translate(br.topLeft());
  p->drawPixmap(0, 0, pxm);
  p->restore();
}

void Glyph::updateGeometry()
{
  if (!_g) return;

  _br = _t->transform().mapRect(_g->rect());
  _gr = _br.translated(_pos);
  _adv = _t->transform().map(_g->advance());
}

void Glyph::dropCachedData()
{
  QPixmapCache::remove(_ck);
}
