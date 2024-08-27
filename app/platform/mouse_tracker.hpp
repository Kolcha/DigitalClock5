/*
 * SPDX-FileCopyrightText: 2020-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QObject>

#include <memory>

#include <QPoint>

class MouseTrackerPrivate;

class MouseTracker : public QObject
{
  Q_OBJECT

public:
  explicit MouseTracker(QObject* parent = nullptr);
  ~MouseTracker();

signals:
  void mousePositionChanged(QPoint p, Qt::KeyboardModifiers m);

private:
  std::unique_ptr<MouseTrackerPrivate> m_impl;
};
