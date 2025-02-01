// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "plugin_core_global.hpp"

#include "config/settings.hpp"

class PLUGIN_CORE_EXPORT PluginSettingsBackend : public SettingsBackend
{
public:
  explicit PluginSettingsBackend(std::unique_ptr<SettingsStorage> st)
    : _storage(std::move(st))
  {}

  void setValue(const QString& k, const QVariant& v) override
  {
    _storage->setValue(k, v);
  }

  QVariant value(const QString& k, const QVariant& d) const override
  {
    return _storage->value(k, d);
  }

  void commitValue(const QString& k) override { Q_UNUSED(k); }
  void discardValue(const QString& k) override { Q_UNUSED(k); }

  // users of this class should call commit/discard on underlying storage
  // raw pointer is provided for convenient usage with Qt API
  // the returned pointer should not be deleted by the user
  SettingsStorage* storage() const noexcept { return _storage.get(); }

private:
  std::unique_ptr<SettingsStorage> _storage;
};


class PLUGIN_CORE_EXPORT PluginSettingsStorage : public SettingsStorage
{
  Q_OBJECT

public:
  explicit PluginSettingsStorage(PluginSettingsBackend& st, QObject* parent = nullptr)
    : SettingsStorage(parent)
    , _st(st)
  {}

  void setValue(const QString& k, const QVariant& v) override { _st.setValue(k, v); }
  QVariant value(const QString& k, const QVariant& d) const override { return _st.value(k, d); }

public slots:
  void commit() override { _st.storage()->commit(); }
  void discard() override { _st.storage()->discard(); }

private:
  PluginSettingsBackend& _st;
};


// function that returns instance section name
// should be used whenever instance settings required
PLUGIN_CORE_EXPORT QString instance_prefix(size_t idx);
