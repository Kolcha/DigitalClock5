// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtCore/QtNumeric>
#include <QtCore/QtTypes>

class ScaleFactor final
{
public:
  constexpr inline ScaleFactor() noexcept
    : ScaleFactor(1.0, 1.0)
  {}

  constexpr inline ScaleFactor(qreal k) noexcept
    : ScaleFactor(k, k)
  {}

  constexpr inline ScaleFactor(qreal sx, qreal sy) noexcept
    : _sx(sx)
    , _sy(sy)
  {}

  constexpr inline qreal x() const noexcept { return _sx; }
  constexpr inline qreal y() const noexcept { return _sy; }

  constexpr inline void setX(qreal x) noexcept { _sx = x; }
  constexpr inline void setY(qreal y) noexcept { _sy = y; }

  constexpr inline ScaleFactor& operator*=(qreal f) noexcept
  {
    _sx *= f;
    _sy *= f;
    return *this;
  }

private:
  qreal _sx = 1.0;
  qreal _sy = 1.0;
};

Q_DECLARE_TYPEINFO(ScaleFactor, Q_PRIMITIVE_TYPE);

constexpr inline ScaleFactor operator*(const ScaleFactor& s, qreal f) noexcept
{
  return {s.x() * f, s.y() * f};
}

constexpr inline ScaleFactor operator*(qreal f, const ScaleFactor& s) noexcept
{
  return s * f;
}

constexpr bool operator==(const ScaleFactor& a, const ScaleFactor& b) noexcept
{
  return qFuzzyCompare(a.x(), b.x()) && qFuzzyCompare(b.x(), b.y());
}

constexpr bool operator!=(const ScaleFactor& a, const ScaleFactor& b) noexcept
{
  return !(a == b);
}
