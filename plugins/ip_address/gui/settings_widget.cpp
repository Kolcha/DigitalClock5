// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "settings_widget.hpp"
#include "ui_settings_widget.h"

namespace plugin::ip {

SettingsWidget::SettingsWidget(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::SettingsWidget)
  , iface_model(new NetworkInterfacesModel(this))
{
  ui->setupUi(this);

  auto no_local_model = new LinkLocalAddressesFilterModel(this);
  no_local_model->setSourceModel(iface_model);
  auto only_up_model = new InactiveInterfacesFilterModel(this);
  only_up_model->setSourceModel(no_local_model);
  ui->interfaces_list->setModel(only_up_model);

  for (int r = 0; r < ui->interfaces_list->model()->rowCount(); r++)
    ui->interfaces_list->setExpanded(ui->interfaces_list->model()->index(r, 0), true);

  connect(iface_model, &NetworkInterfacesModel::selectedIPsChanged, this, &SettingsWidget::onInterfacesListChanged);
}

SettingsWidget::~SettingsWidget()
{
  delete ui;
}

void SettingsWidget::initControls(IpAddressPluginInstanceConfig* icfg)
{
  Q_ASSERT(icfg);
  cfg = icfg;

  QSignalBlocker _(this);

  ui->show_internal_addr_cbox->setChecked(cfg->getShowInternal());
  iface_model->setSelectedIPs(cfg->getInternalAddresses());
  ui->show_external_addr_cbox->setChecked(cfg->getShowExternalIPv4() || cfg->getShowExternalIPv6());
  ui->show_external_v4_cbox->setChecked(cfg->getShowExternalIPv4());
  ui->show_external_v6_cbox->setChecked(cfg->getShowExternalIPv6());
}

void SettingsWidget::onInterfacesListChanged(const NetworkInterfacesModel::SelectedIPs& ips)
{
  cfg->setInternalAddresses(ips);
  emit addressesListChanged();
}

void SettingsWidget::on_show_internal_addr_cbox_clicked(bool checked)
{
  cfg->setShowInternal(checked);
  emit addressesListChanged();
}

void SettingsWidget::on_show_external_addr_cbox_clicked(bool checked)
{
  cfg->setShowExternalIPv4(checked ? ui->show_external_v4_cbox->isChecked() : false);
  cfg->setShowExternalIPv6(checked ? ui->show_external_v6_cbox->isChecked() : false);
  emit addressesListChanged();
}

void SettingsWidget::on_show_external_v4_cbox_clicked(bool checked)
{
  cfg->setShowExternalIPv4(checked);
  emit addressesListChanged();
}

void SettingsWidget::on_show_external_v6_cbox_clicked(bool checked)
{
  cfg->setShowExternalIPv6(checked);
  emit addressesListChanged();
}

} // namespace plugin::ip
