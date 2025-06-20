/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <QtGui/QIconEngine>

class ClockIconEngine : public QIconEngine
{
public:
  explicit ClockIconEngine(QColor color = QColor(101, 99, 255));

  void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
  QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

  [[nodiscard]] QIconEngine* clone() const override;

  QList<QSize> availableSizes(QIcon::Mode mode, QIcon::State state) override;

private:
  QColor m_color;
};
