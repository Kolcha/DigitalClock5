/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "config/settings_storage.hpp"
#include "config/custom_converters.hpp"

#include "date_plugin_impl.hpp"

namespace plugin_impl {

class DatePluginSettings : public SettingsStorageClient {
public:
  explicit DatePluginSettings(ISettingsStorage& st)
      : SettingsStorageClient("Date", st) {}

  CONFIG_OPTION_Q(DatePluginImpl::FormatType, FormatType, DatePluginImpl::System)
  CONFIG_OPTION_Q(QLocale::FormatType, FormatSys, QLocale::LongFormat)
  CONFIG_OPTION_Q(QString, FormatStr, QLocale().dateFormat())
};

} // namespace plugin_impl
