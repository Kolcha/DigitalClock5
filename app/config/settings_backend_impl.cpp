// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "settings_backend_impl.hpp"

#include "dc5_legacy.hpp"

SettingsBackendImpl::SettingsBackendImpl()
{
  compat::migrateConfig(_st);
}

SettingsBackendImpl::SettingsBackendImpl(const QString& filename)
  : _st(filename, QSettings::IniFormat)
{
  compat::migrateConfig(_st);
}

void SettingsBackendImpl::setValue(const QString& k, const QVariant& v)
{
  _runtime_values[k] = v;
}

QVariant SettingsBackendImpl::value(const QString& k, const QVariant& d) const
{
  auto iter = _runtime_values.find(k);
  if (iter == _runtime_values.end()) {
    auto v = _st.value(k, d);
    iter = _runtime_values.insert(k, v);
  }
  return *iter;
}

void SettingsBackendImpl::commitValue(const QString& k)
{
  if (auto iter = _runtime_values.find(k); iter != _runtime_values.end())
    _st.setValue(k, *iter);
}

void SettingsBackendImpl::discardValue(const QString& k)
{
  _runtime_values.remove(k);
}

void SettingsBackendImpl::exportSettings(QVariantHash& s) const
{
  s.insert(_runtime_values);
}

void SettingsBackendImpl::importSettings(const QVariantHash& s)
{
  auto m = compat::mapFromDC5Legacy(s);
  _runtime_values.insert(m.isEmpty() ? s : m);
}

void SettingsBackendImpl::commitAll()
{
  const auto& s = _runtime_values;
  for (auto iter = s.begin(); iter != s.end(); ++iter)
    _st.setValue(iter.key(), iter.value());
}

void SettingsBackendImpl::discardAll()
{
  _runtime_values.clear();
}
