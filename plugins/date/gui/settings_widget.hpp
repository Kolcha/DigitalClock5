/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "impl/date_plugin_impl.hpp"
#include "impl/date_settings.hpp"

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
  Q_OBJECT

public:
  SettingsWidget(PluginSettingsStorage& s, StateClient& t,
                 DatePluginImpl* impl, QWidget* parent = nullptr);
  ~SettingsWidget();

private slots:
  void on_sys_format_rbtn_clicked();
  void on_sys_format_box_activated(int index);

  void on_str_format_rbtn_clicked();
  void on_str_format_edit_editTextChanged(const QString& arg1);
  void on_str_format_help_btn_clicked();

private:
  Ui::SettingsWidget* ui;
  plugin_impl::DatePluginSettings cfg;
  DatePluginImpl* impl;
};
