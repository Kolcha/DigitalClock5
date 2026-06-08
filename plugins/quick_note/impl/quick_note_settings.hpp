/*
 * SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/text/plugin_config.hpp"

namespace plugin::quick_note {

class QuickNotePluginInstanceConfig : public text::PluginInstanceConfig
{
  Q_OBJECT

public:
  using PluginInstanceConfig::PluginInstanceConfig;

  CONFIG_OPTION(LastText, "last_text", QString, tr("double click me!"))
};


class QuickNotePluginConfig : public text::PluginConfig
{
  Q_OBJECT

public:
  using PluginConfig::PluginConfig;

protected:
  std::unique_ptr<text::PluginInstanceConfig>
  createInstanceImpl(std::unique_ptr<SettingsStorage> st) const override
  {
    return std::make_unique<QuickNotePluginInstanceConfig>(std::move(st));
  }
};

} // namespace plugin::quick_note
