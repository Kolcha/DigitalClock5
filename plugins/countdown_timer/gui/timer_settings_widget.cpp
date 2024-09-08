/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "timer_settings_widget.hpp"
#include "ui_timer_settings_widget.h"

#include <QFileDialog>

TimerSettingsWidget::TimerSettingsWidget(PluginSettingsStorage& s, StateClient& t,
                                         CountdownTimerPluginImpl* impl, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::TimerSettingsWidget)
    , cfg(s)
    , impl(impl)
{
  ui->setupUi(this);

  ui->target_time_rbtn->setChecked(cfg.getUseTargetDateTime());
  ui->target_time_edit->setDateTime(cfg.getTargetDateTime());

  ui->interval_rbtn->setChecked(!cfg.getUseTargetDateTime());
  ui->h_edit->setValue(cfg.getIntervalHours());
  ui->m_edit->setValue(cfg.getIntervalMinutes());
  ui->s_edit->setValue(cfg.getIntervalSeconds());

  ui->min_days_edit->setValue(cfg.getHideDaysThreshold());
  ui->also_hide_hours->setChecked(cfg.getAlsoHideHours());

  ui->restart_on_dblclik->setChecked(cfg.getRestartOnDblClick());
  ui->restart_on_timeout->setChecked(cfg.getRestartOnTimeout());

  ui->hide_if_inactive->setChecked(cfg.getHideInactive());
  ui->reverse_counting->setChecked(cfg.getReverseCounting());

  ui->chime_on_timeout->setChecked(cfg.getChimeOnTimeout());
  ui->show_msg->setChecked(cfg.getShowMessage());
  ui->msg_text_edit->setPlainText(cfg.getMessageText());
}

TimerSettingsWidget::~TimerSettingsWidget()
{
  delete ui;
}

void TimerSettingsWidget::on_target_time_rbtn_clicked()
{
  cfg.setUseTargetDateTime(true);
  impl->use_target_datetime = true;
}

void TimerSettingsWidget::on_target_time_edit_dateTimeChanged(const QDateTime& dateTime)
{
  cfg.setTargetDateTime(dateTime);
  impl->target_date_time = dateTime;
}

void TimerSettingsWidget::on_interval_rbtn_clicked()
{
  cfg.setUseTargetDateTime(false);
  impl->use_target_datetime = false;
}

void TimerSettingsWidget::on_h_edit_valueChanged(int arg1)
{
  cfg.setIntervalHours(arg1);
  impl->interval_hours = arg1;
}

void TimerSettingsWidget::on_m_edit_valueChanged(int arg1)
{
  cfg.setIntervalMinutes(arg1);
  impl->interval_minutes = arg1;
}

void TimerSettingsWidget::on_s_edit_valueChanged(int arg1)
{
  cfg.setIntervalSeconds(arg1);
  impl->interval_seconds = arg1;
}

void TimerSettingsWidget::on_min_days_edit_valueChanged(int arg1)
{
  ui->also_hide_hours->setEnabled(arg1 == -1);
  cfg.setHideDaysThreshold(arg1);
  impl->hide_days_threshold = arg1;
  impl->updateWidgetText();
}

void TimerSettingsWidget::on_also_hide_hours_clicked(bool checked)
{
  cfg.setAlsoHideHours(checked);
  impl->also_hide_hours = checked;
  impl->updateWidgetText();
}

void TimerSettingsWidget::on_restart_on_dblclik_clicked(bool checked)
{
  cfg.setRestartOnDblClick(checked);
  impl->restart_on_dbl_click = checked;
}

void TimerSettingsWidget::on_restart_on_timeout_clicked(bool checked)
{
  cfg.setRestartOnTimeout(checked);
  impl->restart_on_timeout = checked;
}

void TimerSettingsWidget::on_hide_if_inactive_clicked(bool checked)
{
  cfg.setHideInactive(checked);
  impl->hide_inactive = checked;
  impl->applySettings();
}

void TimerSettingsWidget::on_reverse_counting_clicked(bool checked)
{
  cfg.setReverseCounting(checked);
  impl->reverse_counting = checked;
  impl->updateWidgetText();
}

void TimerSettingsWidget::on_chime_on_timeout_clicked(bool checked)
{
  cfg.setChimeOnTimeout(checked);
  impl->chime_on_timeout = checked;
}

void TimerSettingsWidget::on_browse_sound_btn_clicked()
{
  QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                  QDir::homePath(),
                                                  tr("Sounds (*.wav *.mp3 *.ogg *.oga *.m4a)"));
  if (filename.isEmpty()) return;

  cfg.setChimeSoundFile(filename);
  impl->chime_sound = filename;
}

void TimerSettingsWidget::on_show_msg_clicked(bool checked)
{
  cfg.setShowMessage(checked);
  impl->show_message = checked;
}

void TimerSettingsWidget::on_msg_text_edit_textChanged()
{
  cfg.setMessageText(ui->msg_text_edit->toPlainText());
  impl->message_text = ui->msg_text_edit->toPlainText();
}
