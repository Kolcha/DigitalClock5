/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin_config.hpp"

#include <QFont>

enum WidgetPosition { Bottom, Right };
enum ResizePolicy { Autoscale, Fixed };
enum WidgetAppearance { FollowClock, Custom };

namespace plugin_impl {

template<class C>
class SectionAppearance : public PluginConfig<C> {
public:
  explicit SectionAppearance(C& c) : PluginConfig<C>(c) {}

  PLG_CONFIG_SEC_OPTION_Q(Appearance, WidgetPosition, WidgetPosition, Bottom)
  PLG_CONFIG_SEC_OPTION_Q(Appearance, ResizePolicy, ResizePolicy, Autoscale)
  PLG_CONFIG_SEC_OPTION_Q(Appearance, FollowClock, bool, true)
  PLG_CONFIG_SEC_OPTION_Q(Appearance, Alignment, Qt::Alignment, Qt::AlignCenter)
  PLG_CONFIG_SEC_OPTION_Q(Appearamce, LayoutConfig, QString, QString())
  PLG_CONFIG_SEC_OPTION_Q(Appearance, UseClockSkin, bool, false)
  PLG_CONFIG_SEC_OPTION_Q(Appearance, CustomFont, QFont, QFont("Comic Sans MS", 72))
  PLG_CONFIG_SEC_OPTION_Q(Appearance, Texture, QBrush, QColor(112, 96, 240))
  PLG_CONFIG_SEC_OPTION_Q(Appearance, TextureStretch, bool, false)
  PLG_CONFIG_SEC_OPTION_Q(Appearance, TexturePerChar, bool, true)
  PLG_CONFIG_SEC_OPTION_Q(Appearance, Background, QBrush, QBrush(Qt::NoBrush))
  PLG_CONFIG_SEC_OPTION_Q(Appearance, BackgroundStretch, bool, false)
  PLG_CONFIG_SEC_OPTION_Q(Appearance, BackgroundPerChar, bool, false)
};

} // namespace plugin_impl
