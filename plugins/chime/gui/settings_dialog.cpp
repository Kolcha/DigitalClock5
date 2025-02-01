/*
 * SPDX-FileCopyrightText: 2016-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"

#include "impl/chime_settings.hpp"

#include "gui/sound_dialog.hpp"

namespace {

int setIndexByValue(QComboBox* box, const QVariant& value)
{
  auto idx = box->findData(value);
  if (idx >= 0)
    box->setCurrentIndex(idx);
  return idx;
}

static constexpr const char* const EVERY_HOUR_LAST_FILE = "last_every_hour_file";
static constexpr const char* const QUARTER_HOUR_LAST_FILE = "last_quarter_hour_file";

} // namespace

namespace chime {

SettingsDialog::SettingsDialog(ChimePluginConfig& icfg, SettingsStorage& st, QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
  , cfg(icfg)
  , state(st)
{
  ui->setupUi(this);

  ui->hour_count_box->addItem(tr("Once"), QVariant::fromValue(Repeat::Once));
  ui->hour_count_box->addItem(tr("1-12 times"), QVariant::fromValue(Repeat::Dynamic));

  ui->quarter_count_box->addItem(tr("Once"), QVariant::fromValue(Repeat::Once));
  ui->quarter_count_box->addItem(tr("1-3 times"), QVariant::fromValue(Repeat::Dynamic));

  QSignalBlocker _(this);

  ui->hour_enabled_cb->setChecked(cfg.getEveryHourEnabled());
  setIndexByValue(ui->hour_count_box, cfg.getEveryHourRepeat());
  ui->quarter_enabled_cb->setChecked(cfg.getQuarterHourEnabled());
  setIndexByValue(ui->quarter_count_box, cfg.getQuarterHourRepeat());

  ui->quiet_hours_enabled->setChecked(cfg.getQuietHoursEnabled());
  ui->q_time_start->setTime(cfg.getQuietHoursStartTime());
  ui->q_time_end->setTime(cfg.getQuietHoursEndTime());
}

SettingsDialog::~SettingsDialog()
{
  delete ui;
}

void SettingsDialog::on_hour_enabled_cb_clicked(bool checked)
{
  cfg.setEveryHourEnabled(checked);
}

void SettingsDialog::on_hour_count_box_activated(int index)
{
  Q_ASSERT(index >= 0);
  cfg.setEveryHourRepeat(ui->hour_count_box->itemData(index).value<Repeat>());
}

void SettingsDialog::on_hour_sound_btn_clicked()
{
  StateStorage st(state);
  SoundDialog dlg(this);
  dlg.setSound(cfg.getEveryHourSignal());
  dlg.setVolume(cfg.getEveryHourVolume());
  dlg.setLastFile(st.value(EVERY_HOUR_LAST_FILE, QString()).toString());
  if (dlg.exec() == QDialog::Rejected) return;
  Q_ASSERT(dlg.result() == QDialog::Accepted);
  cfg.setEveryHourSignal(dlg.sound());
  cfg.setEveryHourVolume(dlg.volume());
  if (dlg.sound().scheme() == "file")
    st.setValue(EVERY_HOUR_LAST_FILE, dlg.sound().toLocalFile());
}

void SettingsDialog::on_quarter_enabled_cb_clicked(bool checked)
{
  cfg.setQuarterHourEnabled(checked);
}

void SettingsDialog::on_quarter_count_box_activated(int index)
{
  Q_ASSERT(index >= 0);
  cfg.setQuarterHourRepeat(ui->quarter_count_box->itemData(index).value<Repeat>());
}

void SettingsDialog::on_quarter_sound_btn_clicked()
{
  StateStorage st(state);
  SoundDialog dlg(this);
  dlg.setSound(cfg.getQuarterHourSignal());
  dlg.setVolume(cfg.getQuarterHourVolume());
  dlg.setLastFile(st.value(QUARTER_HOUR_LAST_FILE, QString()).toString());
  if (dlg.exec() == QDialog::Rejected) return;
  Q_ASSERT(dlg.result() == QDialog::Accepted);
  cfg.setQuarterHourSignal(dlg.sound());
  cfg.setQuarterHourVolume(dlg.volume());
  if (dlg.sound().scheme() == "file")
    st.setValue(QUARTER_HOUR_LAST_FILE, dlg.sound().toLocalFile());
}

void SettingsDialog::on_quiet_hours_enabled_clicked(bool checked)
{
  cfg.setQuietHoursEnabled(checked);
}

void SettingsDialog::on_q_time_start_timeChanged(const QTime& time)
{
  cfg.setQuietHoursStartTime(time);
}

void SettingsDialog::on_q_time_end_timeChanged(const QTime& time)
{
  cfg.setQuietHoursEndTime(time);
}

} // namespace chime
