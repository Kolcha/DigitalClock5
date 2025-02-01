/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "impl/stopwatch_settings.hpp"

namespace timetracker {

namespace Ui {
class TimerSettingsWidget;
}

class TimerSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  explicit TimerSettingsWidget(QWidget* parent = nullptr);
  ~TimerSettingsWidget();

  void initControls(StopwatchInstanceConfig* icfg);

signals:
  void optionChanged(Options opt, const QVariant& val);

private slots:
  void on_hide_if_inactive_clicked(bool checked);

private:
  Ui::TimerSettingsWidget* ui;
  StopwatchInstanceConfig* cfg;
};

} // namespace timetracker
