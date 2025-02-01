// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "settings.hpp"

SettingsStorageImpl::SettingsStorageImpl(SettingsBackend& st, QObject* parent)
  : SettingsStorage(parent)
  , _st(st)
{}

void SettingsStorageImpl::setValue(const QString& k, const QVariant& v)
{
  _mod_keys.insert(k);
  _st.setValue(k, v);
}

QVariant SettingsStorageImpl::value(const QString& k, const QVariant& d) const
{
  return _st.value(k, d);
}

void SettingsStorageImpl::commit()
{
  for (const auto& k : std::as_const(_mod_keys))
    _st.commitValue(k);
  _mod_keys.clear();
}

void SettingsStorageImpl::discard()
{
  for (const auto& k : std::as_const(_mod_keys))
    _st.discardValue(k);
  _mod_keys.clear();
}


PrefixedSettingsStorage::PrefixedSettingsStorage(std::unique_ptr<SettingsStorage> st,
                                                 QString prefix, QObject* parent)
  : SettingsStorage(parent)
  , _storage(std::move(st))
  , _prefix(std::move(prefix))
{
}

void PrefixedSettingsStorage::setValue(const QString& k, const QVariant& v)
{
  _storage->setValue(prefixed(k), v);
}

QVariant PrefixedSettingsStorage::value(const QString& k, const QVariant& d) const
{
  return _storage->value(prefixed(k), d);
}

QString PrefixedSettingsStorage::prefixed(const QString& k) const
{
  return QString("%1/%2").arg(_prefix, k);
}
