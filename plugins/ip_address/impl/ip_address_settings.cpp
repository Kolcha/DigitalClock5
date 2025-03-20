// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ip_address_settings.hpp"

namespace plugin::ip {

NetworkInterfacesModel::SelectedIPs IpAddressPluginInstanceConfig::getInternalAddresses() const
{
  NetworkInterfacesModel::SelectedIPs ips;

  const auto ifaces = ConfigBase::value("interfaces", QStringList());
  for (const auto& iface : ifaces)
    ips[iface] = ConfigBase::value(iface, 0);

  return ips;
}

void IpAddressPluginInstanceConfig::setInternalAddresses(const NetworkInterfacesModel::SelectedIPs& ips)
{
  for (auto iter = ips.begin(); iter != ips.end(); ++iter)
    ConfigBase::setValue(iter.key(), iter.value());
  ConfigBase::setValue("interfaces", ips.keys());
}

} // namespace plugin::ip
