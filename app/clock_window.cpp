/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "clock_window.hpp"

#include <QMenu>

#include <QContextMenuEvent>
#include <QMouseEvent>

#include <QPainter>

ClockWindow::ClockWindow(QWidget* parent)
    : QWidget(parent)
{
  _layout = new QGridLayout(this);
  _layout->setSizeConstraint(QLayout::SetFixedSize);

  _clock = new GraphicsDateTimeWidget(this);
  _layout->addWidget(_clock);

  setLayout(_layout);

  using namespace Qt::Literals::StringLiterals;
  _ctx_menu = new QMenu(this);
  _ctx_menu->addAction(QIcon::fromTheme(u"configure"_s), tr("Settings"),
                       this, &ClockWindow::settingsDialogRequested);
  _ctx_menu->addSeparator();
  _ctx_menu->addAction(QIcon::fromTheme(u"help-about"_s), tr("About"),
                       this, &ClockWindow::aboutDialogRequested);
  _ctx_menu->addSeparator();
  _ctx_menu->addAction(QIcon::fromTheme(u"application-exit"_s), tr("Quit"),
                       this, &ClockWindow::appExitRequested);
}

void ClockWindow::addPluginWidget(GraphicsWidgetBase* w, int row, int column,
                                  int row_span, int column_span)
{
  w->setScaling(_sx, _sy);
  _layout->addWidget(w, row, column, row_span, column_span);
}

void ClockWindow::saveState(State& s) const
{
  s.setValue("origin", _last_origin);
  s.setValue("anchor", (int)_anchor_point);
}

void ClockWindow::loadState(const State& s)
{
  // TODO: maybe set default point based on graphics size
  // as practice shown, it is better to avoid negative Y values :(
  _last_origin = s.value("origin", _last_origin).toPoint();
  _anchor_point = (AnchorPoint)s.value("anchor", (int)AnchorLeft).toInt();
}

void ClockWindow::setAnchorPoint(AnchorPoint ap)
{
  _anchor_point = ap;
  emit saveStateRequested();
}

void ClockWindow::setOriginPoint(QPoint origin)
{
  _last_origin = std::move(origin);
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

void ClockWindow::setFrameVisible(bool vis)
{
  _frame_visible = vis;
  update();
}

void ClockWindow::contextMenuEvent(QContextMenuEvent* event)
{
  _ctx_menu->popup(event->globalPos());
  event->accept();
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
  QWidget::paintEvent(event);

  if (_frame_visible) {
    QPainter p(this);
    p.setPen(QPen(Qt::red, 2, Qt::DotLine, Qt::SquareCap, Qt::MiterJoin));
    p.drawRect(rect().adjusted(1, 1, -1, -1));
  }
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
