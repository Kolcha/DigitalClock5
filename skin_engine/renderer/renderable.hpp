// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "skin_engine_global.hpp"

#include <algorithm>
#include <iterator>
#include <memory>
#include <ranges>
#include <type_traits>
#include <vector>

#include <QtCore/QRect>

class QPainter;

class SKIN_ENGINE_EXPORT Renderable
{
public:
  virtual ~Renderable() = default;

  virtual QRectF geometry() const = 0;
  virtual void draw(QPainter* p) const = 0;
};


class SKIN_ENGINE_EXPORT NoopRenderable : public Renderable
{
public:
  explicit NoopRenderable(QRectF r) noexcept
    : _r(std::move(r))
  {}

  QRectF geometry() const noexcept override { return _r; }
  void draw(QPainter* p) const noexcept override { Q_UNUSED(p); }

private:
  QRectF _r;
};


class SKIN_ENGINE_EXPORT CompositeRenderable : public Renderable
{
public:
  using ContainerType = std::vector<std::unique_ptr<Renderable>>;

  CompositeRenderable() = default;
  CompositeRenderable(ContainerType&& c);

  // MSVC tries to generate copy constructor if class exported
  // so, delete copy operations explicitly and default the move ones
  CompositeRenderable(const CompositeRenderable& ) = delete;
  CompositeRenderable(CompositeRenderable&& ) = default;

  CompositeRenderable& operator =(const CompositeRenderable& ) = delete;
  CompositeRenderable& operator =(CompositeRenderable&& ) = default;

  template<class Iter>
  requires std::is_convertible_v<
      std::iter_value_t<Iter>,
      ContainerType::value_type
  >
  CompositeRenderable(Iter first, Iter last)
    : _c(std::distance(first, last))
    , _dirty(true)
  {
    std::move(first, last, _c.begin());
  }

  template<class R>
  requires std::is_convertible_v<
      std::iter_value_t<std::ranges::iterator_t<R>>,
      ContainerType::value_type
  >
  explicit CompositeRenderable(R&& r)
    : _c(std::ranges::size(r))
    , _dirty(true)
  {
    std::ranges::move(r, _c.begin());
  }

  QRectF geometry() const override;
  void draw(QPainter* p) const override;

  void add(std::unique_ptr<Renderable> r);

  bool empty() const noexcept { return _c.empty(); }

private:
  ContainerType _c;
  mutable QRectF _rect;
  mutable bool _dirty = false;
};


class SKIN_ENGINE_EXPORT RenderableDecorator : public Renderable
{
public:
  explicit RenderableDecorator(std::unique_ptr<Renderable> inner) noexcept
    : _inner(std::move(inner))
  {}

  QRectF geometry() const override { return _inner->geometry(); }
  void draw(QPainter* p) const override { return _inner->draw(p); }

protected:
  const std::unique_ptr<Renderable>& inner() const noexcept { return _inner; }

private:
  std::unique_ptr<Renderable> _inner;
};
