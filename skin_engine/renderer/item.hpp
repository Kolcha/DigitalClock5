// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "skin_engine_global.hpp"

#include "renderable.hpp"

#include <QtCore/QMargins>
#include <QtCore/QPoint>
#include <QtGui/QTransform>

// decorator
class SKIN_ENGINE_EXPORT Item : public Renderable
{
public:
  explicit Item(std::unique_ptr<Renderable> r, QPointF p = {0., 0.}) noexcept;

  QRectF geometry() const override { return rect().translated(_pos); }
  void draw(QPainter *p) const override;

  QRectF rect() const;

  inline QPointF pos() const noexcept { return _pos; }
  inline QMarginsF margins() const noexcept { return _margins; }

  const QTransform& transform() const noexcept { return _transform; }

  inline void setPos(QPointF p) noexcept { _pos = std::move(p); }
  inline void setMargins(QMarginsF m) noexcept { _margins = std::move(m); }

  void setTransform(QTransform t) noexcept { _transform = std::move(t); }

  void setVisible(bool visible) noexcept { _visible = visible; }
  inline void show() noexcept { setVisible(true); }
  inline void hide() noexcept { setVisible(false); }

private:
  std::unique_ptr<Renderable> _r;
  QPointF _pos;
  QMarginsF _margins;
  QTransform _transform;
  bool _visible = true;
};
