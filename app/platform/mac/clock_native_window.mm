/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "clock_native_window.hpp"

#import <AppKit/NSWindow.h>

namespace {

// postpone applying window flags for a few ticks
// it is known that even isVisible() returns true,
// manipulations on winId() may have no effect...
static constexpr const int max_ticks = 2;

} // namespace

static void SetCollectionBehavior(NSWindow* window, NSUInteger flag, bool on)
{
  if (on)
    [window setCollectionBehavior:[window collectionBehavior] | flag];
  else
    [window setCollectionBehavior:[window collectionBehavior] & (~flag)];
}

static void SetVisibleInFullscreen(NSWindow* window, bool vis)
{
  SetCollectionBehavior(window, NSWindowCollectionBehaviorFullScreenNone, !vis);
  SetCollectionBehavior(window, NSWindowCollectionBehaviorFullScreenAuxiliary, vis);
}

inline void setVisibleInFullscreen(NSView* view, bool en)
{
  SetVisibleInFullscreen(view.window, en);
}

static void setHiddenInMissionControl(NSView* view, bool en)
{
  SetCollectionBehavior(view.window, NSWindowCollectionBehaviorManaged, !en);
  SetCollectionBehavior(view.window, NSWindowCollectionBehaviorTransient, en);
}

static void setVisibleOnAllDesktops(NSView* view, bool en)
{
  SetCollectionBehavior(view.window, NSWindowCollectionBehaviorCanJoinAllSpaces, en);
}


void ClockWindow::platformOneTimeFlags()
{
  // this is called in constructor, so window is not yet visible
  // calling winId() is unsafe, so postpone setting window flags
  _data->should_apply_hidden_in_mc = true;
  _data->should_apply_on_all_desktops = true;
}

void ClockWindow::platformTick()
{
  bool need_to_set_flag =
      _data->should_apply_on_top_fullscreen ||
      _data->should_apply_hidden_in_mc ||
      _data->should_apply_on_all_desktops;

  if (need_to_set_flag && isVisible() && _data->ticks_count++ >= max_ticks) {
    NSView* view = reinterpret_cast<NSView*>(winId());

    if (_data->should_apply_on_all_desktops) {
      setVisibleOnAllDesktops(view, true);
      _data->should_apply_on_all_desktops = false;
    }

    if (_data->should_apply_hidden_in_mc) {
      setHiddenInMissionControl(view, true);
      _data->should_apply_hidden_in_mc = false;
    }

    if (_data->should_apply_on_top_fullscreen) {
      setVisibleInFullscreen(view, _data->should_stay_on_top && !_data->detect_fullscreen);
      _data->should_apply_on_top_fullscreen = false;
    }

    _data->ticks_count = 0;
  }
}

void ClockWindow::platformStayOnTop(bool enabled)
{
  _data->should_stay_on_top = enabled;
  _data->should_apply_on_top_fullscreen = true;
}

void ClockWindow::platformFullScreenDetect(bool enabled)
{
  _data->detect_fullscreen = enabled;
  _data->should_apply_on_top_fullscreen = true;
}
