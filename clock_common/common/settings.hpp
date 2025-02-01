// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "clock_common_global.hpp"

#include <QtCore/QObject>

#include <QtCore/QString>
#include <QtCore/QVariant>

class CLOCK_COMMON_EXPORT SettingsBackend
{
public:
  virtual ~SettingsBackend() = default;

  virtual void setValue(const QString& k, const QVariant& v) = 0;
  virtual QVariant value(const QString& k, const QVariant& d) const = 0;

  virtual void commitValue(const QString& k) = 0;
  virtual void discardValue(const QString& k) = 0;
};


class CLOCK_COMMON_EXPORT SettingsStorage : public QObject
{
  Q_OBJECT

public:
  explicit SettingsStorage(QObject* parent = nullptr) : QObject(parent) {}

  virtual void setValue(const QString& k, const QVariant& v) = 0;
  virtual QVariant value(const QString& k, const QVariant& d) const = 0;

public slots:
  virtual void commit() = 0;
  virtual void discard() = 0;
};


class CLOCK_COMMON_EXPORT SettingsStorageImpl : public SettingsStorage
{
  Q_OBJECT

public:
  explicit SettingsStorageImpl(SettingsBackend& st, QObject* parent = nullptr);

  void setValue(const QString& k, const QVariant& v) override;
  QVariant value(const QString& k, const QVariant& d) const override;

public slots:
  void commit() override;
  void discard() override;

private:
  SettingsBackend& _st;
  QSet<QString> _mod_keys;
};


class CLOCK_COMMON_EXPORT PrefixedSettingsStorage final : public SettingsStorage
{
  Q_OBJECT

public:
  PrefixedSettingsStorage(std::unique_ptr<SettingsStorage> st, QString prefix,
                          QObject* parent = nullptr);

  void setValue(const QString& k, const QVariant& v) override;
  QVariant value(const QString& k, const QVariant& d) const override;

public slots:
  inline void commit() override { _storage->commit(); }
  inline void discard() override { _storage->discard(); }

private:
  QString prefixed(const QString& k) const;

private:
  std::unique_ptr<SettingsStorage> _storage;
  QString _prefix;
};


// RAII-style thin wrapper around SettingsStorage
// that commits the changes on its destruction
class CLOCK_COMMON_EXPORT StateStorage final
{
public:
  inline explicit StateStorage(SettingsStorage& st) noexcept : _st(st) {}
  inline ~StateStorage() { _st.commit(); }

  inline void setValue(const QString& k, const QVariant& v) { _st.setValue(k, v); }
  inline QVariant value(const QString& k, const QVariant& d) const { return _st.value(k, d); }

private:
  SettingsStorage& _st;
};
