// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "stopwatch_settings.hpp"

namespace timetracker {

QString StopwatchInstanceConfig::key(Options o)
{
  switch (o) {
    case HideInactive:          return "hide_inactive";

    case PauseHotkey:           return "pause_hotkey";
    case RestartHotkey:         return "restart_hotkey";
  }
  Q_UNREACHABLE();
}

QVariant StopwatchInstanceConfig::def_value(Options o)
{
  switch (o) {
    case HideInactive:          return false;

    case PauseHotkey:           return QString();
    case RestartHotkey:         return QString();
  }
  Q_UNREACHABLE();
}

} // namespace timetracker
