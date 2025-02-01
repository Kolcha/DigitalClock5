/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "logo_label.hpp"

#include <QConicalGradient>
#include <QPainter>
#include <QPixmap>
#include <QRandomGenerator>

static QPixmap apply_texture(const QPixmap& src, const QBrush& brush)
{
  QPixmap pxm = src.copy();
  QPainter p(&pxm);
  p.setCompositionMode(QPainter::CompositionMode_SourceIn);
  p.setPen(Qt::NoPen);
  p.setBrush(brush);
  p.drawRect(QRectF(QPointF(0., 0.), pxm.deviceIndependentSize()));
  return pxm;
}

static int gen_max_count()
{
  return QRandomGenerator::global()->bounded(2, 16);
}

static QColor random_color() noexcept
{
  return QColor::fromRgb(QRandomGenerator::global()->generate());
}

static QGradient conical_gradient()
{
  QConicalGradient g(0.5, 0.5, 45.0);
  g.setStops({
    {0.00, {255,   0,   0}},
    {0.45, {  0,   0, 255}},
    {0.60, {  0, 255,   0}},
    {0.75, {255, 255,   0}},
    {1.00, {255,   0,   0}},
  });
  g.setCoordinateMode(QGradient::ObjectMode);
  return g;
}

struct LogoLabel::Impl {
  QPixmap orig_pixmap;
  int clicks_count = 0;
  int max_clicks_count = 0;
};

LogoLabel::LogoLabel(QWidget* parent)
  : QLabel(parent)
  , _impl(std::make_unique<Impl>())
{
  setCursor(Qt::WhatsThisCursor);
}

LogoLabel::~LogoLabel() = default;

void LogoLabel::mouseReleaseEvent(QMouseEvent* event)
{
  if (_impl->orig_pixmap.isNull()) {
    _impl->orig_pixmap = pixmap();
    _impl->max_clicks_count = gen_max_count();
  }

  if (++_impl->clicks_count == _impl->max_clicks_count) {
    _impl->clicks_count = 0;
    _impl->max_clicks_count = gen_max_count();
  }

  if (!_impl->orig_pixmap.isNull()) {
    auto brush = _impl->clicks_count == 0 ? QBrush(conical_gradient()) : QBrush(random_color());
    setPixmap(apply_texture(_impl->orig_pixmap, brush));
  }

  QLabel::mouseReleaseEvent(event);
}
