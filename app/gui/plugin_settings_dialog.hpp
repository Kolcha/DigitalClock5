/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>

namespace Ui {
class PluginSettingsDialog;
}

class PluginSettingsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit PluginSettingsDialog(QWidget* parent = nullptr);
  ~PluginSettingsDialog();

  void setNumInstances(int n);

  void setWidget(QWidget* w);

signals:
  void instanceChanged(int);

private:
  Ui::PluginSettingsDialog* ui;
};
