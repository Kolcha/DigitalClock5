/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "impl/stopwatch_plugin_impl.hpp"
#include "impl/stopwatch_settings.hpp"

namespace Ui {
class TimerSettingsWidget;
}

class TimerSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  explicit TimerSettingsWidget(PluginSettingsStorage& s, StateClient& t,
                               StopwatchPluginImpl* impl, QWidget* parent = nullptr);
  ~TimerSettingsWidget();

private slots:
  void on_hide_if_inactive_clicked(bool checked);

private:
  Ui::TimerSettingsWidget* ui;
  plugin_impl::StopwatchPluginSettings cfg;
  StopwatchPluginImpl* impl;
};
