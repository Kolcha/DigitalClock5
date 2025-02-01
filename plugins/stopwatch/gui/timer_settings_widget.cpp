/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "timer_settings_widget.hpp"
#include "ui_timer_settings_widget.h"

namespace timetracker {

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

void TimerSettingsWidget::initControls(StopwatchInstanceConfig* icfg)
{
  Q_ASSERT(icfg);
  cfg = icfg;

  QSignalBlocker _(this);

  ui->hide_if_inactive->setChecked(cfg->getHideInactive());
}

void TimerSettingsWidget::on_hide_if_inactive_clicked(bool checked)
{
  cfg->setHideInactive(checked);
  emit optionChanged(HideInactive, checked);
}

} // namespace timetracker
