// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "skinned_widget.hpp"

#include <QtGui/QPainter>

SkinnedWidgetBase::SkinnedWidgetBase(LinesRenderer& r, QWidget* parent)
  : QWidget(parent)
  , _rr(r)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QSize SkinnedWidgetBase::sizeHint() const
{
  auto s = QSize(200, 100);
  if (_r) {
    auto sf = _r->geometry().size();
    s = QSize(sf.width() * _sx, sf.height() * _sy);
  }
  return s;
}

QPoint SkinnedWidgetBase::origin() const
{
  return {qRound(_origin.x() * _sx), qRound(_origin.y() * _sy)};
}

void SkinnedWidgetBase::setTextureCustomization(LinesRenderer::CustomizationType c)
{
  _rr.setTextureCustomization(c);
  doRepaint();
}

void SkinnedWidgetBase::setBackgroundCustomization(LinesRenderer::CustomizationType c)
{
  _rr.setBackgroundCustomization(c);
  doRepaint();
}

void SkinnedWidgetBase::setTexture(QPixmap p, QTileRules t)
{
  _rr.setTexture(std::move(p), t);
  doRepaint();
}

void SkinnedWidgetBase::setBackground(QPixmap p, QTileRules t)
{
  _rr.setBackground(std::move(p), t);
  doRepaint();
}

void SkinnedWidgetBase::setCharSpacing(qreal s)
{
  _rr.setCharSpacing(s);
  doUpdateGeometry();
}

void SkinnedWidgetBase::setLineSpacing(qreal s)
{
  _rr.setLineSpacing(s);
  doUpdateGeometry();
}

void SkinnedWidgetBase::setRespectLineSpacing(bool en)
{
  _rr.setRespectLineSpacing(en);
  doUpdateGeometry();
}

void SkinnedWidgetBase::setTruePerCharRendering(bool en)
{
  _rr.setTruePerCharRendering(en);
  doUpdateGeometry();
}

void SkinnedWidgetBase::setCharMargins(const QMarginsF& m)
{
  _rr.setCharMargins(m);
  doUpdateGeometry();
}

void SkinnedWidgetBase::setTextMargins(const QMarginsF& m)
{
  _rr.setTextMargins(m);
  doUpdateGeometry();
}

void SkinnedWidgetBase::setSkin(std::shared_ptr<Skin> skin)
{
  _rr.setSkin(std::move(skin));
  doUpdateGeometry();
}

void SkinnedWidgetBase::setScaling(qreal sx, qreal sy)
{
  _sx = sx;
  _sy = sy;
  doUpdateGeometry();
}

void SkinnedWidgetBase::setFrameEnabled(bool enabled)
{
  _frame_visible = enabled;
  update();
}

void SkinnedWidgetBase::paintEvent(QPaintEvent* event)
{
  Q_UNUSED(event);
  if (!_r) return;

  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::SmoothPixmapTransform);

  p.scale(_sx, _sy);

  auto g = _r->geometry();
  p.translate(-g.topLeft());
  _r->draw(&p);

  if (_frame_visible) {
    p.resetTransform();
    p.setPen(Qt::red);
    p.setBrush(Qt::NoBrush);
    p.drawRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5));
  }
}

void SkinnedWidgetBase::doUpdateGeometry()
{
  _r = buildRenderable();
  if (!_r) return;
  _origin = -_r->geometry().topLeft();
  updateGeometry();
  update();
}

void SkinnedWidgetBase::doRepaint()
{
  _r = buildRenderable();
  if (!_r) return;
  _origin = -_r->geometry().topLeft();
  update();
}


SkinnedTextWidget::SkinnedTextWidget(QWidget* parent)
  : SkinnedTextWidget(std::make_unique<TextRenderer>(), parent)
{
}

void SkinnedTextWidget::drawText(QString txt, LinesRenderer::Options opt)
{
  _txt = std::move(txt);
  _opt = std::move(opt);
  doUpdateGeometry();
}

std::unique_ptr<Renderable> SkinnedTextWidget::buildRenderable()
{
  return _rr->draw(_txt, _opt);
}

SkinnedTextWidget::SkinnedTextWidget(std::unique_ptr<TextRenderer> r, QWidget* parent)
  : SkinnedWidgetBase(*r, parent)
  , _rr(std::move(r))
{
}
