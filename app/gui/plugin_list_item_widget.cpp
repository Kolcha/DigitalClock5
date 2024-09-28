/*
 * SPDX-FileCopyrightText: 2014-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "plugin_list_item_widget.hpp"
#include "ui_plugin_list_item_widget.h"

#include "plugin_info_dialog.hpp"

PluginListItemWidget::PluginListItemWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PluginListItemWidget)
{
  ui->setupUi(this);
  ui->config_btn->setIcon(QIcon(":/icons/configure.svg"));
  ui->info_btn->setIcon(QIcon(":/icons/help-about.svg"));
}

PluginListItemWidget::~PluginListItemWidget()
{
  delete ui;
}

QString PluginListItemWidget::GetName() const
{
  return _info.id;
}

void PluginListItemWidget::SetInfo(PluginInfo info)
{
  _info = std::move(info);
  SetDisplayName(_info.title);
  SetVersion(_info.metadata.value("version").toString());
  SetConfigurable(_info.configurable);
  SetChecked(_info.enabled);
}

void PluginListItemWidget::SetDisplayName(const QString& name)
{
  ui->name_check_box->setText(name);
}

void PluginListItemWidget::SetVersion(const QString& version)
{
  ui->version_label->setText(version);
}

void PluginListItemWidget::SetConfigurable(bool configable)
{
  ui->config_btn->setEnabled(configable);
}

void PluginListItemWidget::SetChecked(bool checked)
{
  ui->name_check_box->setChecked(checked);
}

void PluginListItemWidget::on_name_check_box_toggled(bool checked)
{
  emit StateChanged(GetName(), checked);
}

void PluginListItemWidget::on_config_btn_clicked()
{
  emit ConfigureRequested(GetName());
}

void PluginListItemWidget::on_info_btn_clicked()
{
  PluginInfoDialog dialog(window());
  dialog.SetInfo(_info);
  dialog.exec();
}
