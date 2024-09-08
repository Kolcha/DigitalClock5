/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "impl/any_zoom_plugin_impl.hpp"
#include "impl/any_zoom_settings.hpp"

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
  Q_OBJECT

public:
  explicit SettingsWidget(PluginSettingsStorage& s, StateClient& t,
                          AnyZoomPluginImpl* impl, QWidget* parent = nullptr);
  ~SettingsWidget();

private slots:
  void on_x_zoom_slider_valueChanged(int value);
  void on_y_zoom_slider_valueChanged(int value);

private:
  Ui::SettingsWidget* ui;
  plugin_impl::AnyZoomPluginSettings cfg;
  AnyZoomPluginImpl* impl;
};
