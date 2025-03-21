/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>

#include "common/instance_options.hpp"
#include "config/global_options.hpp"

class ClockApplication;
class QComboBox;

namespace Ui {
class SettingsDialog;
}

class PluginHandle;

class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SettingsDialog(ClockApplication* app, size_t idx, QWidget* parent = nullptr);
  ~SettingsDialog();

public slots:
  void accept() override;
  void reject() override;

signals:
  void globalOptionChanged(opt::GlobalOptions opt, const QVariant& val);
  void instanceOptionChanged(size_t i, opt::InstanceOptions opt, const QVariant& val);

private slots:
  void on_import_btn_clicked();
  void on_export_btn_clicked();

  void on_windows_box_currentIndexChanged(int index);

  // app global
  void on_lang_tr_btn_clicked();
  void on_lang_list_activated(int index);

  void on_enable_autostart_clicked(bool checked);
  void on_enable_stay_on_top_clicked(bool checked);
  void on_fullscreen_detect_clicked(bool checked);
  void on_enable_transp_for_input_clicked(bool checked);
  void on_should_change_cursor_clicked(bool checked);

  void on_snap_to_edges_clicked(bool checked);
  void on_snap_threshold_valueChanged(int arg1);
  void on_keep_always_visible_clicked(bool checked);

  void on_enable_multiwindow_clicked(bool checked);
  void on_wnd_count_edit_valueChanged(int arg1);
  void on_use_same_appearance_clicked(bool checked);

  void on_transparent_on_hover_clicked(bool checked);
  void on_hide_on_mouse_hover_clicked(bool checked);

  void on_tray_icon_s_action_enabled_clicked(bool checked);
  void on_tray_icon_s_action_box_activated(int index);

  void on_tray_icon_d_action_enabled_clicked(bool checked);
  void on_tray_icon_d_action_box_activated(int index);

  void on_enable_autoupdate_clicked(bool checked);
  void on_update_period_edit_activated(int index);
  void on_check_for_beta_clicked(bool checked);

  // format
  void on_smaller_seconds_clicked(bool checked);
  void on_seconds_scale_factor_edit_valueChanged(int arg1);
  void on_use_custom_format_toggled(bool checked);
  void on_use_custom_format_clicked(bool checked);
  void on_format_edit_currentTextChanged(const QString& arg1);
  void on_format_edit_editTextChanged(const QString& arg1);
  void on_format_help_btn_clicked();
  void on_format_apply_btn_clicked();
  void on_layout_cfg_edit_textEdited(const QString& arg1);
  void on_layout_cfg_help_btn_clicked();
  void on_use_custom_seps_clicked(bool checked);
  void on_custom_seps_edit_textEdited(const QString& arg1);
  void on_custom_seps_help_btn_clicked();

  void updateTimeFormat();

  void on_use_time_zone_clicked(bool checked);
  void on_time_zone_edit_activated(int index);
  void on_multi_timezone_help_btn_clicked();

  // appearance
  void on_font_rbtn_clicked();
  void on_select_font_btn_clicked();

  void on_skin_rbtn_clicked();
  void on_skin_cbox_activated(int index);

  void on_is_separator_flashes_clicked(bool checked);

  void on_scaling_edit_valueChanged(int arg1);
  void on_opacity_edit_valueChanged(int arg1);

  void on_colorize_group_clicked(bool checked);
  void on_select_colorization_color_clicked();
  void on_colorization_strength_edit_valueChanged(int arg1);

  void on_texture_group_clicked(bool checked);
  void on_tx_options_box_activated(int index);
  void on_tx_options_box_currentIndexChanged(int index);
  void tx_use_system_color(bool use);
  void tx_select_color();
  void tx_select_gradient();
  void tx_select_pattern();
  void tx_pattern_stretch(bool checked);
  void on_tx_per_element_cb_clicked(bool checked);

  void on_background_group_clicked(bool checked);
  void on_bg_options_box_activated(int index);
  void on_bg_options_box_currentIndexChanged(int index);
  void bg_use_system_color(bool use);
  void bg_select_color();
  void bg_select_gradient();
  void bg_select_pattern();
  void bg_pattern_stretch(bool checked);
  void on_bg_per_element_cb_clicked(bool checked);

  void on_anchor_left_clicked();
  void on_anchor_center_clicked();
  void on_anchor_right_clicked();

  void on_hs_edit_valueChanged(int arg1);
  void on_vs_edit_valueChanged(int arg1);

  // misc
  void onCharMarginsChanged();
  void onTextMarginsChanged();
  void on_true_per_char_clicked(bool checked);
  void on_respect_line_spacing_clicked(bool checked);
  void on_layout_spacing_edit_valueChanged(int arg1);
  void on_hide_clock_widget_clicked(bool checked);

private:
  void initAppGlobalTab();
  void initGeneralTab(size_t idx);
  void initAppearanceTab(size_t idx);
  void initMiscTab(size_t idx);
  void initPluginsTab();

  void initControlsState();

  void onPluginStateChanged(PluginHandle ph, bool enabled);
  void showPluginInfoDialog(const PluginHandle& ph);

  void initWindowsList();
  void fillLanguagesList();
  void fillUpdatePeriodsList();
  void fillTrayIconActions(QComboBox* box);
  void fillTimeZonesList();
  void fillSkinsList();
  void fillTextureTypes(QComboBox* box);

private:
  Ui::SettingsDialog* ui;
  ClockApplication* app;
  size_t _curr_idx;
  bool _settings_imported = false;
};
