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
class HotkeysSettingsWidget;
}

class HotkeysSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  explicit HotkeysSettingsWidget(QWidget* parent = nullptr);
  ~HotkeysSettingsWidget();

  void initControls(StopwatchInstanceConfig* icfg);

signals:
  void optionChanged(Options opt, const QVariant& val);

private slots:
  void on_pause_seq_edit_editingFinished();
  void on_restart_seq_edit_editingFinished();

private:
  Ui::HotkeysSettingsWidget* ui;
  StopwatchInstanceConfig* cfg = nullptr;
};

} // namespace timetracker
