/*
 * SPDX-FileCopyrightText: 2016-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "impl/chime_plugin_impl.hpp"

namespace chime {

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
  Q_OBJECT

public:
  explicit SettingsWidget(PluginSettingsStorage& s, StateClient& t,
                          ChimePluginImpl* impl, QWidget* parent = nullptr);
  ~SettingsWidget();

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
  Ui::SettingsWidget* ui;
  plugin_impl::ChimePluginSettings cfg;
  StateClient& st;
  ChimePluginImpl* impl;
};

} // namespace chime
