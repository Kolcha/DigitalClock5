/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "settings_storage.hpp"

SettingsStorage::SettingsStorage(const QString& filename)
    : _storage(filename, QSettings::IniFormat)
{
}

void SettingsStorage::setValue(QString key, QVariant val)
{
  _runtime[key] = std::move(val);
}

QVariant SettingsStorage::value(QString key, QVariant def) const
{
  auto iter = _runtime.find(key);
  if (iter != _runtime.end())
    return *iter;
  auto val = _storage.value(key, def);
  _runtime[key] = val;
  return val;
}

void SettingsStorage::commitValue(QString key)
{
  auto iter = _runtime.find(key);
  if (iter == _runtime.end())
    return;
  return _storage.setValue(key, *iter);
}

void SettingsStorage::discardValue(QString key)
{
  _runtime.remove(key);
}

void SettingsStorage::commitAll()
{
  for (auto i = _runtime.cbegin(); i != _runtime.cend(); ++i)
    _storage.setValue(i.key(), i.value());
}

void SettingsStorage::discardAll()
{
  _runtime.clear();
}

void SettingsStorage::exportSettings(QVariantHash& s) const
{
  const auto keys = _storage.allKeys();
  for (const auto& k : keys) s[k] = _storage.value(k);
  s.insert(_runtime);
}

void SettingsStorage::importSettings(const QVariantHash& s)
{
  _runtime.insert(s);
}


SettingsStorageClient::SettingsStorageClient(QString title, ISettingsStorage& st)
    : _st(st)
    , _title(std::move(title))
{
}

void SettingsStorageClient::commit()
{
  keysOperation(&ISettingsStorage::commitValue);
}

void SettingsStorageClient::discard()
{
  keysOperation(&ISettingsStorage::discardValue);
}

void SettingsStorageClient::setValue(QString key, QVariant val)
{
  auto st_key = storageKey(key);
  _st.setValue(st_key, std::move(val));
  _keys.insert(std::move(st_key));
}

QVariant SettingsStorageClient::value(QString key, QVariant def) const
{
  return _st.value(storageKey(key), std::move(def));
}

QString SettingsStorageClient::storageKey(QStringView key) const
{
  return QString("%1/%2").arg(_title, key);
}

void SettingsStorageClient::keysOperation(op_type op)
{
  for (const auto& k : std::as_const(_keys)) (_st.*op)(k);
  _keys.clear();
}


StateClient::StateClient(QString title, ISettingsStorage& st)
    : _st(st)
    , _title(title)
{
}

void StateClient::setValue(QString key, QVariant val)
{
  auto st_key = storageKey(key);
  _st.setValue(st_key, std::move(val));
  _st.commitValue(st_key);
}

QVariant StateClient::value(QString key, QVariant def) const
{
  return _st.value(storageKey(key), std::move(def));
}

QString StateClient::storageKey(QStringView key) const
{
  return QString("%1/%2").arg(_title, key);
}
