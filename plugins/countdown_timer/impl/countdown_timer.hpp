/*
 * SPDX-FileCopyrightText: 2017-2020 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QObject>

namespace countdown_timer {

class CountdownTimer : public QObject
{
  Q_OBJECT
  Q_PROPERTY(qint64 interval READ interval WRITE setInterval)
  Q_PROPERTY(qint64 timeLeft READ timeLeft NOTIFY timeLeftChanged)
  Q_PROPERTY(bool isActive READ isActive NOTIFY activityChanged)

public:
  explicit CountdownTimer(QObject* parent = nullptr);
  ~CountdownTimer();

  qint64 interval() const { return interval_; }
  qint64 timeLeft() const { return time_left_; }
  bool isActive() const { return is_active_; }

signals:
  void activityChanged(bool is_active);
  void timeLeftChanged(qint64 time_left);
  void timeout();

public slots:
  void start();
  void stop();

  void setInterval(int seconds);

protected:
  void timerEvent(QTimerEvent* event) override;

private:
  void setTimeLeft(qint64 time_left);

private:
  int timer_id_;
  qint64 interval_;
  qint64 time_left_;
  bool is_active_;
};

} // namespace countdown_timer
