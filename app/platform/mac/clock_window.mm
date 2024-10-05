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


void ClockWindow::hideInMissionControl()
{
  NSView* view = reinterpret_cast<NSView*>(winId());
  SetCollectionBehavior(view.window, NSWindowCollectionBehaviorTransient, true);
}
