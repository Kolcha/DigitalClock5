/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QHotkey>

#include "config/settings_storage.hpp"
#include "skin/graphics_widgets.hpp"

#include "countdown_timer.hpp"

struct CountdownTimerPluginImpl
{
  void initSettings(const SettingsClient& s);
  void applySettings();

  void initTimer();

  QString widgetText() const;
  void updateWidgetText();

  int interval_hours = 0;
  int interval_minutes = 0;
  int interval_seconds = 0;

  bool use_target_datetime = false;
  QDateTime target_date_time;

  bool chime_on_timeout = false;
  QString chime_sound;

  bool show_message = false;
  QString message_text;

  int hide_days_threshold = 0;
  bool also_hide_hours = false;

  bool restart_on_dbl_click = false;
  bool restart_on_timeout = false;

  std::unique_ptr<QHotkey> pause_hotkey;
  std::unique_ptr<QHotkey> restart_hotkey;

  bool hide_inactive = false;
  bool reverse_counting = false;

  std::shared_ptr<GraphicsTextWidget> widget;
  std::unique_ptr<countdown_timer::CountdownTimer> timer;
};
