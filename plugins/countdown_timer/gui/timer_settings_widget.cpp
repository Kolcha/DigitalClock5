/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "timer_settings_widget.hpp"
#include "ui_timer_settings_widget.h"

#include <QFileDialog>

namespace countdown_timer {

TimerSettingsWidget::TimerSettingsWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::TimerSettingsWidget)
{
  ui->setupUi(this);
}

TimerSettingsWidget::~TimerSettingsWidget()
{
  delete ui;
}

void TimerSettingsWidget::initControls(CountdownTimerInstanceConfig* icfg)
{
  Q_ASSERT(icfg);
  cfg = icfg;

  QSignalBlocker _(this);

  ui->target_time_rbtn->setChecked(cfg->getUseTargetDateTime());
  ui->target_time_edit->setDateTime(cfg->getTargetDateTime());

  ui->interval_rbtn->setChecked(!cfg->getUseTargetDateTime());
  ui->h_edit->setValue(cfg->getIntervalHours());
  ui->m_edit->setValue(cfg->getIntervalMinutes());
  ui->s_edit->setValue(cfg->getIntervalSeconds());

  ui->min_days_edit->setValue(cfg->getHideDaysThreshold());
  ui->also_hide_hours->setChecked(cfg->getAlsoHideHours());

  ui->restart_on_dblclik->setChecked(cfg->getRestartOnDblClick());
  ui->restart_on_timeout->setChecked(cfg->getRestartOnTimeout());

  ui->hide_if_inactive->setChecked(cfg->getHideInactive());
  ui->reverse_counting->setChecked(cfg->getReverseCounting());

  ui->chime_on_timeout->setChecked(cfg->getChimeOnTimeout());
  ui->show_msg->setChecked(cfg->getShowMessage());
  ui->msg_text_edit->setPlainText(cfg->getMessageText());
}

void TimerSettingsWidget::on_target_time_rbtn_clicked()
{
  cfg->setUseTargetDateTime(true);
  emit optionChanged(UseTargetDateTime, true);
}

void TimerSettingsWidget::on_target_time_edit_dateTimeChanged(const QDateTime& dateTime)
{
  cfg->setTargetDateTime(dateTime);
  emit optionChanged(TargetDateTime, dateTime);
}

void TimerSettingsWidget::on_interval_rbtn_clicked()
{
  cfg->setUseTargetDateTime(false);
  emit optionChanged(UseTargetDateTime, false);
}

void TimerSettingsWidget::on_h_edit_valueChanged(int arg1)
{
  cfg->setIntervalHours(arg1);
  emit optionChanged(IntervalHours, arg1);
}

void TimerSettingsWidget::on_m_edit_valueChanged(int arg1)
{
  cfg->setIntervalMinutes(arg1);
  emit optionChanged(IntervalMinutes, arg1);
}

void TimerSettingsWidget::on_s_edit_valueChanged(int arg1)
{
  cfg->setIntervalSeconds(arg1);
  emit optionChanged(IntervalSeconds, arg1);
}

void TimerSettingsWidget::on_min_days_edit_valueChanged(int arg1)
{
  ui->also_hide_hours->setEnabled(arg1 == -1);
  cfg->setHideDaysThreshold(arg1);
  emit optionChanged(HideDaysThreshold, arg1);
}

void TimerSettingsWidget::on_also_hide_hours_clicked(bool checked)
{
  cfg->setAlsoHideHours(checked);
  emit optionChanged(AlsoHideHours, checked);
}

void TimerSettingsWidget::on_restart_on_dblclik_clicked(bool checked)
{
  cfg->setRestartOnDblClick(checked);
  emit optionChanged(RestartOnDblClick, checked);
}

void TimerSettingsWidget::on_restart_on_timeout_clicked(bool checked)
{
  cfg->setRestartOnTimeout(checked);
  emit optionChanged(RestartOnTimeout, checked);
}

void TimerSettingsWidget::on_hide_if_inactive_clicked(bool checked)
{
  cfg->setHideInactive(checked);
  emit optionChanged(HideInactive, checked);
}

void TimerSettingsWidget::on_reverse_counting_clicked(bool checked)
{
  cfg->setReverseCounting(checked);
  emit optionChanged(ReverseCounting, checked);
}

void TimerSettingsWidget::on_chime_on_timeout_clicked(bool checked)
{
  cfg->setChimeOnTimeout(checked);
  emit optionChanged(ChimeOnTimeout, checked);
}

void TimerSettingsWidget::on_browse_sound_btn_clicked()
{
  QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                  QDir::homePath(),
                                                  tr("Sounds (*.wav *.mp3 *.ogg *.oga *.m4a)"));
  if (filename.isEmpty()) return;

  cfg->setChimeSoundFile(filename);
  emit optionChanged(ChimeSoundFile, filename);
}

void TimerSettingsWidget::on_show_msg_clicked(bool checked)
{
  cfg->setShowMessage(checked);
  emit optionChanged(ShowMessage, checked);
}

void TimerSettingsWidget::on_msg_text_edit_textChanged()
{
  cfg->setMessageText(ui->msg_text_edit->toPlainText());
  emit optionChanged(MessageText, cfg->getMessageText());
}

} // namespace countdown_timer
