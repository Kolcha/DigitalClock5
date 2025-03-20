// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "plugin/text/skinned_text_plugin_base.hpp"

#include "impl/ip_address_settings.hpp"

class QTimer;
class QNetworkAccessManager;
class QNetworkReply;

using plugin::ip::IpAddressPluginInstanceConfig;

class IpAddressPlugin : public plugin::text::TextPluginInstanceBase
{
  Q_OBJECT

public:
  IpAddressPlugin(const IpAddressPluginInstanceConfig* cfg);

public slots:
  void startup() override;
  void shutdown() override;

  void UpdateIPsList();

protected:
  QString text() const override { return _last_ip_list.join('\n'); }

  void pluginReloadConfig() override;

private:
  void requestExternalAddress(bool ipv6 = false);

private:
  const IpAddressPluginInstanceConfig* _cfg;
  QTimer* _ip_update_timer = nullptr;
  QStringList _last_ip_list;
  QNetworkAccessManager* _qnam = nullptr;
  bool _getting_external_ip4 = false;
  bool _getting_external_ip6 = false;
};


class IpAddressPluginFactory : public plugin::text::TextPluginBase
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "ip_address.json")
  Q_INTERFACES(ClockPlugin)

public:
  QString name() const override { return tr("IP address"); }
  QString description() const override;

protected:
  Instance createInstance(size_t idx) const override
  {
    auto cfg = qobject_cast<IpAddressPluginInstanceConfig*>(instanceConfig(idx));
    Q_ASSERT(cfg);
    return std::make_unique<IpAddressPlugin>(cfg);
  }

  std::unique_ptr<plugin::text::PluginConfig> createConfig(
    std::unique_ptr<PluginSettingsBackend> b) const override
  {
    using plugin::ip::IpAddressPluginConfig;
    return std::make_unique<IpAddressPluginConfig>(std::move(b));
  }

  QVector<QWidget*> configPagesBeforeCommonPages() override;
};
