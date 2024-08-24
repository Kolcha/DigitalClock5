/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <memory>

#include <QBrush>
#include <QTransform>

#include <QPixmapCache>

#include "observable.hpp"
#include "skin.hpp"

#include "skin_engine_global.hpp"

class SKIN_ENGINE_EXPORT AppearanceChangeListener {
public:
  virtual ~AppearanceChangeListener() = default;

  virtual void onAppearanceChanged() = 0;
};


// many glyphs may have the same appearance
class SKIN_ENGINE_EXPORT Appearance : public Observable<AppearanceChangeListener> {
public:
  QBrush background() const { return _bg; }
  QBrush texture() const { return _tx; }
  bool backgroundStretch() const { return _bg_s; }
  bool textureStretch() const { return _tx_s; }

  void setBackground(QBrush b, bool stretch);
  void setBackground(QBrush b) { setBackground(b, backgroundStretch()); }
  void setBackgroundStretch(bool en) { setBackground(background(), en); }

  void setTexture(QBrush b, bool stretch);
  void setTexture(QBrush b) { setTexture(b, textureStretch()); }
  void setTextureStretch(bool en) { setTexture(texture(), en); }

private:
  QBrush _bg;
  QBrush _tx;
  bool _bg_s = false;
  bool _tx_s = false;
};


// many glyphs may have the same transform
class SKIN_ENGINE_EXPORT Transform : public Observable<AppearanceChangeListener> {
public:
  QTransform transform() const { return _t; }

  void setTransform(QTransform t);

private:
  QTransform _t;
};


// non-shareable, as has unique data such as position
class SKIN_ENGINE_EXPORT Glyph : public AppearanceChangeListener {
public:
  Glyph() = default;
  explicit Glyph(std::shared_ptr<Skin::Glyph> g);

  Glyph(const Glyph&) = default;
  Glyph(Glyph&&) = default;

  Glyph& operator=(const Glyph&) = default;
  Glyph& operator=(Glyph&&) = default;

  ~Glyph();

  std::shared_ptr<Skin::Glyph> glyph() const { return _g; }
  void setGlyph(std::shared_ptr<Skin::Glyph> g);

  // ---------------------

  // geometry
  QRectF rect() const { return _br; }
  QPointF pos() const { return _pos; }
  QRectF geometry() const { return _gr; }
  QPointF advance() const { return _adv; }

  std::shared_ptr<Transform> transform() const { return _t; }

  bool resizeEnabled() const { return _resize_enabled; }

  // appearance
  std::shared_ptr<Appearance> appearance() const { return _a; }

  bool visible() const { return _vis; }

  // ---------------------

  // geometry
  void setPos(QPointF p);

  void setTransform(std::shared_ptr<Transform> t);

  void setResizeEnabled(bool enabled);
  inline void enableResize() { setResizeEnabled(true); }
  inline void disableResize() { setResizeEnabled(false); }

  // resize item in given direction
  // aspect ratio is preserved
  void resize(qreal l, Qt::Orientation o);

  // appearance
  void setAppearance(std::shared_ptr<Appearance> a);

  void setVisible(bool visible) { _vis = visible; }
  void show() { setVisible(true); }
  void hide() { setVisible(false); }
  void toggleVisibility() { setVisible(!visible()); }

  // ---------------------

  void draw(QPainter* p) const;

  // listeners
  void onAppearanceChanged();

protected:
  void updateGeometry();
  void dropCachedData();

  void setCachingEnabled(bool en) { _caching = en; }
  inline void enableCaching() { setCachingEnabled(true); }
  inline void disableCaching() { setCachingEnabled(false); }

private:
  std::shared_ptr<Skin::Glyph> _g;
  std::shared_ptr<Transform> _t;
  std::shared_ptr<Appearance> _a;

  QRectF _br;
  QRectF _gr;
  QPointF _pos;
  QPointF _adv;

  bool _vis = true;
  bool _caching = true;

  // is resizing enabled?
  bool _resize_enabled = false;
  // scaling coefficient to achive "resize effect"
  qreal _ks = 1.0;

  // draw() is const (and expected to be const),
  // and caching should be transparent for users
  mutable QPixmapCache::Key _ck;
  mutable size_t _ht = 0;
};
