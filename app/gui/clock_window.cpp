/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "clock_window.hpp"

#include <QApplication>

#include <QMenu>
#include <QScreen>

#include <QContextMenuEvent>
#include <QMouseEvent>

namespace {

// this wrapper function allows to change window flag on runtime as it should be
// after setWindowFlag() call widget becomes hidden, and must be shown explicitly
// show() activates the widget, so previous becomes inactive, this is annoying...
// so, remember the current active widget and restore it after changing the flag
void wrap_set_window_flag(QWidget* wnd, Qt::WindowType flag, bool set)
{
  QWidget* aw = QApplication::activeWindow();
  bool last_visible = wnd->isVisible();
  wnd->setWindowFlag(flag, set);
  if (last_visible != wnd->isVisible()) wnd->show();
  if (aw) aw->activateWindow();
}

} // namespace

ClockWindow::ClockWindow(QWidget* parent)
    : QWidget(parent)
{
  setAttribute(Qt::WA_NativeWindow);
  setAttribute(Qt::WA_DontCreateNativeAncestors);

  _layout = new QGridLayout(this);
  _layout->setSizeConstraint(QLayout::SetFixedSize);
  _layout->setContentsMargins(0, 0, 0, 0);
  _layout->setSpacing(0);

  _clock = new GraphicsDateTimeWidget(this);
  _layout->addWidget(_clock);

  setLayout(_layout);

  // not in layout! resized manually
  _overlay = new OverlayWidget(this);
  _overlay->move({0, 0});
  _overlay->resize(sizeHint());

  using namespace Qt::Literals::StringLiterals;
  _ctx_menu = new QMenu(this);
  _ctx_menu->addAction(QIcon(":/icons/configure.svg"), tr("Settings"),
                       this, &ClockWindow::settingsDialogRequested);
  addPositionMenu();
  _ctx_menu->addSeparator();
  _ctx_menu->addAction(QIcon(":/icons/help-about.svg"), tr("About"),
                       this, &ClockWindow::aboutDialogRequested);
  _ctx_menu->addSeparator();
  _ctx_menu->addAction(QIcon(":/icons/application-exit.svg"), tr("Quit"),
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

void ClockWindow::moveToPredefinedPos(Qt::Alignment a)
{
  auto tpos = pos();

  auto sg = screen()->availableGeometry();

  if (a & Qt::AlignLeft)
    tpos.setX(sg.left());
  if (a & Qt::AlignHCenter)
    tpos.setX(sg.center().x() - width()/2);
  if (a & Qt::AlignRight)
    tpos.setX(sg.right() - width());

  if (a & Qt::AlignTop)
    tpos.setY(sg.top());
  if (a & Qt::AlignVCenter)
    tpos.setY(sg.center().y() - height()/2);
  if (a & Qt::AlignBottom)
    tpos.setY(sg.bottom() - height());

  move(tpos);
  updateLastOrigin();
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

void ClockWindow::setKeepVisible(bool en)
{
  _keep_visible = en;
  updateWindowPos();
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
  _overlay->setFrameVisible(vis);
}

void ClockWindow::setTransparentOnHover(bool en)
{
  _transparent_on_hover = en;
}

void ClockWindow::setOpacityOnHover(qreal o)
{
  _opacity_on_hover = o;
}

#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
void ClockWindow::handleMouseMove(const QPoint& global_pos)
{
  if (!_transparent_on_hover)
    return;

  bool entered = property("dc_mouse_entered").toBool();

  QRect rect = frameGeometry();
#ifndef Q_OS_MACOS
  QTransform t;
  t.scale(devicePixelRatioF(), devicePixelRatioF());
  rect = t.mapRect(rect);
#endif
  if (rect.contains(global_pos) && !entered) {
    entered = true;
    setProperty("dc_orig_opacity", windowOpacity());
    setWindowOpacity(_opacity_on_hover);
  }

  if (!rect.contains(global_pos) && entered) {
    entered = false;
    setWindowOpacity(property("dc_orig_opacity").toReal());
  }

  setProperty("dc_mouse_entered", entered);
}
#endif

void ClockWindow::setStayOnTop(bool en)
{
  _should_stay_on_top = en;
  wrap_set_window_flag(this, Qt::WindowStaysOnTopHint, en);
}

void ClockWindow::setTransparentForInput(bool en)
{
  wrap_set_window_flag(this, Qt::WindowTransparentForInput, en);
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
    updateLastOrigin();
    _is_dragging = false;
    event->accept();
  }
}

void ClockWindow::moveEvent(QMoveEvent* event)
{
  QWidget::moveEvent(event);
  if (!isVisible()) return;
  if (_keep_visible) updateWindowPos();
}

void ClockWindow::resizeEvent(QResizeEvent* event)
{
  QWidget::resizeEvent(event);
  _overlay->resize(event->size());
  _overlay->raise();  // to make sure that it is always on top

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

void ClockWindow::updateLastOrigin()
{
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
}

void ClockWindow::updateWindowPos()
{
  auto tpos = pos();

  auto sg = screen()->geometry();

  if (tpos.x() < sg.left())
    tpos.setX(sg.left());
  if (tpos.x() + width() > sg.right())
    tpos.setX(sg.right() - width());

  if (tpos.y() < sg.top())
    tpos.setY(sg.top());
  if (tpos.y() + height() > sg.bottom())
    tpos.setY(sg.bottom() - height());

  if (tpos != pos()) {
    move(tpos);
    updateLastOrigin();
  }
}

void ClockWindow::addPositionMenu()
{
  auto pos_menu = _ctx_menu->addMenu(tr("Position"));
  auto t_menu = pos_menu->addMenu(tr("Top"));
  t_menu->addAction(tr("Left"),   this, [this]() { moveToPredefinedPos(Qt::AlignTop | Qt::AlignLeft); });
  t_menu->addAction(tr("Middle"), this, [this]() { moveToPredefinedPos(Qt::AlignTop | Qt::AlignHCenter); });
  t_menu->addAction(tr("Right"),  this, [this]() { moveToPredefinedPos(Qt::AlignTop | Qt::AlignRight); });
  auto m_menu = pos_menu->addMenu(tr("Middle"));
  m_menu->addAction(tr("Left"),   this, [this]() { moveToPredefinedPos(Qt::AlignVCenter | Qt::AlignLeft); });
  m_menu->addAction(tr("Middle"), this, [this]() { moveToPredefinedPos(Qt::AlignVCenter | Qt::AlignHCenter); });
  m_menu->addAction(tr("Right"),  this, [this]() { moveToPredefinedPos(Qt::AlignVCenter | Qt::AlignRight); });
  auto b_menu = pos_menu->addMenu(tr("Bottom"));
  b_menu->addAction(tr("Left"),   this, [this]() { moveToPredefinedPos(Qt::AlignBottom | Qt::AlignLeft); });
  b_menu->addAction(tr("Middle"), this, [this]() { moveToPredefinedPos(Qt::AlignBottom | Qt::AlignHCenter); });
  b_menu->addAction(tr("Right"),  this, [this]() { moveToPredefinedPos(Qt::AlignBottom | Qt::AlignRight); });
}
