/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QHotkey>

#include "config/settings_storage.hpp"
#include "skin/graphics_widgets.hpp"

#include "timetracker.hpp"

struct StopwatchPluginImpl
{
  void initSettings(const SettingsClient& s);
  void applySettings();

  bool last_active = false;
  int last_elapsed = 0;

  bool hide_inactive = false;

  std::unique_ptr<QHotkey> pause_hotkey;
  std::unique_ptr<QHotkey> restart_hotkey;

  std::shared_ptr<GraphicsTextWidget> widget;
  std::unique_ptr<timetracker::Timetracker> tracker;
};
