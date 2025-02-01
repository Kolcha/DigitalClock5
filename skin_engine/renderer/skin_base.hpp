// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "skin_engine_global.hpp"

#include "skin.hpp"

class SKIN_ENGINE_EXPORT SkinBase : public Skin
{
public:
  void setTexture(const QPixmap& b, const QTileRules& t) override;
  void setBackground(const QPixmap& b, const QTileRules& t) override;

  void setPerCharRendering(bool en) noexcept override { _per_char_rendering = en; }

  void setCharSpacing(qreal s) noexcept override { _spacing = s; }

  void setCharMargins(const QMarginsF& m) noexcept override { _margins = m; }

  const QPixmap& texture() const noexcept { return _texture; }
  const QTileRules& textureRules() const noexcept { return _tx_rules; }

  const QPixmap& background() const noexcept { return _background; }
  const QTileRules& backgroundRules() const noexcept { return _bg_rules; }

  bool perCharRendering() const noexcept { return _per_char_rendering; }

  qreal charSpacing() const noexcept { return _spacing; }

  const QMarginsF& charMargins() const noexcept { return _margins; }

private:
  QPixmap _texture;
  QPixmap _background;
  QTileRules _tx_rules;
  QTileRules _bg_rules;
  bool _per_char_rendering = false;
  qreal _spacing = 0;
  QMarginsF _margins;
};


class SKIN_ENGINE_EXPORT SkinRenderable final : public RenderableDecorator
{
public:
  using RenderableDecorator::RenderableDecorator;

  void draw(QPainter* p) const override;

  void setTexture(QPixmap b, QTileRules t) noexcept;
  void setBackground(QPixmap b, QTileRules t) noexcept;
/*
  const QPixmap& texture() const noexcept { return _texture; }
  const QTileRules& textureRules() const noexcept { return _tx_rules; }

  const QPixmap& background() const noexcept { return _background; }
  const QTileRules& backgroundRules() const noexcept { return _bg_rules; }
*/

private:
  QPixmap _texture;
  QPixmap _background;
  QTileRules _tx_rules;
  QTileRules _bg_rules;
};
