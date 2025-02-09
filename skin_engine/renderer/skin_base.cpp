// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "skin_base.hpp"

#include <QtCore/QtMath>
#include <QtWidgets/qdrawutil.h>

#include "painter_guard.hpp"

static QRect fit_rect(const QRectF& rf)
{
  auto t = qFloor(rf.top());
  auto l = qFloor(rf.left());
  auto b = qCeil(rf.bottom());
  auto r = qCeil(rf.right());
  return QRect(QPoint(l, t), QPoint(r, b));
}

void SkinBase::setTexture(const QPixmap& b, const QTileRules& t)
{
  _texture = b;
  _tx_rules = t;
}

void SkinBase::setBackground(const QPixmap& b, const QTileRules& t)
{
  _background = b;
  _bg_rules = t;
}

void SkinRenderable::draw(QPainter* p) const
{
  PainterGuard _(*p);

  const auto& t = p->transform();
  const auto br = t.mapRect(geometry());

  const auto pxm_size = br.size() * p->device()->devicePixelRatio();
  QPixmap pxm(qCeil(pxm_size.width()), qCeil(pxm_size.height()));
  pxm.setDevicePixelRatio(p->device()->devicePixelRatio());
  pxm.fill(Qt::transparent);

  // due to transform and floating point part truncation,
  // size may become 0 in one or both directions
  // QPainter "doesn't like" such surfaces and goes crazy,
  // even may not paint on other surfaces anymore
  // so, avoid passing such things to QPainter
  if (pxm.rect().isEmpty())
    return;

  {
    QPainter pp(&pxm);
    pp.setRenderHints(p->renderHints());
    pp.setPen(p->pen());
    pp.setBrush(p->brush());
    pp.translate(-br.topLeft());
    pp.setTransform(t, true);

    inner()->draw(&pp);

    if (!_texture.isNull()) {
      pp.setCompositionMode(QPainter::CompositionMode_SourceIn);
      qDrawBorderPixmap(&pp, fit_rect(geometry()), QMargins(),
                        _texture, _texture.rect(), QMargins(),
                        _tx_rules);
    }

    if (!_background.isNull()) {
      pp.setCompositionMode(QPainter::CompositionMode_DestinationOver);
      qDrawBorderPixmap(&pp, fit_rect(geometry()), QMargins(),
                        _background, _background.rect(), QMargins(),
                        _bg_rules);
    }
  }

  p->resetTransform();
  p->translate(br.topLeft());
  p->drawPixmap(0, 0, pxm);
}

void SkinRenderable::setTexture(QPixmap b, QTileRules t) noexcept
{
  _texture = std::move(b);
  _tx_rules = std::move(t);
}

void SkinRenderable::setBackground(QPixmap b, QTileRules t) noexcept
{
  _background = std::move(b);
  _bg_rules = std::move(t);
}
