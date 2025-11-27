/*
 * SPDX-FileCopyrightText: 2016-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>

#include "impl/talking_clock_settings.hpp"

namespace talking_clock {

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  SettingsDialog(TalkingClockPluginConfig& icfg,
                 const QTimeZone& tz,
                 QWidget* parent = nullptr);
  ~SettingsDialog();

private slots:
  void on_voice_config_btn_clicked();

  void on_every_hour_clicked(bool checked);
  void on_quarter_hour_clicked(bool checked);

  void on_hour_format_edit_textEdited(const QString& arg1);
  void on_quarter_format_edit_textEdited(const QString& arg1);

private:
  Ui::SettingsDialog* ui;
  TalkingClockPluginConfig& cfg;
  QTimeZone tz;
};

} // namespace talking_clock
