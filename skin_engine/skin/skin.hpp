/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <memory>

#include <QHash>
#include <QPoint>
#include <QRect>

#include "skin_engine_global.hpp"
#include "observable.hpp"

class QPainter;

class SKIN_ENGINE_EXPORT GeometryChangeListener {
public:
  virtual ~GeometryChangeListener() = default;

  virtual void onGeometryChanged() = 0;
};


class SKIN_ENGINE_EXPORT Skin {
public:
  virtual ~Skin() = default;

  // generic properties
  virtual bool hasAlternateSeparator() const = 0;
  virtual bool supportsCustomSeparator() const = 0;

  class Glyph : public Observable<GeometryChangeListener> {
  public:
    virtual ~Glyph() = default;

    virtual QRectF rect() const = 0;
    // dy should be negative as Y points to down
    virtual QPointF advance() const = 0;

    virtual void draw(QPainter* p) const = 0;
  };

  std::shared_ptr<Glyph> glyph(char32_t c) const;

  QMarginsF margins() const { return _mgs; }

  void setMargins(QMarginsF mgs);

protected:
  virtual std::shared_ptr<Glyph> create(char32_t c) const = 0;

private:
  QMarginsF _mgs;

  class DecoratedGlyph;
  mutable QHash<char32_t, std::shared_ptr<DecoratedGlyph>> _cache;
};
