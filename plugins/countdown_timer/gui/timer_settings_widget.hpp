/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "impl/countdown_timer_settings.hpp"

namespace countdown_timer {

namespace Ui {
class TimerSettingsWidget;
}

class TimerSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  explicit TimerSettingsWidget(QWidget* parent = nullptr);
  ~TimerSettingsWidget();

  void initControls(CountdownTimerInstanceConfig* icfg);

signals:
  void optionChanged(Options opt, const QVariant& val);

private slots:
  void on_target_time_rbtn_clicked();
  void on_target_time_edit_dateTimeChanged(const QDateTime& dateTime);

  void on_interval_rbtn_clicked();
  void on_h_edit_valueChanged(int arg1);
  void on_m_edit_valueChanged(int arg1);
  void on_s_edit_valueChanged(int arg1);

  void on_min_days_edit_valueChanged(int arg1);
  void on_also_hide_hours_clicked(bool checked);

  void on_restart_on_dblclik_clicked(bool checked);
  void on_restart_on_timeout_clicked(bool checked);

  void on_hide_if_inactive_clicked(bool checked);
  void on_reverse_counting_clicked(bool checked);

  void on_chime_on_timeout_clicked(bool checked);
  void on_browse_sound_btn_clicked();

  void on_show_msg_clicked(bool checked);
  void on_msg_text_edit_textChanged();

private:
  Ui::TimerSettingsWidget* ui;
  CountdownTimerInstanceConfig* cfg;
};

} // namespace countdown_timer
