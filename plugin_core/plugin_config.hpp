/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin_core_global.hpp"

#include "config/settings_storage.hpp"

class PLUGIN_CORE_EXPORT PluginSettingsStorage : public SettingsStorageClient,
                                                 public ISettingsStorage {
public:
  PluginSettingsStorage(QString prefix, ISettingsStorage& st);

  void setValue(QString key, QVariant val) override;
  QVariant value(QString key, QVariant def) const override;

  // actually should not be used, but still provided for completeness
  void commitValue(QString key) override;
  void discardValue(QString key) override;
};
