/*
 * SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"

#include "config/settings.hpp"

#include <QColorDialog>
#include <QMessageBox>
#include <QSignalBlocker>

namespace scheduled_color {

SettingsDialog::SettingsDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
{
  ui->setupUi(this);

  ui->mode_combo->setItemData(0, TimeColorPoint::FixedDuration);
  ui->mode_combo->setItemData(1, TimeColorPoint::UntilTime);
  ui->mode_combo->setItemData(2, TimeColorPoint::UntilNextPoint);

  ui->remove_btn->setEnabled(false);
  ui->details_group->setEnabled(false);
}

SettingsDialog::~SettingsDialog()
{
  delete ui;
}

void SettingsDialog::initControls(SettingsStorage* settings)
{
  _settings = settings;
  loadTimePoints();
}

void SettingsDialog::loadTimePoints()
{
  if (!_settings)
    return;

  _loading = true;

  _time_points.clear();
  ui->time_points_list->clear();

  int count = _settings->value("time_points_count", 0).toInt();

  for (int i = 0; i < count; ++i) {
    QString prefix = QString("point_%1_").arg(i);

    TimeColorPoint point;
    point.time = QTime::fromString(_settings->value(prefix + "time", "00:00").toString(), "HH:mm");
    point.color = QColor(_settings->value(prefix + "color", "#FFFFFF").toString());
    point.mode = static_cast<TimeColorPoint::DurationMode>(_settings->value(prefix + "mode", 2).toInt());
    point.until_time = QTime::fromString(_settings->value(prefix + "until_time", "00:00").toString(), "HH:mm");
    point.duration_seconds = _settings->value(prefix + "duration", 3600).toInt();

    if (!point.time.isValid()) {
      point.time = QTime(0, 0);
    }
    if (!point.color.isValid()) {
      point.color = Qt::white;
    }
    if (!point.until_time.isValid()) {
      point.until_time = QTime(0, 0);
    }

    _time_points.push_back(point);

    QString displayText = QString("%1 - %2")
                          .arg(point.time.toString("HH:mm"))
                          .arg(getColorName(point.color));
    auto* item = new QListWidgetItem(displayText);
    item->setBackground(point.color);

    QColor textColor = (point.color.lightness() > 128) ? Qt::black : Qt::white;
    item->setForeground(textColor);

    ui->time_points_list->addItem(item);
  }

  _loading = false;

  if (!_time_points.empty()) {
    ui->time_points_list->setCurrentRow(0);
  }
}

void SettingsDialog::saveTimePoints()
{
  if (!_settings || _loading)
    return;

  _settings->setValue("time_points_count", static_cast<int>(_time_points.size()));

  for (size_t i = 0; i < _time_points.size(); ++i) {
    QString prefix = QString("point_%1_").arg(i);
    const auto& point = _time_points[i];

    _settings->setValue(prefix + "time", point.time.toString("HH:mm"));
    _settings->setValue(prefix + "color", point.color.name());
    _settings->setValue(prefix + "mode", static_cast<int>(point.mode));
    _settings->setValue(prefix + "until_time", point.until_time.toString("HH:mm"));
    _settings->setValue(prefix + "duration", point.duration_seconds);
  }

  emit timePointsChanged();
}

void SettingsDialog::on_add_btn_clicked()
{
  TimeColorPoint point;
  point.time = QTime::currentTime();
  point.color = QColor(Qt::white);
  point.mode = TimeColorPoint::UntilNextPoint;
  point.until_time = QTime(0, 0);
  point.duration_seconds = 3600;

  _time_points.push_back(point);

  QString displayText = QString("%1 - %2")
                        .arg(point.time.toString("HH:mm"))
                        .arg(getColorName(point.color));
  auto* item = new QListWidgetItem(displayText);
  item->setBackground(point.color);

  QColor textColor = (point.color.lightness() > 128) ? Qt::black : Qt::white;
  item->setForeground(textColor);

  ui->time_points_list->addItem(item);
  ui->time_points_list->setCurrentRow(ui->time_points_list->count() - 1);

  saveTimePoints();
}

void SettingsDialog::on_remove_btn_clicked()
{
  if (_current_row < 0 || _current_row >= static_cast<int>(_time_points.size()))
    return;

  auto reply = QMessageBox::question(
                 this,
                 tr("Confirm Removal"),
                 tr("Are you sure you want to remove this time point?"),
                 QMessageBox::Yes | QMessageBox::No
               );

  if (reply != QMessageBox::Yes)
    return;

  _time_points.erase(_time_points.begin() + _current_row);
  delete ui->time_points_list->takeItem(_current_row);

  saveTimePoints();
}

void SettingsDialog::on_time_points_list_currentRowChanged(int row)
{
  _current_row = row;

  bool hasSelection = (row >= 0 && row < static_cast<int>(_time_points.size()));
  ui->remove_btn->setEnabled(hasSelection);
  ui->details_group->setEnabled(hasSelection);

  if (hasSelection) {
    updateDetailPanel(row);
  }
}

void SettingsDialog::updateDetailPanel(int row)
{
  if (row < 0 || row >= static_cast<int>(_time_points.size()))
    return;

  QSignalBlocker blocker(this);

  const auto& point = _time_points[row];

  ui->time_edit->setTime(point.time);

  QString colorText = QString("Background: %1").arg(point.color.name());
  ui->color_preview->setStyleSheet(colorText);

  for (int i = 0; i < ui->mode_combo->count(); ++i) {
    if (ui->mode_combo->itemData(i).toInt() == point.mode) {
      ui->mode_combo->setCurrentIndex(i);
      break;
    }
  }

  ui->duration_spin->setValue(point.duration_seconds);
  ui->until_time_edit->setTime(point.until_time);

  updateModeFieldsVisibility();
}

void SettingsDialog::updateModeFieldsVisibility()
{
  if (_current_row < 0 || _current_row >= static_cast<int>(_time_points.size())) {
    ui->duration_spin->setVisible(false);
    ui->duration_label->setVisible(false);
    ui->until_time_edit->setVisible(false);
    ui->until_time_label->setVisible(false);
    return;
  }

  auto mode = _time_points[_current_row].mode;

  bool showDuration = (mode == TimeColorPoint::FixedDuration);
  bool showUntilTime = (mode == TimeColorPoint::UntilTime);

  ui->duration_spin->setVisible(showDuration);
  ui->duration_label->setVisible(showDuration);
  ui->until_time_edit->setVisible(showUntilTime);
  ui->until_time_label->setVisible(showUntilTime);
}

void SettingsDialog::updateListItem(int row)
{
  if (row < 0 || row >= static_cast<int>(_time_points.size()))
    return;

  const auto& point = _time_points[row];

  QString displayText = QString("%1 - %2")
                        .arg(point.time.toString("HH:mm"))
                        .arg(getColorName(point.color));

  auto* item = ui->time_points_list->item(row);
  if (item) {
    item->setText(displayText);
    item->setBackground(point.color);

    QColor textColor = (point.color.lightness() > 128) ? Qt::black : Qt::white;
    item->setForeground(textColor);
  }
}

void SettingsDialog::on_time_edit_timeChanged(const QTime& time)
{
  if (_loading || _current_row < 0 || _current_row >= static_cast<int>(_time_points.size()))
    return;

  _time_points[_current_row].time = time;
  updateListItem(_current_row);
  saveTimePoints();
}

void SettingsDialog::on_color_btn_clicked()
{
  if (_current_row < 0 || _current_row >= static_cast<int>(_time_points.size()))
    return;

  QColor current = _time_points[_current_row].color;
  QColor color = QColorDialog::getColor(current, this, tr("Select Color"));

  if (color.isValid()) {
    _time_points[_current_row].color = color;
    updateListItem(_current_row);
    updateDetailPanel(_current_row);
    saveTimePoints();
  }
}

void SettingsDialog::on_mode_combo_currentIndexChanged(int index)
{
  if (_loading || _current_row < 0 || _current_row >= static_cast<int>(_time_points.size()))
    return;

  auto mode = static_cast<TimeColorPoint::DurationMode>(ui->mode_combo->itemData(index).toInt());
  _time_points[_current_row].mode = mode;

  updateModeFieldsVisibility();
  saveTimePoints();
}

void SettingsDialog::on_duration_spin_valueChanged(int value)
{
  if (_loading || _current_row < 0 || _current_row >= static_cast<int>(_time_points.size()))
    return;

  _time_points[_current_row].duration_seconds = value;
  saveTimePoints();
}

void SettingsDialog::on_until_time_edit_timeChanged(const QTime& time)
{
  if (_loading || _current_row < 0 || _current_row >= static_cast<int>(_time_points.size()))
    return;

  _time_points[_current_row].until_time = time;
  saveTimePoints();
}

QString SettingsDialog::getColorName(const QColor& color) const
{
  return color.name().toUpper();
}

} // namespace scheduled_color
