/*
 * SPDX-FileCopyrightText: 2017-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>

#include "scale_factor.hpp"
#include "impl/any_zoom_settings.hpp"

namespace any_zoom {

namespace Ui {
class ZoomDialog;
}

class ZoomDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ZoomDialog(AnyZoomPluginInstanceConfig* icfg,
                      QWidget* parent = nullptr);
  ~ZoomDialog();

signals:
  void zoomChanged(const ScaleFactor& zoom);

private slots:
  void onZoomValueChanged();

private:
  Ui::ZoomDialog* ui;
  AnyZoomPluginInstanceConfig* cfg;
};

} // namespace any_zoom
