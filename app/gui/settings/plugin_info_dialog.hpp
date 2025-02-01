/*
 * SPDX-FileCopyrightText: 2013-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>

namespace Ui {
class PluginInfoDialog;
}

class PluginHandle;

class PluginInfoDialog : public QDialog
{
  Q_OBJECT

public:
  explicit PluginInfoDialog(const PluginHandle& ph,
                            QWidget* parent = nullptr);
  ~PluginInfoDialog();

private:
  Ui::PluginInfoDialog* ui;
};
