/*
 * SPDX-FileCopyrightText: 2018-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "timetracker.hpp"

namespace timetracker {

Timetracker::Timetracker(QObject* parent) : QObject(parent)
{
}

int Timetracker::elapsed() const
{
  int elapsed = last_elapsed_;
  if (timer_.isValid())
    elapsed += timer_.elapsed() / 1000;
  return elapsed;
}

bool Timetracker::isActive() const
{
  return timer_.isValid();
}

void Timetracker::start()
{
  if (timer_.isValid())
    return;

  timer_.start();
  emit activityChanged(timer_.isValid());
}

void Timetracker::stop()
{
  if (!timer_.isValid())
    return;

  last_elapsed_ += timer_.elapsed() / 1000;
  timer_.invalidate();
  emit activityChanged(timer_.isValid());
}

void Timetracker::reset()
{
  last_elapsed_ = 0;
  if (timer_.isValid())
    timer_.restart();
  emit activityChanged(timer_.isValid());
}

void Timetracker::setElapsed(int elapsed_secs)
{
  last_elapsed_ = elapsed_secs;
}

} // namespace timetracker
