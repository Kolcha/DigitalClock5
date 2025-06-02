/*
 * SPDX-FileCopyrightText: 2017-2020 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "countdown_timer.hpp"

namespace countdown_timer {

CountdownTimer::CountdownTimer(QObject* parent) :
  QObject(parent),
  timer_id_(-1), interval_(-1), time_left_(-1),
  is_active_(false)
{
  connect(this, &CountdownTimer::timeout, this, &CountdownTimer::stop);
}

CountdownTimer::~CountdownTimer()
{
  if (isActive()) stop();
}

void CountdownTimer::start()
{
  if (isActive() || timeLeft() <= 0) return;
  timer_id_ = startTimer(1000);
  is_active_ = true;
  emit activityChanged(is_active_);
}

void CountdownTimer::stop()
{
  if (!isActive()) return;
  killTimer(timer_id_);
  is_active_ = false;
  emit activityChanged(is_active_);
}

void CountdownTimer::setInterval(qint64 seconds)
{
  setTimeLeft(seconds);
  interval_ = seconds;
}

void CountdownTimer::timerEvent(QTimerEvent* event)
{
  if (timeLeft() > 0) setTimeLeft(timeLeft() - 1);
  if (timeLeft() == 0) emit timeout();
  QObject::timerEvent(event);
}

void CountdownTimer::setTimeLeft(qint64 time_left)
{
  if (time_left < 0 || time_left == time_left_) return;
  time_left_ = time_left;
  emit timeLeftChanged(time_left_);
}

} // namespace countdown_timer
