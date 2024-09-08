/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "chime_plugin_impl.hpp"

#include "chime_settings.hpp"

static bool is_quiet_time(const QTime& c_time, const QTime& s_time, const QTime& e_time)
{
  if (s_time <= e_time)
    return (s_time <= c_time) && (c_time <= e_time);

  return ((s_time <= c_time) && (c_time <= QTime(23, 59, 59, 999))) ||
         ((QTime(0, 0) <= c_time) && (c_time <= e_time));
}

void ChimePluginImpl::initSettings(PluginSettingsStorage& st)
{
  plugin_impl::ChimePluginSettings cfg(st);

  every_hour_enabled = cfg.hour().getEnabled();
  every_hour_signal = cfg.hour().getSignal();
  every_hour_volume = cfg.hour().getVolume();
  every_hour_repeat = cfg.hour().getRepeat();

  quarter_hour_enabled = cfg.quarter().getEnabled();
  quarter_hour_signal = cfg.quarter().getSignal();
  quarter_hour_volume = cfg.quarter().getVolume();
  quarter_hour_repeat = cfg.quarter().getRepeat();

  quiet_hours_enabled = cfg.quietTime().getEnabled();
  quiet_hours_start = cfg.quietTime().getStartTime();
  quiet_hours_end = cfg.quietTime().getEndTime();
}

bool ChimePluginImpl::isQuietTime(const QTime& t) const
{
  if (quiet_hours_enabled)
    return is_quiet_time(t, quiet_hours_start, quiet_hours_end);
  return false;
}
