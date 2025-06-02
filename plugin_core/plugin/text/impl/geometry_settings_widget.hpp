/*
 * SPDX-FileCopyrightText: 2024-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "plugin/text/plugin_config.hpp"

namespace plugin::text::impl {

namespace Ui {
class GeometrySettingsWidget;
}

class GeometrySettingsWidget : public QWidget
{
  Q_OBJECT

public:
  GeometrySettingsWidget(QWidget* parent = nullptr);
  ~GeometrySettingsWidget();

  void initControls(PluginInstanceConfig* icfg);

signals:
  void pluginOptionChanged(wpo::InstanceOptions opt, const QVariant& val);
  void sharedOptionChanged(opt::InstanceOptions opt, const QVariant& val);

private slots:
  void onCharMarginsChanged();
  void onTextMarginsChanged();

  void on_true_per_char_clicked(bool checked);
  void on_respect_line_spacing_clicked(bool checked);

  void on_hs_edit_valueChanged(int arg1);
  void on_vs_edit_valueChanged(int arg1);

private:
  Ui::GeometrySettingsWidget* ui;
  PluginInstanceConfig* cfg = nullptr;
};

} // namespace plugin::text::impl
