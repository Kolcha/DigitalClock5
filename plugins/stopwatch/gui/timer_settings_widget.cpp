/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "timer_settings_widget.hpp"
#include "ui_timer_settings_widget.h"

TimerSettingsWidget::TimerSettingsWidget(SettingsClient& s, StateClient& t,
                                         StopwatchPluginImpl* impl, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::TimerSettingsWidget)
    , cfg(s)
    , impl(impl)
{
  ui->setupUi(this);

  ui->hide_if_inactive->setChecked(cfg.getHideInactive());
}

TimerSettingsWidget::~TimerSettingsWidget()
{
  delete ui;
}

void TimerSettingsWidget::on_hide_if_inactive_clicked(bool checked)
{
  cfg.setHideInactive(checked);
  impl->hide_inactive = checked;
  impl->applySettings();
}
