/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QSystemTrayIcon>

#include <QTime>

class ClockTrayIcon : public QSystemTrayIcon
{
  Q_OBJECT
public:
  explicit ClockTrayIcon(QObject* parent = nullptr);

private slots:
  void updateIcon();

private:
  QTime m_last_update;
};
