// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "clock_widget.hpp"

ClockWidget::ClockWidget(QWidget* parent)
  : ClockWidget(std::make_unique<TimeRenderer>(), parent)
{
}

void ClockWidget::setFormat(QString fmt)
{
  _rr->setFormat(std::move(fmt));
  doUpdateGeometry();
}

void ClockWidget::setOptions(LinesRenderer::Options opt)
{
  _rr->setOptions(std::move(opt));
  doUpdateGeometry();
}

void ClockWidget::setCustomSeparators(QString seps)
{
  _rr->setCustomSeparators(std::move(seps));
  doUpdateGeometry();
}

void ClockWidget::setTokenTransform(const QString& token, const QTransform& tr)
{
  _rr->setTokenTransform(token, tr);
  doUpdateGeometry();
}

void ClockWidget::setDateTime(const QDateTime& dt)
{
  _dt = dt;
  doUpdateGeometry();
}

void ClockWidget::setFlashingSeparator(bool en)
{
  _rr->setFlashingSeparator(en);
  doRepaint();
}

void ClockWidget::animateSeparator()
{
  _rr->animateSeparator();
  doRepaint();
}

std::unique_ptr<Renderable> ClockWidget::buildRenderable()
{
  return _rr->draw(_dt);
}

ClockWidget::ClockWidget(std::unique_ptr<TimeRenderer> r, QWidget* parent)
  : SkinnedWidgetBase(*r, parent)
  , _rr(std::move(r))
  , _dt(QDateTime::currentDateTime())
{
}
