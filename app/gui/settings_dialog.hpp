/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>

class Application;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SettingsDialog(Application* app, int idx, QWidget* parent = nullptr);
  ~SettingsDialog();

public slots:
  void accept() override;
  void reject() override;

private slots:
  void on_windows_box_currentIndexChanged(int index);

  // app global
  void on_enable_autostart_clicked(bool checked);
  void on_enable_stay_on_top_clicked(bool checked);
  void on_enable_transp_for_input_clicked(bool checked);

  void on_enable_multiwindow_clicked(bool checked);
  void on_wnd_count_edit_valueChanged(int arg1);
  void on_use_same_appearance_clicked(bool checked);

  void on_transparent_on_hover_clicked(bool checked);
  void on_hide_on_mouse_hover_clicked(bool checked);

  void on_enable_autoupdate_clicked(bool checked);
  void on_update_period_edit_activated(int index);
  void on_check_for_beta_clicked(bool checked);

  void on_enable_debug_options_clicked(bool checked);

  // general
  void on_smaller_seconds_clicked(bool checked);
  void on_seconds_scale_factor_edit_valueChanged(int arg1);
  void on_use_custom_format_toggled(bool checked);
  void on_use_custom_format_clicked(bool checked);
  void on_format_edit_textChanged(const QString& arg1);
  void on_format_edit_textEdited(const QString& arg1);
  void on_format_help_btn_clicked();
  void on_format_apply_btn_clicked();
  void on_layout_cfg_edit_textEdited(const QString& arg1);
  void on_layout_cfg_help_btn_clicked();
  void on_use_custom_seps_clicked(bool checked);
  void on_custom_seps_edit_textEdited(const QString& arg1);

  void updateTimeFormat();

  void on_use_time_zone_clicked(bool checked);
  void on_time_zone_edit_activated(int index);

  // appearance
  void on_font_rbtn_clicked();
  void on_select_font_btn_clicked();

  void on_skin_rbtn_clicked();
  void on_skin_cbox_activated(int index);

  void on_is_separator_flashes_clicked(bool checked);

  void on_scaling_x_edit_valueChanged(int arg1);
  void on_scaling_y_edit_valueChanged(int arg1);
  void on_scaling_same_btn_clicked(bool checked);

  void on_opacity_edit_valueChanged(int arg1);

  void on_use_colorization_clicked(bool checked);
  void on_select_colorization_color_clicked();
  void on_colorization_strength_edit_valueChanged(int arg1);

  void on_texture_group_clicked(bool checked);
  void on_tx_solid_color_rbtn_clicked();
  void on_tx_select_color_btn_clicked();
  void on_tx_gradient_rbtn_clicked();
  void on_tx_select_gradient_btn_clicked();
  void on_tx_pattern_rbtn_clicked();
  void on_tx_select_pattern_btn_clicked();
  void on_tx_pattern_stretch_clicked(bool checked);
  void on_tx_per_element_cb_clicked(bool checked);

  void on_background_group_clicked(bool checked);
  void on_bg_solid_color_rbtn_clicked();
  void on_bg_select_color_btn_clicked();
  void on_bg_gradient_rbtn_clicked();
  void on_bg_select_gradient_btn_clicked();
  void on_bg_pattern_rbtn_clicked();
  void on_bg_select_pattern_btn_clicked();
  void on_bg_pattern_stretch_clicked(bool checked);
  void on_bg_per_element_cb_clicked(bool checked);

  // plugins
  void onPluginStateChanged(const QString& id, bool enabled);

private:
  void initAppGlobalTab();
  void initGeneralTab(int idx);
  void initAppearanceTab(int idx);
  void initPluginsTab();

  void applyTimeZoneSettings();
  void applyColorization();
  void updateColorization();
  template<typename Method, typename... Args>
  void applyWindowOption(Method method, Args&&... args);
  template<typename Method, typename... Args>
  void applyClockOption(Method method, Args&&... args);

  template<typename Method, typename... Args>
  void notifyOptionChanged(Method method, Args&&... args);

private:
  Ui::SettingsDialog* ui;
  Application* app;
  int _curr_idx;
  QString _last_path;
};