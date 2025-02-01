// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "skin_engine_global.hpp"

#include "skin_base.hpp"

#include <QtCore/QHash>
#include <QtSvg/QSvgRenderer>

class SKIN_ENGINE_EXPORT Glyph
{
public:
  virtual ~Glyph() = default;

  virtual QRectF rect() const = 0;
  virtual QPointF advance() const = 0;

  virtual void draw(QPainter* p) const = 0;
};


class SKIN_ENGINE_EXPORT GlyphBase : public Glyph
{
public:
  QRectF rect() const noexcept override { return _rect; }
  QPointF advance() const noexcept override { return _adv; }

  // factory may override geometry
  void setRect(QRectF r) noexcept { _rect = std::move(r); }
  void setAdvance(QPointF a) noexcept { _adv = std::move(a); }

protected:
  GlyphBase(QRectF r) noexcept;

private:
  QRectF _rect;
  QPointF _adv;
};


class SKIN_ENGINE_EXPORT RasterGlyph : public GlyphBase
{
public:
  explicit RasterGlyph(const QString& filename);
  explicit RasterGlyph(QPixmap pxm) noexcept;

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


class SKIN_ENGINE_EXPORT VectorGlyph : public GlyphBase
{
public:
  explicit VectorGlyph(const QString& filename);
  explicit VectorGlyph(std::unique_ptr<QSvgRenderer> rr);

  void draw(QPainter* p) const override;

private:
  std::unique_ptr<QSvgRenderer> _renderer;
};


class SKIN_ENGINE_EXPORT ImageSkin : public SkinBase
{
public:
  void insertGlyph(char32_t c, std::shared_ptr<Glyph> g);
  std::shared_ptr<Glyph> glyph(char32_t c) const noexcept;

  std::unique_ptr<Renderable> draw(const QString &str) const override;

  bool supports(char32_t ch) const override { return _glyps.contains(ch); }

  Metrics metrics() const override;

private:
  std::unique_ptr<Renderable> maybeCustomize(std::unique_ptr<Renderable> r,
                                             bool enable) const;
  std::unique_ptr<Renderable> customize(std::unique_ptr<Renderable> r) const;

private:
  QHash<char32_t, std::shared_ptr<Glyph>> _glyps;
};
