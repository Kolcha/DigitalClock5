/*
 * SPDX-FileCopyrightText: 2017-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "zoom_dialog.hpp"
#include "ui_zoom_dialog.h"

namespace any_zoom {

ZoomDialog::ZoomDialog(AnyZoomPluginInstanceConfig* icfg, QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::ZoomDialog)
  , cfg(icfg)
{
  ui->setupUi(this);

  ui->x_zoom_edit->setValue(cfg->getZoomX());
  ui->y_zoom_edit->setValue(cfg->getZoomY());

  connect(ui->x_zoom_slider, &QSlider::valueChanged, this, &ZoomDialog::onZoomValueChanged);
  connect(ui->y_zoom_slider, &QSlider::valueChanged, this, &ZoomDialog::onZoomValueChanged);
}

ZoomDialog::~ZoomDialog()
{
  delete ui;
}

void ZoomDialog::onZoomValueChanged()
{
  cfg->setZoomX(ui->x_zoom_edit->value());
  cfg->setZoomY(ui->y_zoom_edit->value());
  emit zoomChanged({ui->x_zoom_edit->value() / 100., ui->y_zoom_edit->value() / 100.});
}

} // namespace any_zoom
