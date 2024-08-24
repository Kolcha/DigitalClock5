/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin_config.hpp"

#include "date_plugin_impl.hpp"

namespace plugin_impl {

template<class C>
class DatePluginSettings : public PluginConfig<C> {
public:
  explicit DatePluginSettings(C& c) : PluginConfig<C>(c) {}

  PLG_CONFIG_SEC_OPTION_Q(Date, FormatType, DatePluginImpl::FormatType, DatePluginImpl::System)
  PLG_CONFIG_SEC_OPTION_Q(Date, FormatSys, QLocale::FormatType, QLocale::LongFormat)
  PLG_CONFIG_SEC_OPTION_Q(Date, FormatStr, QString, QLocale().dateFormat())
};

} // namespace plugin_impl
