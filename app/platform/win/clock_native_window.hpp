/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "gui/clock_window.hpp"

class ClockNativeWindow : public ClockWindow
{
  Q_OBJECT

public:
  explicit ClockNativeWindow(QWidget* parent = nullptr);

public slots:
  void setStayOnTop(bool en) override;

  void runStayOnTopHacks();
};
