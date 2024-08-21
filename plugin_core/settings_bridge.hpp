/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QObject>

#include "plugin_core_global.hpp"
#include "shared_config_keys.hpp"

class PLUGIN_CORE_EXPORT SettingsChangeTransmitter : public QObject
{
  Q_OBJECT

public:
  explicit SettingsChangeTransmitter(QObject* parent = nullptr);

public slots:
  void setOpacity(int o);

  void setScalingH(int s);
  void setScalingV(int s);

  void setSpacingH(int s);
  void setSpacingV(int s);

  void setApplyColorization(bool en);
  void setColorizationStrength(qreal v);
  void setColorizationColor(const QColor& c);

  void setUseFontInsteadOfSkin(bool en);
  void setFont(const QFont& f);
  void setSkin(const QString& s);

  void setSeparatorFlashes(bool en);
  void setUseCustomSeparators(bool en);
  void setCustomSeparators(const QString& s);

  void setTimeFormat(const QString& s);
  void setSecondsScaleFactor(int s);

  void setTexture(const QBrush& tx);
  void setTextureStretch(bool en);
  void setTexturePerCharacter(bool en);

  void setBackground(const QBrush& bg);
  void setBackgroundStretch(bool en);
  void setBackgroundPerCharacter(bool en);

signals:
  void optionChanged(cs::SharedConfigKeys opt, const QVariant& val);
};


class PLUGIN_CORE_EXPORT SettingsChangeReceiver : public QObject
{
  Q_OBJECT

public:
  explicit SettingsChangeReceiver(QObject* parent = nullptr);

public slots:
  void onOptionChanged(cs::SharedConfigKeys opt, const QVariant& val);

protected:
  virtual void onOpacityChanged(int o) = 0;

  virtual void onScalingHChanged(int s) = 0;
  virtual void onScalingVChanged(int s) = 0;

  virtual void onSpacingHChanged(int s) = 0;
  virtual void onSpacingVChanged(int s) = 0;

  virtual void onApplyColorizationChanged(bool en) = 0;
  virtual void onColorizationStrengthChanged(qreal v) = 0;
  virtual void onColorizationColorChanged(const QColor& c) = 0;

  virtual void onUseFontInsteadOfSkinChanged(bool en) = 0;
  virtual void onFontChanged(const QFont& f) = 0;
  virtual void onSkinChanged(const QString& s) = 0;

  virtual void onSeparatorFlashesChanged(bool en) = 0;
  virtual void onUseCustomSeparatorsChanged(bool en) = 0;
  virtual void onCustomSeparatorsChanged(const QString& s) = 0;

  virtual void onTimeFormatChanged(const QString& s) = 0;
  virtual void onSecondsScaleFactorChanged(int s) = 0;

  virtual void onTextureChanged(const QBrush& tx) = 0;
  virtual void onTextureStretchChanged(bool en) = 0;
  virtual void onTexturePerCharacterChanged(bool en) = 0;

  virtual void onBackgroundChanged(const QBrush& bg) = 0;
  virtual void onBackgroundStretchChanged(bool en) = 0;
  virtual void onBackgroundPerCharacterChanged(bool en) = 0;
};
