/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QLabel>

#include <memory>

class LogoLabel final : public QLabel
{
  Q_OBJECT

public:
  explicit LogoLabel(QWidget* parent = nullptr);
  ~LogoLabel();

protected:
  void mouseReleaseEvent(QMouseEvent* event) override;

private:
  struct Impl;
  std::unique_ptr<Impl> _impl;
};
