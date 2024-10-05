/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "gui/clock_window.hpp"

#import <AppKit/NSWindow.h>


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


void ClockWindow::setHiddenInMissionControl(bool en)
{
  NSView* view = reinterpret_cast<NSView*>(winId());
  SetCollectionBehavior(view.window, NSWindowCollectionBehaviorManaged, !en);
  SetCollectionBehavior(view.window, NSWindowCollectionBehaviorTransient, en);
}

void ClockWindow::setVisibleOnAllDesktops(bool en)
{
  NSView* view = reinterpret_cast<NSView*>(winId());
  SetCollectionBehavior(view.window, NSWindowCollectionBehaviorCanJoinAllSpaces, en);
  SetVisibleInFullscreen(view.window, !_detect_fullscreen);
}

void ClockWindow::setStayOnTop(bool en)
{
  NSView* view = reinterpret_cast<NSView*>(winId());
  [view.window setLevel: en ? NSFloatingWindowLevel : NSNormalWindowLevel];
  SetVisibleInFullscreen(view.window, !_detect_fullscreen);
}

void ClockWindow::setTransparentForInput(bool en)
{
  NSView* view = reinterpret_cast<NSView*>(winId());
  [view.window setIgnoresMouseEvents: en];
}
