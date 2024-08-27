/*
 * SPDX-FileCopyrightText: 2020-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mouse_tracker.hpp"

#import <AppKit/NSEvent.h>
#import <AppKit/NSScreen.h>

class MouseTrackerPrivate : public QObject
{
  Q_OBJECT

public:
  MouseTrackerPrivate(QObject* parent = nullptr);
  ~MouseTrackerPrivate();

signals:
  void mousePosChanged(QPoint pos, Qt::KeyboardModifiers m);

private:
  id _global_monitor = nil;
  id _local_monitor = nil;
};


static NSPoint point_from_top_left(NSPoint p)
{
  CGFloat global_height = 0;
  for (NSScreen* screen in [NSScreen screens]) {
    NSRect rect = [screen frame];
    CGFloat cur_height = rect.origin.y + rect.size.height;
    if (cur_height > global_height)
      global_height = cur_height;
  }

  p.y = global_height - p.y;
  return p;
}


MouseTrackerPrivate::MouseTrackerPrivate(QObject* parent)
  : QObject(parent)
{
  void (^mouse_even_handler)(NSEvent*) = ^(NSEvent* event) {
    if ([event type] == NSEventTypeMouseMoved) {
      NSPoint p = point_from_top_left([NSEvent mouseLocation]);
      NSEventModifierFlags mod_flags = [NSEvent modifierFlags];
      Qt::KeyboardModifiers m = Qt::NoModifier;
      if (mod_flags & NSEventModifierFlagShift)
        m |= Qt::ShiftModifier;
      if (mod_flags & NSEventModifierFlagControl)
        m |= Qt::ControlModifier;
      if (mod_flags & NSEventModifierFlagOption)
        m |= Qt::AltModifier;
      if (mod_flags & NSEventModifierFlagCommand)
        m |= Qt::ControlModifier;
      emit mousePosChanged(QPoint(p.x, p.y), m);
    }
  };

  if (!_global_monitor) {
    _global_monitor = [NSEvent addGlobalMonitorForEventsMatchingMask:
            (NSEventMaskMouseMoved)
            handler:mouse_even_handler];
  }

  if (!_local_monitor) {
    _local_monitor = [NSEvent addLocalMonitorForEventsMatchingMask:
            (NSEventMaskMouseMoved)
            handler:^(NSEvent* event) {
        mouse_even_handler(event);
        return event;
    }];
  }
}

MouseTrackerPrivate::~MouseTrackerPrivate()
{
  if (_global_monitor) {
    [NSEvent removeMonitor: _global_monitor];
    _global_monitor = nil;
  }

  if (_local_monitor) {
    [NSEvent removeMonitor: _local_monitor];
    _local_monitor = nil;
  }
}


MouseTracker::MouseTracker(QObject* parent)
  : QObject(parent)
  , m_impl(std::make_unique<MouseTrackerPrivate>())
{
  connect(m_impl.get(), &MouseTrackerPrivate::mousePosChanged, this, &MouseTracker::mousePositionChanged);
}

MouseTracker::~MouseTracker() = default;

#include "mouse_tracker.moc"
