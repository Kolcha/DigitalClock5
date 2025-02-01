/*
 * SPDX-FileCopyrightText: 2014-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "plugin_list_item_widget.hpp"
#include "ui_plugin_list_item_widget.h"

PluginListItemWidget::PluginListItemWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PluginListItemWidget)
{
  ui->setupUi(this);
  ui->config_btn->setIcon(QIcon(":/icons/configure.svg"));
  ui->info_btn->setIcon(QIcon(":/icons/help-about.svg"));

  connect(ui->name_check_box, &QCheckBox::clicked,
          this, &PluginListItemWidget::StateChanged);
  connect(ui->config_btn, &QToolButton::clicked,
          this, &PluginListItemWidget::ConfigureRequested);
  connect(ui->info_btn, &QToolButton::clicked,
          this, &PluginListItemWidget::InfoDialogRequested);
}

PluginListItemWidget::~PluginListItemWidget()
{
  delete ui;
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
