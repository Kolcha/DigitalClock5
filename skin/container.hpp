#pragma once

#include "glyph.hpp"

class Algorithm {
public:
  virtual ~Algorithm() = default;

  using GlyphList = QVector<std::shared_ptr<Glyph>>;

  virtual void process(const GlyphList& glyphs) = 0;

  virtual QRectF rect() const = 0;
  virtual QPointF advance() const = 0;
};


class ContainerImplBase : public Skin::Glyph {
public:
  explicit ContainerImplBase(std::shared_ptr<Algorithm> algo);

  QRectF rect() const override { return _rect; }
  QPointF advance() const override { return _adv; }

  void draw(QPainter* p) const override;

  void addGlyph(std::shared_ptr<::Glyph> g);
  void clear() { _glyphs.clear(); }

  auto& glyphs() { return _glyphs; }
  const auto& glyphs() const { return _glyphs; }

  auto algorithm() const { return _algo; }

  void updateGeometry();

private:
  QVector<std::shared_ptr<::Glyph>> _glyphs;
  std::shared_ptr<Algorithm> _algo;
  // cache geometry data
  QRectF _rect;
  QPointF _adv;
};


class LinearLayout : public Algorithm {
public:
  void process(const GlyphList& glyps) override;

  QRectF rect() const override { return _rect; }
  QPointF advance() const override { return _adv; }

  qreal spacing() const { return _spacing; }

  Qt::Alignment alignment() const { return _align; }

  void setSpacing(qreal spacing) { _spacing = spacing; }

  void setAlignment(Qt::Alignment align) { _align = align; }

protected:
  virtual void updatePos(Glyph& curr, const Glyph& prev) const = 0;
  virtual void updateAdv(QPointF& adv, const Glyph& curr) const = 0;
  virtual void applyAlignment(const GlyphList& glyps) const = 0;

private:
  QRectF _rect;
  QPointF _adv;
  qreal _spacing = 0;
  Qt::Alignment _align = Qt::AlignTop | Qt::AlignLeft;
};


class HLayout : public LinearLayout {
protected:
  void updatePos(Glyph& curr, const Glyph& prev) const override;
  void updateAdv(QPointF& adv, const Glyph& curr) const override;
  void applyAlignment(const GlyphList& glyps) const override;
};


class VLayout : public LinearLayout {
protected:
  void updatePos(Glyph& curr, const Glyph& prev) const override;
  void updateAdv(QPointF& adv, const Glyph& curr) const override;
  void applyAlignment(const GlyphList& glyps) const override;
};


template<class LayoutImpl>
class ContainerGlyphPrivate : public ContainerImplBase {
public:
  using AlgorithmType = LayoutImpl;
  ContainerGlyphPrivate() : ContainerImplBase(std::make_shared<LayoutImpl>()) {}
};


template<class ContainerImpl>
class ContainerGlyph : public Glyph {
public:
  using AlgorithmType = typename ContainerImpl::AlgorithmType;

  explicit ContainerGlyph()
      : ContainerGlyph(std::make_shared<ContainerImpl>())
  {
    disableCaching();
  }

  // doesn't call updateGeometry()
  // it must be called at the end
  void addGlyph(std::shared_ptr<Glyph> g)
  {
    _impl->addGlyph(std::move(g));
  }

  void clear()
  {
    _impl->clear();
    updateGeometry();
  }

  auto algorithm() const
  {
    return std::static_pointer_cast<AlgorithmType>(_impl->algorithm());
  }

  // no implicit geometry updates propagation
  // to allow multiple glyphs to be updated at once
  // without unnecessary intermediate updates for each of them
  void updateGeometry()
  {
    _impl->updateGeometry();
    Glyph::updateGeometry();
  }

  qsizetype size() const { return _impl->glyphs().size(); }

  auto& operator[](qsizetype i) { return _impl->glyphs()[i]; }
  auto& operator[](qsizetype i) const { return _impl->glyphs()[i]; }

  // maybe it worth to implement begin()/end() too, but not now

private:
  ContainerGlyph(std::shared_ptr<ContainerImpl> impl)
      : Glyph(impl)
      , _impl(impl)
  {
    Glyph::updateGeometry();
  }

private:
  std::shared_ptr<ContainerImpl> _impl;
};

using HContainerGlyphPrivate = ContainerGlyphPrivate<HLayout>;
using VContainerGlyphPrivate = ContainerGlyphPrivate<VLayout>;

using HContainerGlyph = ContainerGlyph<HContainerGlyphPrivate>;
using VContainerGlyph = ContainerGlyph<VContainerGlyphPrivate>;