/*
 * SPDX-FileCopyrightText: 2024-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "plugin/text/plugin_config.hpp"

class QComboBox;

namespace plugin::text::impl {

namespace Ui {
class AppearanceSettingsWidget;
}

class AppearanceSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  AppearanceSettingsWidget(QWidget* parent = nullptr);
  ~AppearanceSettingsWidget();

  void initControls(PluginInstanceConfig* icfg);

signals:
  void pluginOptionChanged(wpo::InstanceOptions opt, const QVariant& val);
  void sharedOptionChanged(opt::InstanceOptions opt, const QVariant& val);

private slots:
  void on_pos_bottom_clicked();
  void on_pos_right_clicked();

  void on_resize_auto_clicked();
  void on_resize_fixed_clicked();

  void on_appear_follow_clicked();
  void on_appear_custom_clicked();

  void on_align_h_box_activated(int index);
  void on_align_v_box_activated(int index);
  void on_layout_cfg_edit_textEdited(const QString& arg1);
  void on_layout_cfg_help_btn_clicked();

  void on_font_rbtn_clicked();
  void on_font_select_clicked();
  void on_skin_rbtn_clicked();

  void on_tx_group_clicked(bool checked);
  void on_tx_options_box_activated(int index);
  void on_tx_options_box_currentIndexChanged(int index);
  void tx_use_system_color(bool use);
  void tx_select_color();
  void tx_select_gradient();
  void tx_select_pattern();
  void tx_pattern_stretch(bool checked);
  void on_tx_per_char_clicked(bool checked);

  void on_bg_group_clicked(bool checked);
  void on_bg_options_box_activated(int index);
  void on_bg_options_box_currentIndexChanged(int index);
  void bg_use_system_color(bool use);
  void bg_select_color();
  void bg_select_gradient();
  void bg_select_pattern();
  void bg_pattern_stretch(bool checked);
  void on_bg_per_char_clicked(bool checked);

private:
  void fillAlignmentList();
  void fillTextureTypes(QComboBox* box);

private:
  Ui::AppearanceSettingsWidget* ui;
  PluginInstanceConfig* cfg = nullptr;
};

} // namespace plugin::text::impl
