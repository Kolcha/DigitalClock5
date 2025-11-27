/*
 * SPDX-FileCopyrightText: 2016-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"

#include "voice_config_dialog.hpp"

#define CURRENT_TIME()    QDateTime::currentDateTime().toTimeZone(tz).time()

namespace talking_clock {

SettingsDialog::SettingsDialog(TalkingClockPluginConfig& icfg,
                               const QTimeZone& tz, QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
  , cfg(icfg)
  , tz(tz)
{
  ui->setupUi(this);

  ui->every_hour->setChecked(cfg.getEveryHourEnabled());
  ui->hour_format_edit->setText(cfg.getEveryHourFormat());
  ui->hour_format_edit->setToolTip(CURRENT_TIME().toString(ui->hour_format_edit->text()));

  ui->quarter_hour->setChecked(cfg.getQuarterHourEnabled());
  ui->quarter_format_edit->setText(cfg.getQuarterHourFormat());
  ui->quarter_format_edit->setToolTip(CURRENT_TIME().toString(ui->quarter_format_edit->text()));
}

SettingsDialog::~SettingsDialog()
{
  delete ui;
}

void SettingsDialog::on_voice_config_btn_clicked()
{
  VoiceConfigDialog dlg(this);

  dlg.setVolume(cfg.getVolume());

  dlg.setRate(cfg.getSynthesisRate());
  dlg.setPitch(cfg.getSynthesisPitch());
  dlg.setEngine(cfg.getSynthesisEngine());
  dlg.setLanguage(cfg.getSynthesisLanguage());
  dlg.setVoice(cfg.getSynthesisVoice());

  if (dlg.exec() == QDialog::Rejected) return;
  Q_ASSERT(dlg.result() == QDialog::Accepted);

  cfg.setVolume(dlg.volume());

  cfg.setSynthesisRate(dlg.rate());
  cfg.setSynthesisPitch(dlg.pitch());
  cfg.setSynthesisEngine(dlg.engine());
  cfg.setSynthesisLanguage(dlg.language());
  cfg.setSynthesisVoice(dlg.voice());
}

void SettingsDialog::on_every_hour_clicked(bool checked)
{
  cfg.setEveryHourEnabled(checked);
}

void SettingsDialog::on_quarter_hour_clicked(bool checked)
{
  cfg.setQuarterHourEnabled(checked);
}

void SettingsDialog::on_hour_format_edit_textEdited(const QString& arg1)
{
  ui->hour_format_edit->setToolTip(CURRENT_TIME().toString(arg1));
  cfg.setEveryHourFormat(arg1);
}

void SettingsDialog::on_quarter_format_edit_textEdited(const QString& arg1)
{
  ui->quarter_format_edit->setToolTip(CURRENT_TIME().toString(arg1));
  cfg.setQuarterHourFormat(arg1);
}

} // namespace talking_clock
