/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <QSettings>

#include "clock_common_global.hpp"

// TODO: const &
class CLOCK_COMMON_EXPORT ISettingsStorage {
public:
  virtual ~ISettingsStorage() = default;

  virtual void setValue(QString key, QVariant val) = 0;
  virtual QVariant value(QString key, QVariant def) const = 0;

  virtual void commitValue(QString key) = 0;
  virtual void discardValue(QString key) = 0;
};


// TODO: move to app
class CLOCK_COMMON_EXPORT SettingsStorage : public ISettingsStorage {
public:
  SettingsStorage() = default;
  explicit SettingsStorage(const QString& filename);

  // stores given value in runtime storage
  void setValue(QString key, QVariant val);
  // tries to get the value from runtime first,
  // if not found - tries to read from permanent,
  // if still nothing is there - returns default
  QVariant value(QString key, QVariant def) const;

  // writes single value to permanent storage
  void commitValue(QString key);
  // drops single value from runtime storage
  void discardValue(QString key);

  // writes all runtime values to permanent
  void commitAll();
  // drops all values from runtime storage
  void discardAll();

  // reads everything from permanent storage
  // than updates with runtime data
  void exportSettings(QVariantHash& s) const;
  // writes only to runtime settings
  // doesn't update permanent storage
  void importSettings(const QVariantHash& s);

private:
  QSettings _storage;
  QVariantHash _runtime;
};


class CLOCK_COMMON_EXPORT SettingsStorageClient {
public:
  template<typename T>
  struct toVariant {
    QVariant operator()(const T& v) { return QVariant::fromValue(v); }
  };

  template<typename T>
  struct fromVariant {
    T operator()(const QVariant& v) { return qvariant_cast<T>(v); }
  };

  SettingsStorageClient(QString title, ISettingsStorage& st);
  virtual ~SettingsStorageClient() = default;

  QString title() const { return _title; }

  void commit();
  void discard();

protected:
  void setValue(QString key, QVariant val);
  QVariant value(QString key, QVariant def) const;

  auto& storage() noexcept { return _st; }
  auto& storage() const noexcept { return _st; }

  QString storageKey(QStringView key) const;

private:
  typedef void(ISettingsStorage::*op_type)(QString);
  void keysOperation(op_type op);

private:
  ISettingsStorage& _st;
  QSet<QString> _keys;
  QString _title;
};


#define CONFIG_OPTION(type, name, key, def_value) \
  void set##name(const type& val)                 \
    { setValue(key, toVariant<type>{}(val)); }    \
  type get##name() const                          \
    { return fromVariant<type>{}(value(key, toVariant<type>{}(def_value))); }

#define CONFIG_OPTION_Q(type, name, def_value)    \
  CONFIG_OPTION(type, name, QLatin1String(#name), def_value)


// state client doesn't assume discard operation,
// all changes are stored permanently
// state client is generic, only users know details
class CLOCK_COMMON_EXPORT StateClient {
public:
  template<typename T>
  using toVariant = SettingsStorageClient::toVariant<T>;

  template<typename T>
  using fromVariant = SettingsStorageClient::fromVariant<T>;

  StateClient(QString title, ISettingsStorage& st);

  void setValue(QString key, QVariant val);
  QVariant value(QString key, QVariant def) const;

protected:
  QString storageKey(QStringView key) const;

private:
  ISettingsStorage& _st;
  QString _title;
};
