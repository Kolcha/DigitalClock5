// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ip_address_plugin.hpp"

// #include <functional>

#include <QTimer>
#include <QNetworkInterface>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "gui/settings_widget.hpp"

IpAddressPlugin::IpAddressPlugin(const IpAddressPluginInstanceConfig* cfg)
  : TextPluginInstanceBase(*cfg)
  , _cfg(cfg)
{
}

void IpAddressPlugin::startup()
{
  _qnam = new QNetworkAccessManager(this);
  _ip_update_timer = new QTimer();
  connect(_ip_update_timer, &QTimer::timeout, this, &IpAddressPlugin::UpdateIPsList);
  _ip_update_timer->setSingleShot(false);
  _ip_update_timer->start(15 * 60 * 1000);
  UpdateIPsList();
  TextPluginInstanceBase::startup();
}

void IpAddressPlugin::shutdown()
{
  TextPluginInstanceBase::shutdown();
  _ip_update_timer->stop();
  _ip_update_timer->deleteLater();
  _qnam->deleteLater();
}

void IpAddressPlugin::UpdateIPsList()
{
  _last_ip_list.clear();

  if (_cfg->getShowInternal()) {
    // get interested internal addresses
    const auto ips = _cfg->getInternalAddresses();
    for (auto iter = ips.begin(); iter != ips.end(); ++iter) {
      const auto& iname = iter.key();
      const auto& idxes = iter.value();
      auto iface = QNetworkInterface::interfaceFromName(iname);
      if (!iface.isValid()) continue;
      if (iface.flags() & QNetworkInterface::IsUp) {
        const auto addr_entries = iface.addressEntries();
        for (qsizetype i = 0; i < addr_entries.size(); i++) {
          if ((idxes & (1 << i)) != 0)
            _last_ip_list.append(addr_entries[i].ip().toString());
        }
      }
    }
  }

  if (_cfg->getShowExternalIPv4() && !_getting_external_ip4) {
    // get external IPv4 address
    requestExternalAddress(false);
  }

  if (_cfg->getShowExternalIPv6() && !_getting_external_ip6) {
    // get external IPv6 address
    requestExternalAddress(true);
  }

  if (_last_ip_list.isEmpty() && !_getting_external_ip4)
    _last_ip_list.append(tr("<no interfaces selected>"));
}

void IpAddressPlugin::pluginReloadConfig()
{
  if (!_getting_external_ip4 && !_getting_external_ip6)
    UpdateIPsList();
}

void IpAddressPlugin::requestExternalAddress(bool ipv6)
{
  auto getting_external_ip = ipv6 ? &_getting_external_ip6 : &_getting_external_ip4;
  auto ipify_api_url = ipv6 ? "https://api6.ipify.org" : "https://api.ipify.org/";

  const auto external_placeholder = tr("waiting for an external address...");

  *getting_external_ip = true;
  _last_ip_list.append(external_placeholder);
  QNetworkReply* reply = _qnam->get(QNetworkRequest(QUrl(ipify_api_url)));
  connect(reply, &QNetworkReply::finished, [=, this] () {
    *getting_external_ip = false;
    auto i = _last_ip_list.indexOf(external_placeholder);
    Q_ASSERT(i != -1);
    if (reply->error() == QNetworkReply::NoError)
      _last_ip_list[i] = QString::fromLatin1(reply->readAll());
    else
      _last_ip_list[i] = reply->errorString();
    reply->deleteLater();
    repaintWidget();
  });
}


QString IpAddressPluginFactory::description() const
{
  return tr("Displays local IP address(es) under clock.");
}

QVector<QWidget*> IpAddressPluginFactory::configPagesBeforeCommonPages()
{
  using plugin::ip::SettingsWidget;
  auto page = new SettingsWidget();
  connect(this, &IpAddressPluginFactory::instanceSwitched, page, [this, page](size_t idx) {
    page->initControls(qobject_cast<IpAddressPluginInstanceConfig*>(instanceConfig(idx)));
    if (!hasInstances()) return;
    disconnect(page, &SettingsWidget::addressesListChanged, nullptr, nullptr);
    auto inst = qobject_cast<IpAddressPlugin*>(instance(idx));
    connect(page, &SettingsWidget::addressesListChanged, inst, &IpAddressPlugin::UpdateIPsList);
  });
  return { page };
}
