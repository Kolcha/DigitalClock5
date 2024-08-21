/*
 * SPDX-FileCopyrightText: 2013-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>

#include "core/plugin_info.hpp"

namespace Ui {
class PluginInfoDialog;
}

class PluginInfoDialog : public QDialog
{
  Q_OBJECT

public:
  explicit PluginInfoDialog(QWidget* parent = nullptr);
  ~PluginInfoDialog();

  void SetInfo(const PluginInfo& info);

private:
  Ui::PluginInfoDialog* ui;
};
