#pragma once

#include <QBrush>
#include <QString>
#include <QTimeZone>
#include <QVariant>

#include "settings_storage.hpp"

// QBush seems not deserializes coordinate mode, and it is set
// to default (QGradient::LogicalMode) after deserialization
// this is not suitable, so re-create brush if required
template<>
struct SettingsStorageClient::fromVariant<QBrush> {
  QBrush operator ()(const QVariant& v)
  {
    QBrush brush = qvariant_cast<QBrush>(v);
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
struct SettingsStorageClient::toVariant<QGradient> {
  QVariant operator ()(const QGradient& v)
  {
    return toVariant<QBrush>()(QBrush(v));
  }
};

template<>
struct SettingsStorageClient::fromVariant<QGradient> {
  QGradient operator ()(const QVariant& v)
  {
    return *fromVariant<QBrush>()(v).gradient();
  }
};

// serialize QTimeZone as QString (IANA ID)
template<>
struct SettingsStorageClient::toVariant<QTimeZone> {
  QVariant operator ()(const QTimeZone& v)
  {
    return toVariant<QString>()(QString::fromLatin1(v.id()));
  }
};

template<>
struct SettingsStorageClient::fromVariant<QTimeZone> {
  QTimeZone operator ()(const QVariant& v)
  {
    return QTimeZone(fromVariant<QString>()(v).toLatin1());
  }
};

// serialize QFlags<> as int
template<typename T>
struct SettingsStorageClient::toVariant<QFlags<T>> {
  QVariant operator ()(const QFlags<T>& v)
  {
    return toVariant<int>()(static_cast<int>(v));
  }
};

template<typename T>
struct SettingsStorageClient::fromVariant<QFlags<T>> {
  QFlags<T> operator ()(const QVariant& v)
  {
    return static_cast<QFlags<T>>(fromVariant<int>()(v));
  }
};
