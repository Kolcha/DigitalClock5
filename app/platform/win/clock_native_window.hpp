/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "gui/clock_window.hpp"

struct ClockWindow::PlatformData {
  QScreen* last_screen = nullptr;
  QSet<QString> fullscreen_ignore_list;
  bool should_stay_on_top = false;
  bool detect_fullscreen = true;

  bool should_apply_win_d_hack = false;
  int ticks_count = 0;
};
