/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "state.hpp"

#include "config/custom_converters.hpp"
#include "config/settings_storage.hpp"

class CLOCK_COMMON_EXPORT CommonAppearanceState : public StateImpl {
public:
  using StateImpl::StateImpl;

  CONFIG_OPTION_Q(QColor, TextureColor, QColor(112, 96, 240))
  CONFIG_OPTION_Q(QGradient, TextureGradient, sample_conical_gradient())
  CONFIG_OPTION_Q(QPixmap, TexturePattern, sample_pattern())
  CONFIG_OPTION_Q(QColor, BackgroundColor, QColor(0, 0, 0, 160))
  CONFIG_OPTION_Q(QGradient, BackgroundGradient, sample_linear_gradient())
  CONFIG_OPTION_Q(QPixmap, BackgroundPattern, sample_pattern())

private:
  static QGradient sample_conical_gradient();
  static QGradient sample_linear_gradient();
  static QPixmap sample_pattern();
};
