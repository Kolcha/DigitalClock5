/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_widget.hpp"
#include "ui_settings_widget.h"

SettingsWidget::SettingsWidget(SettingsClient& s, StateClient& t,
                               AnyZoomPluginImpl* impl, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SettingsWidget)
    , cfg(s)
    , impl(impl)
{
  ui->setupUi(this);

  ui->x_zoom_edit->setValue(cfg.getZoomX());
  ui->y_zoom_edit->setValue(cfg.getZoomY());
}

SettingsWidget::~SettingsWidget()
{
  delete ui;
}

void SettingsWidget::on_x_zoom_slider_valueChanged(int value)
{
  impl->zoom_x = value;
  cfg.setZoomX(value);
  emit impl->optionChanged(cs::ScalingH, value);
}

void SettingsWidget::on_y_zoom_slider_valueChanged(int value)
{
  impl->zoom_y = value;
  cfg.setZoomY(value);
  emit impl->optionChanged(cs::ScalingV, value);
}
