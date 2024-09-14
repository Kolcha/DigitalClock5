/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "settings_storage.hpp"

#include <QtCore/qtmetamacros.h>

#include "custom_converters.hpp"
#include "sample_brushes.hpp"

class CLOCK_COMMON_EXPORT AppearanceSectionBase : public SettingsStorageClient {
  Q_GADGET
public:
  enum CustomizationType {
    None,
    SolidColor,
    Gradient,
    Pattern,
  };
  Q_ENUM(CustomizationType)

  using SettingsStorageClient::SettingsStorageClient;

  CONFIG_OPTION_Q(bool, UseSystemForeground, false)
  QBrush getTexture() const;
  CONFIG_OPTION_Q(CustomizationType, TextureType, SolidColor)
  CONFIG_OPTION_Q(QColor, TextureColor, QColor(112, 96, 240))
  CONFIG_OPTION_Q(QGradient, TextureGradient, sample_conical_gradient())
  QPixmap getTexturePattern() const;
  CONFIG_OPTION_Q(QString, TexturePatternFile, QString())
  CONFIG_OPTION_Q(bool, TextureStretch, false)
  CONFIG_OPTION_Q(bool, TexturePerCharacter, true)
  bool shouldUseSystemForeground() const;

  CONFIG_OPTION_Q(bool, UseSystemBackground, false)
  QBrush getBackground() const;
  CONFIG_OPTION_Q(CustomizationType, BackgroundType, None)
  CONFIG_OPTION_Q(QColor, BackgroundColor, QColor(0, 0, 0, 160))
  CONFIG_OPTION_Q(QGradient, BackgroundGradient, sample_linear_gradient())
  QPixmap getBackgroundPattern() const;
  CONFIG_OPTION_Q(QString, BackgroundPatternFile, QString())
  CONFIG_OPTION_Q(bool, BackgroundStretch, false)
  CONFIG_OPTION_Q(bool, BackgroundPerCharacter, false)
  bool shouldUseSystemBackground() const;
};
