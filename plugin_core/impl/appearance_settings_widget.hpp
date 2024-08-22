/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "config/settings_storage.hpp"
#include "plugin_section_appearance.hpp"
#include "widget_plugin_base_impl.hpp"

namespace Ui {
class AppearanceSettingsWidget;
}

class AppearanceSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  AppearanceSettingsWidget(WidgetPluginBaseImpl* w, SettingsClient& s,
                           StateClient& t, QWidget* parent = nullptr);
  ~AppearanceSettingsWidget();

private slots:
  void on_pos_bottom_clicked();
  void on_pos_right_clicked();

  void on_resize_auto_clicked();
  void on_resize_fixed_clicked();

  void on_appear_follow_clicked();
  void on_appear_custom_clicked();

  void on_align_h_box_activated(int index);
  void on_align_v_box_activated(int index);

  void on_font_select_clicked();

  void on_tx_group_clicked(bool checked);
  void on_tx_color_clicked();
  void on_tx_color_select_clicked();
  void on_tx_gradient_clicked();
  void on_tx_gradient_select_clicked();
  void on_tx_pattern_clicked();
  void on_tx_pattern_select_clicked();
  void on_tx_stretch_clicked(bool checked);
  void on_tx_per_char_clicked(bool checked);

  void on_bg_group_clicked(bool checked);
  void on_bg_color_clicked();
  void on_bg_color_select_clicked();
  void on_bg_gradient_clicked();
  void on_bg_gradient_select_clicked();
  void on_bg_pattern_clicked();
  void on_bg_pattern_select_clicked();
  void on_bg_stretch_clicked(bool checked);
  void on_bg_per_char_clicked(bool checked);

private:
  Ui::AppearanceSettingsWidget* ui;
  WidgetPluginBaseImpl* impl;
  GraphicsWidgetBase* widget;
  plugin_impl::SectionAppearance<SettingsClient> cfg;
  StateClient& st;
  QString _last_path;
};
