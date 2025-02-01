// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "skin_engine_global.hpp"

#include "renderable.hpp"

#include <QtCore/QMargins>
#include <QtCore/QString>
#include <QtGui/QPixmap>
#include <QtWidgets/QTileRules>

class SKIN_ENGINE_EXPORT Skin
{
public:
  virtual ~Skin() = default;

  // single line only
  virtual std::unique_ptr<Renderable> draw(const QString& str) const = 0;

  // is character supported and can be drawn?
  virtual bool supports(char32_t ch) const = 0;

  virtual void setTexture(const QPixmap& b, const QTileRules& t) = 0;
  virtual void setBackground(const QPixmap& b, const QTileRules& t) = 0;

  virtual void setPerCharRendering(bool en) = 0;

  virtual void setCharSpacing(qreal s) = 0;

  virtual void setCharMargins(const QMarginsF& m) = 0;

  struct Metrics {
    qreal ascent;     // distance from baseline to top
    qreal descent;    // distance from baseline to bottom

    // line height = ascent + descent, so no field provided

    qreal line_spacing;
  };

  virtual Metrics metrics() const = 0;
};
