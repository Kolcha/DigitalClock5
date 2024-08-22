/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "plugin_settings_dialog.hpp"
#include "ui_plugin_settings_dialog.h"

#include <QBoxLayout>

PluginSettingsDialog::PluginSettingsDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::PluginSettingsDialog)
{
  ui->setupUi(this);

  connect(ui->comboBox, &QComboBox::currentIndexChanged,
          this, &PluginSettingsDialog::instanceChanged);
}

PluginSettingsDialog::~PluginSettingsDialog()
{
  delete ui;
}

void PluginSettingsDialog::setNumInstances(int n)
{
  QSignalBlocker _(ui->comboBox);
  for (int i = 0; i < n; i++)
    ui->comboBox->addItem(tr("Instance %1").arg(i+1));
  ui->comboBox->setCurrentIndex(0);
  ui->comboBox->setVisible(n > 1);
}

void PluginSettingsDialog::setWidget(QWidget* w)
{
  delete ui->widget->layout();
  auto l = new QVBoxLayout(ui->widget);
  l->setSpacing(0);
  l->setContentsMargins(0, 0, 0, 0);
  l->addWidget(w);
}
