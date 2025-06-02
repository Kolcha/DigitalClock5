// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <type_traits>

#include <QtCore/QMargins>
#include <QtCore/QString>
#include <QtCore/QTimeZone>
#include <QtCore/QVariant>
#include <QtGui/QBrush>
#include <QtGui/QFont>
#include <QtGui/QFontInfo>

#include "settings.hpp"

// QBush seems not deserializes coordinate mode, and it is set
// to default (QGradient::LogicalMode) after deserialization
// this is not suitable, so re-create brush if required
template<>
struct ConfigBase::fromVariant<QBrush> {
  QBrush operator ()(const QVariant& v)
  {
    auto brush = qvariant_cast<QBrush>(v);
    if (brush.gradient()) {
      QGradient gradient = *brush.gradient();
      gradient.setCoordinateMode(QGradient::ObjectMode);
      brush = QBrush(gradient);
    }
    return brush;
  }
};

// save QGradient as QBrush, Qt seems to have serializer for that
template<>
struct ConfigBase::toVariant<QGradient> {
  QVariant operator ()(const QGradient& v)
  {
    return toVariant<QBrush>()(QBrush(v));
  }
};

template<>
struct ConfigBase::fromVariant<QGradient> {
  QGradient operator ()(const QVariant& v)
  {
    return *fromVariant<QBrush>()(v).gradient();
  }
};

// serialize QTimeZone as QString (IANA ID)
template<>
struct ConfigBase::toVariant<QTimeZone> {
  QVariant operator ()(const QTimeZone& v)
  {
    return toVariant<QString>()(QString::fromLatin1(v.id()));
  }
};

template<>
struct ConfigBase::fromVariant<QTimeZone> {
  QTimeZone operator ()(const QVariant& v)
  {
    return QTimeZone(fromVariant<QString>()(v).toLatin1());
  }
};

// serialize QMargins as string

namespace detail {

template<typename T>
concept margins = std::is_same_v<T, QMargins> || std::is_same_v<T, QMarginsF>;

template<typename T>
struct get_margins_data_type;

template<>
struct get_margins_data_type<QMargins> {
  using value = int;
};

template<>
struct get_margins_data_type<QMarginsF> {
  using value = qreal;
};

} // detail

template<detail::margins T>
struct ConfigBase::toVariant<T> {
  QVariant operator()(const T& v)
  {
    auto ms = QString("%1,%2,%3,%4")
                  .arg(v.left()).arg(v.top())
                  .arg(v.right()).arg(v.bottom());
    return toVariant<QString>()(ms);
  }
};

template<detail::margins T>
struct ConfigBase::fromVariant<T> {
  T operator()(const QVariant& v)
  {
    using val_type = detail::get_margins_data_type<T>::value;
    auto ms = fromVariant<QString>()(v).split(',');
    val_type l = ms.size() > 0 ? ms[0].toFloat() : val_type(0);
    val_type t = ms.size() > 1 ? ms[1].toFloat() : val_type(0);
    val_type r = ms.size() > 2 ? ms[2].toFloat() : val_type(0);
    val_type b = ms.size() > 3 ? ms[3].toFloat() : val_type(0);
    return ms.empty() ? T() : T(l, t, r, b);
  }
};

// serialize QFlags<> as int
template<typename T>
struct ConfigBase::toVariant<QFlags<T>> {
  QVariant operator ()(const QFlags<T>& v)
  {
    return toVariant<int>()(static_cast<int>(v));
  }
};

template<typename T>
struct ConfigBase::fromVariant<QFlags<T>> {
  QFlags<T> operator ()(const QVariant& v)
  {
    return static_cast<QFlags<T>>(fromVariant<int>()(v));
  }
};

// serialize any enum values as int
template<typename T>
  requires std::is_enum_v<T>
struct ConfigBase::toVariant<T> {
  QVariant operator ()(const T& v)
  {
    return toVariant<int>()(static_cast<int>(v));
  }
};

template<typename T>
  requires std::is_enum_v<T>
struct ConfigBase::fromVariant<T> {
  T operator ()(const QVariant& v)
  {
    return static_cast<T>(fromVariant<int>()(v));
  }
};

// resolve actually used font
template<>
struct ConfigBase::fromVariant<QFont> {
  QFont operator ()(const QVariant& v)
  {
    auto res_font = qvariant_cast<QFont>(v);
    QFontInfo fi(res_font);
    res_font.setFamily(fi.family());
    res_font.setPointSize(fi.pointSize());
    res_font.setStyle(fi.style());
    res_font.setStyleHint(fi.styleHint());
    return res_font;
  }
};
