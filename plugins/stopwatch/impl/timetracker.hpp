/*
 * SPDX-FileCopyrightText: 2018-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QObject>
#include <QElapsedTimer>

namespace timetracker {

class Timetracker : public QObject
{
  Q_OBJECT

public:
  explicit Timetracker(QObject* parent = nullptr);

  int elapsed() const;
  bool isActive() const;

public slots:
  void start();
  void stop();
  void reset();

  void setElapsed(int elapsed_secs);

signals:
  void activityChanged(bool active);

private:
  QElapsedTimer timer_;
  int last_elapsed_ = 0;
};

} // namespace timetracker
