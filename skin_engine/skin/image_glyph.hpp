/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "skin.hpp"

#include <QPixmap>
#include <QSvgRenderer>

#include "skin_engine_global.hpp"

class SKIN_ENGINE_EXPORT ImageGlyphBase : public Skin::Glyph {
public:
  QRectF rect() const override { return _rect; }
  QPointF advance() const override { return _adv; }

  // factory may override geometry
  void setRect(QRectF r) { _rect = std::move(r); }
  void setAdvance(QPointF a) { _adv = std::move(a); }

protected:
  ImageGlyphBase(QRectF r)
      : _rect(std::move(r))
      , _adv(_rect.width(), -_rect.height())
  {}

private:
  QRectF _rect;
  QPointF _adv;
};


class SKIN_ENGINE_EXPORT RasterGlyph : public ImageGlyphBase {
public:
  explicit RasterGlyph(const QString& filename)
      : RasterGlyph(QPixmap(filename))
  {}

  explicit RasterGlyph(QPixmap&& pxm)
      : ImageGlyphBase(pxm.rect())
      , _pixmap(std::move(pxm))
  {}

  // raster images don't look good on HighDPI screens
  // unless they have suitable device/pixel ratio
  // don't make any assumption on it,
  // let the caller to specify it explicitly
  // should be called before custom geometry definition
  void setDevicePixelRatio(qreal dpr);

  void draw(QPainter* p) const override;

private:
  QPixmap _pixmap;
};


class SKIN_ENGINE_EXPORT VectorGlyph : public ImageGlyphBase {
public:
  explicit VectorGlyph(const QString& filename)
      : VectorGlyph(std::make_unique<QSvgRenderer>(filename))
  {}

  explicit VectorGlyph(std::unique_ptr<QSvgRenderer> rr)
      : ImageGlyphBase({{0, 0}, rr->defaultSize()})
      , _renderer(std::move(rr))
  {}

  void draw(QPainter* p) const override;

private:
  std::unique_ptr<QSvgRenderer> _renderer;
};
