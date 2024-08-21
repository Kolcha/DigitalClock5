/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "settings_bridge.hpp"

class Application;
class ClockWindow;

// any of received changes should not be committed, just applied on runtime
class SettingsChangeListener : public SettingsChangeReceiver
{
  Q_OBJECT

public:
  explicit SettingsChangeListener(Application* a, size_t i, QObject* parent = nullptr);
  ~SettingsChangeListener();

protected:
  void onOpacityChanged(int o) override;

  void onScalingHChanged(int s) override;
  void onScalingVChanged(int s) override;

  void onSpacingHChanged(int s) override;
  void onSpacingVChanged(int s) override;

  void onApplyColorizationChanged(bool en) override;
  void onColorizationStrengthChanged(qreal v) override;
  void onColorizationColorChanged(const QColor& c) override;

  void onUseFontInsteadOfSkinChanged(bool en) override;
  void onFontChanged(const QFont& f) override;
  void onSkinChanged(const QString& s) override;

  void onSeparatorFlashesChanged(bool en) override;
  void onUseCustomSeparatorsChanged(bool en) override;
  void onCustomSeparatorsChanged(const QString& s) override;

  void onTimeFormatChanged(const QString& s) override;
  void onSecondsScaleFactorChanged(int s) override;

  void onTextureChanged(const QBrush& tx) override;
  void onTextureStretchChanged(bool en) override;
  void onTexturePerCharacterChanged(bool en) override;

  void onBackgroundChanged(const QBrush& bg) override;
  void onBackgroundStretchChanged(bool en) override;
  void onBackgroundPerCharacterChanged(bool en) override;

private:
  Application* _app;
  ClockWindow* _wnd;

  struct Impl;
  std::unique_ptr<Impl> _impl;
};
