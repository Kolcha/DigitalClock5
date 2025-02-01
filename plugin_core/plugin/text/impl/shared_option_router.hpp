// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "plugin/text/plugin_config.hpp"

namespace plugin::text::impl {

class SharedOptionRouter
{
public:
  explicit SharedOptionRouter(const PluginInstanceConfig& pc) noexcept
    : _plugin_settings(pc)
  {}

  bool should_route(opt::InstanceOptions o) const;

  template<typename T>
  T value(opt::InstanceOptions o) const
  {
    return should_route(o) ? clock_value<T>(o) : plugin_value<decltype(o), T>(o);
  }

  void onClockOptionChanged(opt::InstanceOptions opt, const QVariant& val)
  {
    _clock_settings[opt] = val;
  }

private:
  template<typename T>
  T clock_value(opt::InstanceOptions o) const
  {
    return _clock_settings.value(o).value<T>();
  }

  template<typename Opt, typename T>
  T plugin_value(Opt o) const
  {
    return _plugin_settings.value<Opt, T>(o, PluginInstanceConfig::def_value(o).template value<T>());
  }

private:
  const PluginInstanceConfig& _plugin_settings;
  QHash<opt::InstanceOptions, QVariant> _clock_settings;
};

} // namespace plugin::text::impl
