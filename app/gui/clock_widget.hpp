// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "skinned_widget.hpp"

#include "skin/time_renderer.hpp"

class ClockWidget : public SkinnedWidgetBase
{
  Q_OBJECT

public:
  explicit ClockWidget(QWidget* parent = nullptr);

  void setFormat(QString fmt);
  void setOptions(LinesRenderer::Options opt);

  void setCustomSeparators(QString seps);

  void setTokenTransform(const QString& token, const QTransform& tr);

public slots:
  void setDateTime(const QDateTime& dt);

  void setFlashingSeparator(bool en);
  inline void enableFlashingSeparator() { setFlashingSeparator(true); }
  inline void disableFlashingSeparator() { setFlashingSeparator(false); }

  void animateSeparator();

protected:
  std::unique_ptr<Renderable> buildRenderable() override;

private:
  ClockWidget(std::unique_ptr<TimeRenderer> r, QWidget *parent);

private:
  std::unique_ptr<TimeRenderer> _rr;
  QDateTime _dt;
};
