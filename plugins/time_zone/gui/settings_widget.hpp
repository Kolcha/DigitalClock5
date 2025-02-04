/*
 * SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "impl/time_zone_settings.hpp"

namespace plugin::time_zone {

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
  Q_OBJECT

public:
  SettingsWidget(QWidget* parent = nullptr);
  ~SettingsWidget();

  void initControls(TimeZonePluginInstanceConfig* icfg);

signals:
  void tzFormatChanged();

private slots:
  void on_str_format_edit_editTextChanged(const QString& arg1);
  void on_str_format_help_btn_clicked();

private:
  Ui::SettingsWidget* ui;
  TimeZonePluginInstanceConfig* cfg = nullptr;
};

} // namespace plugin::time_zone
