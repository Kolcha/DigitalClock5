/*
 * SPDX-FileCopyrightText: 2014-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "core/plugin_info.hpp"

namespace Ui {
class PluginListItemWidget;
}

class PluginListItemWidget : public QWidget
{
  Q_OBJECT

public:
  explicit PluginListItemWidget(QWidget* parent = nullptr);
  ~PluginListItemWidget();

  QString GetName() const;

  void SetInfo(PluginInfo info);

public slots:
  void SetDisplayName(const QString& name);
  void SetVersion(const QString& version);
  void SetConfigurable(bool configable);
  void SetChecked(bool checked);

signals:
  void StateChanged(const QString& name, bool new_state);
  void ConfigureRequested(const QString& name);

private slots:
  void on_name_check_box_toggled(bool checked);
  void on_config_btn_clicked();
  void on_info_btn_clicked();

private:
  Ui::PluginListItemWidget* ui;
  PluginInfo _info;
};
