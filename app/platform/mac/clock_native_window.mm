/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "clock_native_window.hpp"

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


ClockNativeWindow::ClockNativeWindow(QWidget* parent)
    : ClockWindow(parent)
{
  setHiddenInMissionControl(true);
  setVisibleOnAllDesktops(true);
}

void ClockNativeWindow::setStayOnTop(bool en)
{
  ClockWindow::setStayOnTop(en);

  NSView* view = reinterpret_cast<NSView*>(winId());
  SetVisibleInFullscreen(view.window, !_detect_fullscreen);
}

void ClockNativeWindow::setHiddenInMissionControl(bool en)
{
  NSView* view = reinterpret_cast<NSView*>(winId());
  SetCollectionBehavior(view.window, NSWindowCollectionBehaviorManaged, !en);
  SetCollectionBehavior(view.window, NSWindowCollectionBehaviorTransient, en);
}

void ClockNativeWindow::setVisibleOnAllDesktops(bool en)
{
  NSView* view = reinterpret_cast<NSView*>(winId());
  SetCollectionBehavior(view.window, NSWindowCollectionBehaviorCanJoinAllSpaces, en);
  SetVisibleInFullscreen(view.window, !_detect_fullscreen);
}
