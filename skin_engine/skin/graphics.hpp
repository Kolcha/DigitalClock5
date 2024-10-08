/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDateTime>
#include <QTimeZone>

#include "container.hpp"
#include "skin.hpp"

#include "skin_engine_global.hpp"

class SKIN_ENGINE_EXPORT GraphicsBase {
public:
  GraphicsBase();

  std::shared_ptr<Skin> skin() const { return _skin; }

  qreal charSpacing() const { return _char_spacing; }
  qreal lineSpacing() const { return _line_spacing; }

  QMarginsF margins() const { return _tg.margins(); }

  bool ignoreAX() const { return _ignore_ax; }
  bool ignoreAY() const { return _ignore_ay; }

  Qt::Alignment alignment() const { return _tg.algorithm()->alignment(); }
  Qt::Alignment alignment(size_t i) const
  { return _tg.algorithm()->alignment(i); }

  // ---------------------

  QRectF rect() const { return _tg.rect(); }
  QPointF advance() const { return _tg.advance(); }

  // ---------------------

  QBrush background() const { return _bg; }
  QBrush texture() const { return _tx; }

  bool backgroundStretch() const { return _bg_s; }
  bool textureStretch() const { return _tx_s; }

  bool backgroundPerChar() const { return _bg_pc; }
  bool texturePerChar() const { return _tx_pc; }

  // ---------------------

  void setSkin(std::shared_ptr<Skin> skin);

  void setCharSpacing(qreal spacing);
  void setLineSpacing(qreal spacing);

  void setMargins(QMarginsF margins);

  void setIgnoreAX(bool ignore);
  void setIgnoreAY(bool ignore);

  void setAlignment(Qt::Alignment a);
  void setAlignment(size_t i, Qt::Alignment a);

  // unique function to set per-line alignment at once
  void applyLayoutConfig(QStringView cfg);

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

  void draw(QPainter* p) const { _tg.draw(p); }

  void rebuildLayout();

protected:
  auto charAppearance() const { return _char_a; }
  auto textAppearance() const { return _text_a; }

  auto& vcontainer() { return _tg; }

  void rebuild();
  void update();

  virtual void clearLayout() = 0;
  virtual void buildLayout() = 0;

private:
  VContainerGlyph _tg;
  std::shared_ptr<Skin> _skin;

  qreal _char_spacing = 0;
  qreal _line_spacing = 0;

  std::shared_ptr<Appearance> _char_a;
  std::shared_ptr<Appearance> _text_a;

  QBrush _bg;
  QBrush _tx;
  bool _bg_s = false;
  bool _tx_s = false;
  bool _bg_pc = false;
  bool _tx_pc = true;

  bool _ignore_ax = false;
  bool _ignore_ay = false;
};


class SKIN_ENGINE_EXPORT GraphicsText : public GraphicsBase {
public:
  explicit GraphicsText(QString text = {});

  QString text() const { return _text; }

  void setText(QString text);

protected:
  void clearLayout() override;
  void buildLayout() override;

private:
  QString _text;
  QVector<QVector<std::shared_ptr<Glyph>>> _lines;
};


class SKIN_ENGINE_EXPORT GraphicsDateTime : public GraphicsBase {
public:
  explicit GraphicsDateTime(QDateTime dt = QDateTime::currentDateTime());

  QDateTime currentDateTime() const { return _dt; }
  QTimeZone currentTimeZone() const { return _dt.timeZone(); }
  QString format() const { return _fmt; }

  bool flashSeparators() const { return _flash_seps; }
  bool useAlternateSeparator() const { return _use_alt_sep; }
  bool useCustomSeparators() const { return _use_cust_seps; }

  QString customSeparators() { return _custom_seps; }

  QTransform tokenTransform(QString token) const;

  void setDateTime(QDateTime dt);
  void setTimeZone(QTimeZone tz);
  void setFormat(QString fmt);

  void setFlashSeparator(bool en);
  void setUseAlternateSeparator(bool en);
  void setUseCustomSeparators(bool en);

  void setCustomSeparators(QString seps);

  void updateSeparatorsState();

  void setTokenTransform(QString token, QTransform t);
  void removeTokenTransform(QString token);
  void clearTokenTransform();

protected:
  void clearLayout() override;
  void buildLayout() override;

private:
  void rebuildLayout(const auto& dt_tokens);
  void updateLayout(const auto& dt_tokens);

private:
  QDateTime _dt;
  QString _fmt;

  QStringList _last_tokens;

  using LineToken = QVector<std::shared_ptr<Glyph>>;
  using Line = QVector<LineToken>;
  QVector<Line> _lines;

  QVector<std::shared_ptr<Glyph>> _seps;
  QString _custom_seps;

  bool _flash_seps = true;
  bool _use_alt_sep = false;
  bool _use_cust_seps = false;
  bool _sep_visible = true;

  QHash<QString, QTransform> _tt;
};
