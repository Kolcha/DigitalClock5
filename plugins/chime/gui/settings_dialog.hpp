/*
 * SPDX-FileCopyrightText: 2016-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>

#include "impl/chime_settings.hpp"

namespace chime {

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  SettingsDialog(ChimePluginConfig& icfg, SettingsStorage& st,
                 QWidget* parent = nullptr);
  ~SettingsDialog();

private slots:
  void on_hour_enabled_cb_clicked(bool checked);
  void on_hour_count_box_activated(int index);
  void on_hour_sound_btn_clicked();

  void on_quarter_enabled_cb_clicked(bool checked);
  void on_quarter_count_box_activated(int index);
  void on_quarter_sound_btn_clicked();

  void on_quiet_hours_enabled_clicked(bool checked);
  void on_q_time_start_timeChanged(const QTime& time);
  void on_q_time_end_timeChanged(const QTime& time);

private:
  Ui::SettingsDialog* ui;
  ChimePluginConfig& cfg;
  SettingsStorage& state;
};

} // namespace chime
