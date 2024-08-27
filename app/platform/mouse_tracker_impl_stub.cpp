/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mouse_tracker.hpp"

class MouseTrackerPrivate {};

MouseTracker::MouseTracker(QObject* parent)
  : QObject(parent)
{
}

MouseTracker::~MouseTracker() = default;
