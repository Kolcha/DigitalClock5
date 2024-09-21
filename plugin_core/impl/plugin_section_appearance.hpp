/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "config/appearance_base.hpp"

#include <QFont>

enum WidgetPosition { Bottom, Right };
enum ResizePolicy { Autoscale, Fixed };
enum WidgetAppearance { FollowClock, Custom };

namespace plugin_impl {

class SectionAppearance : public AppearanceSectionBase {
public:
  explicit SectionAppearance(ISettingsStorage& st)
      : AppearanceSectionBase("Appearance", st) {}

  CONFIG_OPTION_Q(WidgetPosition, WidgetPosition, Bottom)
  CONFIG_OPTION_Q(ResizePolicy, ResizePolicy, Autoscale)
  CONFIG_OPTION_Q(bool, FollowClock, true)
  CONFIG_OPTION_Q(Qt::Alignment, Alignment, Qt::AlignCenter)
  CONFIG_OPTION_Q(QString, LayoutConfig, QString())
  CONFIG_OPTION_Q(bool, UseClockSkin, false)
  CONFIG_OPTION_Q(QFont, CustomFont, QFont("Comic Sans MS", 24))
};

} // namespace plugin_impl
