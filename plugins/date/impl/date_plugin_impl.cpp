/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "date_plugin_impl.hpp"

#include "date_settings.hpp"

void DatePluginImpl::initSettings(const SettingsClient& st)
{
  plugin_impl::DatePluginSettings cfg(st);
  format_type = cfg.getFormatType();
  sys_format = cfg.getFormatSys();
  str_format = cfg.getFormatStr();
}

void DatePluginImpl::updateWidget()
{
  if (!widget)
    return;

  QString fmt = str_format;
  if (format_type == System) {
    auto fmt_type = QLocale::LongFormat;
    if (sys_format != fmt_type)
      fmt_type = QLocale::ShortFormat;
    fmt = QLocale().dateFormat(fmt_type);
  }

  widget->setFormat(std::move(fmt));
}
