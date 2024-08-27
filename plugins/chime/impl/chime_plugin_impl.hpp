/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "chime_settings.hpp"

struct ChimePluginImpl
{
  void initSettings(const SettingsClient& st);
  bool isQuietTime(const QTime& t) const;

  bool every_hour_enabled = false;
  QUrl every_hour_signal;
  int every_hour_volume = 90;
  chime::Repeat every_hour_repeat = chime::Once;

  bool quarter_hour_enabled = false;
  QUrl quarter_hour_signal;
  int quarter_hour_volume = 90;
  chime::Repeat quarter_hour_repeat = chime::Once;

  bool quiet_hours_enabled = false;
  QTime quiet_hours_start;
  QTime quiet_hours_end;

  bool playback_allowed = false;
};
