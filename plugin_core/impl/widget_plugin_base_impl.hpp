/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QGridLayout>

#include "skin/font_skin.hpp"
#include "skin/graphics_widgets.hpp"

#include "clock_plugin.hpp"

#include "plugin_section_appearance.hpp"

class WidgetPluginBaseImpl {
public:
  // shared data not owned by the plugin
  std::shared_ptr<Skin> skin;
  SkinAccessExtension::AppearanceSettings skin_cfg;

  QWidget* wnd = nullptr;
  QGridLayout* layout = nullptr;

  // internal data, owned by the plugin
  std::shared_ptr<GraphicsWidgetBase> widget;

  // plugin settings
  WidgetPosition position = Bottom;
  ResizePolicy resize_policy = Autoscale;
  WidgetAppearance appearance = FollowClock;
  Qt::Alignment content_alignment = Qt::AlignCenter;
  QString layout_cfg;
  bool use_clock_skin = false;

  std::shared_ptr<FontSkin> custom_skin;

  QBrush tx;
  bool tx_stretch = false;
  bool tx_per_char = true;

  QBrush bg;
  bool bg_stretch = false;
  bool bg_per_char = false;

  void applyAppearanceSettings();
  void repositionWidget();

  void initSettings(PluginSettingsStorage& st);
};
