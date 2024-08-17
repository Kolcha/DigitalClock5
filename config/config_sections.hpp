#pragma once

#include <QFont>

#include "settings_storage.hpp"

// even it may look as unnecessary and unused header,
// it is requeired due to some templates behind macro
#include "custom_converters.hpp"

class SectionAppGlobal : public SettingsStorageClient {
public:
  using SettingsStorageClient::SettingsStorageClient;

  CONFIG_OPTION_Q(int, NumInstances, 1)
  CONFIG_OPTION_Q(bool, AppearancePerInstance, false)

  CONFIG_OPTION_Q(bool, StayOnTop, true)
  CONFIG_OPTION_Q(bool, TransparentForMouse, false)

  CONFIG_OPTION_Q(bool, CheckForUpdates, true)
  CONFIG_OPTION_Q(bool, CheckForBetaVersion, true)
  CONFIG_OPTION_Q(int, UpdatePeriodDays, 7)
};


class SectionLimits : public SettingsStorageClient {
public:
  using SettingsStorageClient::SettingsStorageClient;

  CONFIG_OPTION_Q(int, MinScaling, 10)
  CONFIG_OPTION_Q(int, MaxScaling, 250)

  CONFIG_OPTION_Q(int, BaseSize, 120)

  CONFIG_OPTION_Q(int, MinSpacing, -8)
  CONFIG_OPTION_Q(int, MaxSpacing, 16)
};

// per-window settings

class SectionAppearance : public SettingsStorageClient {
public:
  using SettingsStorageClient::SettingsStorageClient;

  CONFIG_OPTION_Q(int, ScalingH, 100)
  CONFIG_OPTION_Q(int, ScalingV, 100)

  CONFIG_OPTION_Q(int, SpacingH, 4)
  CONFIG_OPTION_Q(int, SpacingV, 4)

  CONFIG_OPTION_Q(int, Opacity, 80)

  CONFIG_OPTION_Q(bool, ApplyColorization, false)
  CONFIG_OPTION_Q(qreal, ColorizationStrength, 0.8)
  CONFIG_OPTION_Q(QColor, ColorizationColor, QColor(0, 85, 255))

  CONFIG_OPTION_Q(bool, UseFontInsteadOfSkin, true)
  CONFIG_OPTION_Q(QFont, Font, QFont("Comic Sans MS", 96))
  CONFIG_OPTION_Q(QString, Skin, QString("electronic"))

  CONFIG_OPTION_Q(bool, SeparatorFlashes, true)
  CONFIG_OPTION_Q(bool, UseCustomSeparators, false)
  CONFIG_OPTION_Q(QString, CustomSeparators, QString())

  CONFIG_OPTION_Q(QString, TimeFormat, QString("HH:mm"))
  CONFIG_OPTION_Q(int, SecondsScaleFactor, 100)

  CONFIG_OPTION_Q(QBrush, Texture, QColor(112, 96, 240))
  CONFIG_OPTION_Q(bool, TextureStretch, false)
  CONFIG_OPTION_Q(bool, TexturePerCharacter, true)

  CONFIG_OPTION_Q(QBrush, Background, Qt::NoBrush)
  CONFIG_OPTION_Q(bool, BackgroundStretch, false)
  CONFIG_OPTION_Q(bool, BackgroundPerCharacter, false)
};

class SectionGeneric : public SettingsStorageClient {
public:
  using SettingsStorageClient::SettingsStorageClient;

  CONFIG_OPTION_Q(bool, ShowLocalTime, true)
  CONFIG_OPTION_Q(QTimeZone, TimeZone, QTimeZone::systemTimeZone())

  CONFIG_OPTION_Q(int, AnchorPoint, 0)

  CONFIG_OPTION_Q(bool, SnapToEdge, true)
  CONFIG_OPTION_Q(int, SnapThreshold, 15)
};
