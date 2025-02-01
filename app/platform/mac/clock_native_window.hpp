/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "gui/clock_window.hpp"

struct ClockWindow::PlatformData {
  bool should_stay_on_top = false;
  bool detect_fullscreen = true;

  bool should_apply_on_top_fullscreen = false;
  bool should_apply_on_all_desktops = false;
  bool should_apply_hidden_in_mc = false;
  int ticks_count = 0;
};
