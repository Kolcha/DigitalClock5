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
  if (_bg == b && _bg_s == stretch) return;
  _bg = std::move(b);
  _bg_s = stretch;
  notify(&AppearanceChangeListener::onAppearanceChanged);
}

void Appearance::setTexture(QBrush b, bool stretch)
{
  if (_tx == b && _tx_s == stretch) return;
  _tx = std::move(b);
  _tx_s = stretch;
  notify(&AppearanceChangeListener::onAppearanceChanged);
}


void Transform::setTransform(QTransform t)
{
  if (_t == t) return;
  _t = std::move(t);
  notify(&AppearanceChangeListener::onAppearanceChanged);
}


Glyph::Glyph(std::shared_ptr<Skin::Glyph> g)
    : _g(std::move(g))
    , _t(std::make_shared<Transform>())
    , _a(std::make_shared<Appearance>())
{
  _t->subscribe(this);
  _a->subscribe(this);
  updateGeometry();
}

Glyph::~Glyph()
{
  _a->unsubscribe(this);
  _t->unsubscribe(this);
}

void Glyph::setGlyph(std::shared_ptr<Skin::Glyph> g)
{
  if (_g == g) return;
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
  if (_t == t) return;
  _t->unsubscribe(this);
  _t = std::move(t);
  _t->subscribe(this);
  updateGeometry();
  dropCachedData();
}

void Glyph::setResizeEnabled(bool enabled)
{
  _resize_enabled = enabled;
  if (!enabled) _ks = 1.0;
  updateGeometry();
}

void Glyph::resize(qreal l, Qt::Orientation o)
{
  Q_ASSERT(_resize_enabled);
  if (o == Qt::Horizontal)
    _ks *= l / _br.width();
  else
    _ks *= l / _br.height();
  updateGeometry();
}

void Glyph::setAppearance(std::shared_ptr<Appearance> a)
{
  if (_a == a) return;
  _a->unsubscribe(this);
  _a = std::move(a);
  _a->subscribe(this);
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
  p->scale(_ks, _ks);
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

void Glyph::onAppearanceChanged()
{
  dropCachedData();
}

void Glyph::updateGeometry()
{
  if (!_g) return;

  _br = _t->transform().scale(_ks, _ks).mapRect(_g->rect());
  _gr = _br.translated(_pos);
  _adv = _t->transform().scale(_ks, _ks).map(_g->advance());
}

void Glyph::dropCachedData()
{
  QPixmapCache::remove(_ck);
}
