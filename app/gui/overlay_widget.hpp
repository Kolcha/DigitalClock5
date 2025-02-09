/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

class OverlayWidget : public QWidget
{
  Q_OBJECT

public:
  explicit OverlayWidget(QWidget* parent = nullptr);

  bool frameVisible() const { return _frame_visible; }

public slots:
  void enableFrame() { setFrameVisible(true); }
  void disableFrame() { setFrameVisible(false); }
  void setFrameVisible(bool vis);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  bool _frame_visible = false;
};
