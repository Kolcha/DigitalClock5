/*
 * SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>

#include "impl/scheduled_color_settings.hpp"

class SettingsStorage;

namespace scheduled_color {

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SettingsDialog(QWidget* parent = nullptr);
  ~SettingsDialog();

  void initControls(SettingsStorage* settings);

signals:
  void timePointsChanged();

private slots:
  void on_add_btn_clicked();
  void on_remove_btn_clicked();
  void on_time_points_list_currentRowChanged(int row);
  void on_time_edit_timeChanged(const QTime& time);
  void on_color_btn_clicked();
  void on_mode_combo_currentIndexChanged(int index);
  void on_duration_spin_valueChanged(int value);
  void on_until_time_edit_timeChanged(const QTime& time);

private:
  void loadTimePoints();
  void saveTimePoints();
  void updateListItem(int row);
  void updateDetailPanel(int row);
  void updateModeFieldsVisibility();
  QString getColorName(const QColor& color) const;

private:
  Ui::SettingsDialog* ui;
  SettingsStorage* _settings = nullptr;
  std::vector<TimeColorPoint> _time_points;
  int _current_row = -1;
  bool _loading = false;
};

} // namespace scheduled_color
