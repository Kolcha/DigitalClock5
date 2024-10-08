/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "graphics.hpp"

// potentially useless, as doesn't respect spacing for the first and last lines
// but from other side, it produces very tight rect with no extra spacing
// can be used as widget for displaying data from plugins
// TODO: add option to use full string height, add ascent/descent values to skin
class SKIN_ENGINE_EXPORT GraphicsWidgetBase : public QWidget
{
  Q_OBJECT

public:
  enum AutoResizePolicy { None, KeepWidth, KeepHeight };
  Q_ENUM(AutoResizePolicy)

  GraphicsWidgetBase(std::shared_ptr<GraphicsBase> gr, QWidget* parent = nullptr);

  QSize sizeHint() const override;

  bool hasHeightForWidth() const override;
  int heightForWidth(int w) const override;

  AutoResizePolicy autoResizePolicy() const { return _rp; }

  std::shared_ptr<Skin> skin() const { return _gt->skin(); }

  qreal charSpacing() const { return _gt->charSpacing(); }
  qreal lineSpacing() const { return _gt->lineSpacing(); }

  QMarginsF margins() const { return _gt->margins(); }

  bool ignoreAX() const { return _gt->ignoreAX(); }
  bool ignoreAY() const { return _gt->ignoreAY(); }

  qreal scalingX() const { return _sx; }
  qreal scalingY() const { return _sy; }

  Qt::Alignment alignment() const { return _gt->alignment(); }
  QString layoutConfig() const { return _lcfg; }

  // ---------------------

  QPoint origin() const;  // in widget coordinates
  QRectF graphicsRect() const { return _gt->rect(); }
  QPointF advance() const;

  // ---------------------

  QBrush background() const { return _gt->background(); }
  QBrush texture() const { return _gt->texture(); }

  bool backgroundStretch() const { return _gt->backgroundStretch(); }
  bool textureStretch() const { return _gt->textureStretch(); }

  bool backgroundPerChar() const { return _gt->backgroundPerChar(); }
  bool texturePerChar() const { return _gt->texturePerChar(); }

  bool useSystemBackground() const { return _sys_bg; }
  bool useSystemForeground() const { return _sys_fg; }

  // ---------------------

  void setSkin(std::shared_ptr<Skin> skin);

signals:
  void skinChanged(std::shared_ptr<Skin> skin);

public slots:
  void rebuildLayout();

  void setAutoResizePolicy(AutoResizePolicy rp);

  void setCharSpacing(qreal spacing);
  void setLineSpacing(qreal spacing);

  void setMargins(QMarginsF margins);

  void setIgnoreAX(bool ignore);
  void setIgnoreAY(bool ignore);

  void setScalingX(qreal sx) { setScaling(sx, scalingY()); }
  void setScalingY(qreal sy) { setScaling(scalingX(), sy); }
  void setScaling(qreal sx, qreal sy);

  void setAlignment(Qt::Alignment a);
  void setLayoutConfig(QString lcfg);

  // ---------------------

  void setBackground(QBrush b, bool stretch, bool per_char);

  inline void setBackground(QBrush b)
  {
    setBackground(b, backgroundStretch(), backgroundPerChar());
  }

  inline void setBackgroundStretch(bool en)
  {
    setBackground(background(), en, backgroundPerChar());
  }

  inline void setBackgroundPerChar(bool en)
  {
    setBackground(background(), backgroundStretch(), en);
  }

  // ---------------------

  void setTexture(QBrush b, bool stretch, bool per_char);

  inline void setTexture(QBrush b)
  {
    setTexture(b, textureStretch(), texturePerChar());
  }

  inline void setTextureStretch(bool en)
  {
    setTexture(texture(), en, texturePerChar());
  }

  inline void setTexturePerChar(bool en)
  {
    setTexture(texture(), textureStretch(), en);
  }

  // ---------------------

  void setUseSystemBackground(bool use);
  void setUseSystemForeground(bool use);

protected:
  void paintEvent(QPaintEvent* event) override;
  bool event(QEvent* e) override;

private:
  std::shared_ptr<GraphicsBase> _gt;
  AutoResizePolicy _rp = None;
  qreal _k = 1.0;

  qreal _sx = 1.0;
  qreal _sy = 1.0;

  QString _lcfg;

  QBrush _last_bg;
  QBrush _last_tx;
  bool _sys_bg = false;
  bool _sys_fg = false;
};


class SKIN_ENGINE_EXPORT GraphicsTextWidget : public GraphicsWidgetBase
{
  Q_OBJECT

public:
  explicit GraphicsTextWidget(QWidget* parent = nullptr);

  QString text() const { return _gt->text(); }

public slots:
  void setText(QString text);

private:
  GraphicsTextWidget(std::shared_ptr<GraphicsText> gt, QWidget* parent);

private:
  std::shared_ptr<GraphicsText> _gt;
};


class SKIN_ENGINE_EXPORT GraphicsDateTimeWidget : public GraphicsWidgetBase
{
  Q_OBJECT

public:
  explicit GraphicsDateTimeWidget(QWidget* parent = nullptr);

  QDateTime currentDateTime() const { return _gt->currentDateTime(); }
  QTimeZone currentTimeZone() const { return _gt->currentTimeZone(); }
  QString format() const { return _gt->format(); }

  bool flashSeparators() const { return _gt->flashSeparators(); }
  bool useAlternateSeparator() const { return _gt->useAlternateSeparator(); }
  bool useCustomSeparators() const { return _gt->useCustomSeparators(); }

  QString customSeparators() { return _gt->customSeparators(); }

  QTransform tokenTransform(QString token) const { return _gt->tokenTransform(token); }

signals:
  // time in configured time zone (local time by default)
  void dateTimeChanged(const QDateTime& dt);

public slots:
  void setDateTime(QDateTime dt);
  void setTimeZone(QTimeZone tz);
  void setFormat(QString fmt);

  void setFlashSeparator(bool en);
  void setUseAlternateSeparator(bool en);
  void setUseCustomSeparators(bool en);

  void setCustomSeparators(QString seps);

  void updateSeparatorsState();

  void setSecondsScaleFactor(qreal sf);
  void setTokenTransform(QString token, QTransform t);
  void removeTokenTransform(QString token);
  void clearTokenTransform();

private:
  GraphicsDateTimeWidget(std::shared_ptr<GraphicsDateTime> gt, QWidget* parent);

private:
  std::shared_ptr<GraphicsDateTime> _gt;
};
