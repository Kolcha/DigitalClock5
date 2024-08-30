/*
 * SPDX-FileCopyrightText: 2016-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_widget.hpp"
#include "ui_settings_widget.h"

#include "gui/sound_dialog.hpp"

namespace chime {

SettingsWidget::SettingsWidget(SettingsClient& s, StateClient& t,
                               ChimePluginImpl* impl, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SettingsWidget)
    , cfg(s)
    , st(t)
    , impl(impl)
{
  ui->setupUi(this);
  impl->initSettings(s);

  ui->hour_count_box->addItem(tr("Once"), static_cast<int>(Repeat::Once));
  ui->hour_count_box->addItem(tr("1-12 times"), static_cast<int>(Repeat::Dynamic));

  ui->quarter_count_box->addItem(tr("Once"), static_cast<int>(Repeat::Once));
  ui->quarter_count_box->addItem(tr("1-3 times"), static_cast<int>(Repeat::Dynamic));

  ui->hour_enabled_cb->setChecked(cfg.hour().getEnabled());
  switch (cfg.hour().getRepeat()) {
    case Repeat::Once:
      ui->hour_count_box->setCurrentIndex(0);
      break;

    case Repeat::Dynamic:
      ui->hour_count_box->setCurrentIndex(1);
      break;
  }

  ui->quarter_enabled_cb->setChecked(cfg.quarter().getEnabled());
  switch (cfg.quarter().getRepeat()) {
    case Repeat::Once:
      ui->quarter_count_box->setCurrentIndex(0);
      break;

    case Repeat::Dynamic:
      ui->quarter_count_box->setCurrentIndex(1);
      break;
  }

  ui->q_time_start->setTime(cfg.quietTime().getStartTime());
  ui->q_time_end->setTime(cfg.quietTime().getEndTime());

  ui->quiet_hours_enabled->setChecked(cfg.quietTime().getEnabled());
}

SettingsWidget::~SettingsWidget()
{
  delete ui;
}

void SettingsWidget::on_hour_enabled_cb_clicked(bool checked)
{
  impl->every_hour_enabled = checked;
  cfg.hour().setEnabled(checked);
}

void SettingsWidget::on_hour_count_box_activated(int index)
{
  Q_ASSERT(index >= 0);
  impl->every_hour_repeat = ui->hour_count_box->itemData(index).value<Repeat>();
  cfg.hour().setRepeat(impl->every_hour_repeat);
}

void SettingsWidget::on_hour_sound_btn_clicked()
{
  SoundDialog dlg(this);
  dlg.setSound(impl->every_hour_signal);
  dlg.setVolume(impl->every_hour_volume);
  dlg.setLastFile(st.value("every_hour/last_file", "").toString());

  if (dlg.exec() == QDialog::Rejected) return;

  Q_ASSERT(dlg.result() == QDialog::Accepted);
  impl->every_hour_signal = dlg.sound();
  impl->every_hour_volume = dlg.volume();

  if (dlg.sound().scheme() == "file")
    st.setValue("every_hour/last_file", dlg.sound().toLocalFile());

  cfg.hour().setSignal(dlg.sound());
  cfg.hour().setVolume(dlg.volume());
}

void SettingsWidget::on_quarter_enabled_cb_clicked(bool checked)
{
  impl->quarter_hour_enabled = checked;
  cfg.quarter().setEnabled(checked);
}

void SettingsWidget::on_quarter_count_box_activated(int index)
{
  Q_ASSERT(index >= 0);
  impl->quarter_hour_repeat = ui->quarter_count_box->itemData(index).value<Repeat>();
  cfg.quarter().setRepeat(impl->quarter_hour_repeat);
}

void SettingsWidget::on_quarter_sound_btn_clicked()
{
  SoundDialog dlg(this);
  dlg.setSound(impl->quarter_hour_signal);
  dlg.setVolume(impl->quarter_hour_volume);
  dlg.setLastFile(st.value("quarter_hour/last_file", "").toString());

  if (dlg.exec() == QDialog::Rejected) return;

  Q_ASSERT(dlg.result() == QDialog::Accepted);
  impl->quarter_hour_signal = dlg.sound();
  impl->quarter_hour_volume = dlg.volume();

  if (dlg.sound().scheme() == "file")
    st.setValue("quarter_hour/last_file", dlg.sound().toLocalFile());

  cfg.quarter().setSignal(dlg.sound());
  cfg.quarter().setVolume(dlg.volume());
}

void SettingsWidget::on_quiet_hours_enabled_clicked(bool checked)
{
  impl->quiet_hours_enabled = checked;
  cfg.quietTime().setEnabled(checked);
}

void SettingsWidget::on_q_time_start_timeChanged(const QTime& time)
{
  impl->quiet_hours_start = time;
  cfg.quietTime().setStartTime(time);
}

void SettingsWidget::on_q_time_end_timeChanged(const QTime& time)
{
  impl->quiet_hours_end = time;
  cfg.quietTime().setEndTime(time);
}

} // namespace chime
