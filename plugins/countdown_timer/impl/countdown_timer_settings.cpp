// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "countdown_timer_settings.hpp"

namespace countdown_timer {

QDateTime default_target_date()
{
  return QDateTime(QDate(QDate::currentDate().year() + 1, 1, 1), QTime(0, 0, 0));
}

QString CountdownTimerInstanceConfig::key(Options o)
{
  switch (o) {
    case IntervalHours:         return "interval_hours";
    case IntervalMinutes:       return "interval_minutes";
    case IntervalSeconds:       return "interval_seconds";

    case UseTargetDateTime:     return "use_target_time";
    case TargetDateTime:        return "target_datetime";

    case ChimeOnTimeout:        return "chime_on_timeout";
    case ChimeSoundFile:        return "chime_sound_file";

    case ShowMessage:           return "show_message";
    case MessageText:           return "message_text";

    case HideDaysThreshold:     return "hide_days_threshold";
    case AlsoHideHours:         return "also_hide_hours";

    case RestartOnDblClick:     return "restart_on_dblclick";
    case RestartOnTimeout:      return "restart_on_timeout";

    case HideInactive:          return "hide_inactive";

    case ReverseCounting:       return "reverse_counting";

    case PauseHotkey:           return "pause_hotkey";
    case RestartHotkey:         return "restart_hotkey";
  }
  Q_ASSERT(false);
  return {};
}

QVariant CountdownTimerInstanceConfig::def_value(Options o)
{
  switch (o) {
    case IntervalHours:         return 0;
    case IntervalMinutes:       return 0;
    case IntervalSeconds:       return 0;

    case UseTargetDateTime:     return true;
    case TargetDateTime:        return default_target_date();

    case ChimeOnTimeout:        return false;
    case ChimeSoundFile:        return QString();

    case ShowMessage:           return false;
    case MessageText:           return QString();

    case HideDaysThreshold:     return 0;
    case AlsoHideHours:         return false;

    case RestartOnDblClick:     return false;
    case RestartOnTimeout:      return false;

    case HideInactive:          return false;

    case ReverseCounting:       return false;

    case PauseHotkey:           return QString();
    case RestartHotkey:         return QString();
  }
  Q_ASSERT(false);
  return {};
}

} // namespace countdown_timer
