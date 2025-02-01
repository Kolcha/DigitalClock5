// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "skin_engine_global.hpp"

#include <QtWidgets/QWidget>

#include "renderer/renderer.hpp"
#include "scale_factor.hpp"

class SKIN_ENGINE_EXPORT SkinnedWidgetBase : public QWidget
{
  Q_OBJECT

public:
  explicit SkinnedWidgetBase(LinesRenderer& r, QWidget* parent = nullptr);

  QSize sizeHint() const override;

  QPoint origin() const;

  ScaleFactor scaling() const noexcept { return {_sx, _sy}; }

  void setTextureCustomization(LinesRenderer::CustomizationType c);
  void setBackgroundCustomization(LinesRenderer::CustomizationType c);

  void setTexture(QPixmap p, QTileRules t = {});
  void setBackground(QPixmap p, QTileRules t = {});

  void setCharSpacing(qreal s);
  void setLineSpacing(qreal s);
  void setRespectLineSpacing(bool en);

  // not recommended for arbitrary text
  void setTruePerCharRendering(bool en);

  void setCharMargins(QMarginsF m);
  void setTextMargins(QMarginsF m);

  void setSkin(std::shared_ptr<Skin> skin);

  void setScaling(qreal sx, qreal sy);
  inline void setScaling(const ScaleFactor& f) { setScaling(f.x(), f.y()); }

  void setFrameEnabled(bool enabled);
  inline void enableFrame() { setFrameEnabled(true); }
  inline void disableFrame() { setFrameEnabled(false); }

protected:
  void paintEvent(QPaintEvent* event) override;

protected:
  virtual std::unique_ptr<Renderable> buildRenderable() = 0;

  void doUpdateGeometry();
  void doRepaint();

private:
  qreal _sx = 1.0;
  qreal _sy = 1.0;
  LinesRenderer& _rr;
  std::unique_ptr<Renderable> _r;
  QPointF _origin{0, 0};
  bool _frame_visible = false;
};


class SKIN_ENGINE_EXPORT SkinnedTextWidget : public SkinnedWidgetBase
{
  Q_OBJECT

public:
  explicit SkinnedTextWidget(QWidget* parent = nullptr);

public slots:
  void drawText(QString txt, LinesRenderer::Options opt);

protected:
  std::unique_ptr<Renderable> buildRenderable() override;

private:
  SkinnedTextWidget(std::unique_ptr<TextRenderer> r, QWidget *parent);

private:
  std::unique_ptr<TextRenderer> _rr;
  QString _txt;
  LinesRenderer::Options _opt;
};
