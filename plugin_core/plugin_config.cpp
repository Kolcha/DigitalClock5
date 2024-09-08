/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "plugin_config.hpp"

PluginSettingsStorage::PluginSettingsStorage(QString prefix, ISettingsStorage& st)
    : SettingsStorageClient(prefix, st)
{
}

void PluginSettingsStorage::setValue(QString key, QVariant val)
{
  SettingsStorageClient::setValue(std::move(key), std::move(val));
}

QVariant PluginSettingsStorage::value(QString key, QVariant def) const
{
  return SettingsStorageClient::value(std::move(key), std::move(def));
}

void PluginSettingsStorage::commitValue(QString key)
{
  storage().commitValue(storageKey(key));
}

void PluginSettingsStorage::discardValue(QString key)
{
  storage().discardValue(storageKey(key));
}
