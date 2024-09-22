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
class GeometrySettingsWidget;
}

class GeometrySettingsWidget : public QWidget
{
  Q_OBJECT

public:
  GeometrySettingsWidget(WidgetPluginBaseImpl* w, PluginSettingsStorage& s,
                         StateClient& t, QWidget* parent = nullptr);
  ~GeometrySettingsWidget();

private slots:
  void onCharMarginsChanged();
  void onTextMarginsChanged();

  void on_ignore_ax_clicked(bool checked);
  void on_ignore_ay_clicked(bool checked);

  void on_hs_edit_valueChanged(int arg1);
  void on_vs_edit_valueChanged(int arg1);

private:
  Ui::GeometrySettingsWidget* ui;
  WidgetPluginBaseImpl* impl;
  GraphicsWidgetBase* widget;
  plugin_impl::SectionAppearance cfg;
  StateClient st;
};
