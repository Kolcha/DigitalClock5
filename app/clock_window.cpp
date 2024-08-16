/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "clock_window.hpp"

#include <QMouseEvent>

ClockWindow::ClockWindow(QWidget* parent)
    : QWidget(parent)
{
  _layout = new QGridLayout(this);
  _layout->setSizeConstraint(QLayout::SetFixedSize);

  _clock = new GraphicsDateTimeWidget(this);
  _layout->addWidget(_clock);

  setLayout(_layout);
}

void ClockWindow::addPluginWidget(GraphicsWidgetBase* w, int row, int column,
                                  int row_span, int column_span)
{
  w->setScaling(_sx, _sy);
  _layout->addWidget(w, row, column, row_span, column_span);
}

void ClockWindow::saveState(State& s) const
{
  s.setValue("state/origin", _last_origin);
  s.setValue("state/anchor", (int)_anchor_point);
}

void ClockWindow::loadState(const State& s)
{
  // TODO: maybe set default point based on graphics size
  // as practice shown, it is better to avoid negative Y values :(
  _last_origin = s.value("state/origin", QPoint(300, 300)).toPoint();
  _anchor_point = (AnchorPoint)s.value("state/anchor", (int)AnchorLeft).toInt();
}

void ClockWindow::setAnchorPoint(AnchorPoint ap)
{
  _anchor_point = ap;
  emit saveStateRequested();
}

void ClockWindow::setSnapToEdge(bool enable, int threshold)
{
  _snap_to_edge = enable;
  _snap_threshold = threshold;
}

void ClockWindow::setScaling(qreal sx, qreal sy)
{
  _sx = sx;
  _sy = sy;
  const auto& widgets = children();
  for (auto o : widgets)
    if (auto w = qobject_cast<GraphicsWidgetBase*>(o))
      w->setScaling(_sx, _sy);
  updateGeometry();
  update();
}

void ClockWindow::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    _drag_pos = event->globalPosition().toPoint() - frameGeometry().topLeft();
    _is_dragging = true;
    event->accept();
  }
}

void ClockWindow::mouseMoveEvent(QMouseEvent* event)
{
  if (event->buttons() & Qt::LeftButton) {
    QPoint target_pos = event->globalPosition().toPoint() - _drag_pos;
    if (_snap_to_edge) {
      QRect screen = window()->screen()->availableGeometry();
      QRect widget = frameGeometry();
      if (qAbs(target_pos.x() - screen.left()) <= _snap_threshold)
        target_pos.setX(screen.left());
      if (qAbs(target_pos.y() - screen.top()) <= _snap_threshold)
        target_pos.setY(screen.top());
      if (qAbs(screen.right() - (target_pos.x() + widget.width())) <= _snap_threshold)
        target_pos.setX(screen.right() - widget.width());
      if (qAbs(screen.bottom() - (target_pos.y() + widget.height())) <= _snap_threshold)
        target_pos.setY(screen.bottom() - widget.height());
    }
    move(target_pos);
    event->accept();
  }
}

void ClockWindow::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    auto curr_origin = _clock->mapToGlobal(_clock->origin());

    switch (_anchor_point) {
      case AnchorLeft:
        _last_origin = curr_origin;
        break;
      case AnchorCenter:
        _last_origin = curr_origin + QPoint(_clock->advance().x(), 0)/2;
        break;
      case AnchorRight:
        _last_origin = curr_origin + QPoint(_clock->advance().x(), 0);
        break;
    }

    emit saveStateRequested();

    _is_dragging = false;
    event->accept();
  }
}

void ClockWindow::paintEvent(QPaintEvent* event)
{
  // TODO: draw frame when setting dialog is open
  QWidget::paintEvent(event);
}

void ClockWindow::resizeEvent(QResizeEvent* event)
{
  QWidget::resizeEvent(event);

  // do not apply anhoring logic during dragging
  if (_is_dragging) return;

  // state restore must happen before the first resize event!
  // previous origin and correct anchor point must be known here
  auto curr_origin = _clock->mapToGlobal(_clock->origin());

  switch (_anchor_point) {
    case AnchorLeft:
      move(pos() + _last_origin - curr_origin);
      break;
    case AnchorCenter:
      move(pos() + _last_origin - (curr_origin + QPoint(_clock->advance().x(), 0)/2));
      break;
    case AnchorRight:
      move(pos() + _last_origin - (curr_origin + QPoint(_clock->advance().x(), 0)));
      break;
  }
}
