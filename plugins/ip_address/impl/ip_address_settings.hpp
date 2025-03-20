// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "plugin/text/plugin_config.hpp"

#include "network_interfaces_model.hpp"

namespace plugin::ip {

class IpAddressPluginInstanceConfig : public text::PluginInstanceConfig
{
  Q_OBJECT

public:
  using PluginInstanceConfig::PluginInstanceConfig;

  CONFIG_OPTION(ShowInternal, "show_internal_addresses", bool, false)
  CONFIG_OPTION(ShowExternalIPv4, "show_external_ipv4", bool, true)
  CONFIG_OPTION(ShowExternalIPv6, "show_external_ipv6", bool, false)

  NetworkInterfacesModel::SelectedIPs getInternalAddresses() const;
  void setInternalAddresses(const NetworkInterfacesModel::SelectedIPs& ips);
};


class IpAddressPluginConfig : public text::PluginConfig
{
  Q_OBJECT

public:
  using PluginConfig::PluginConfig;

protected:
  std::unique_ptr<text::PluginInstanceConfig>
  createInstanceImpl(std::unique_ptr<SettingsStorage> st) const override
  {
    return std::make_unique<IpAddressPluginInstanceConfig>(std::move(st));
  }
};

} // namespace plugin::ip
