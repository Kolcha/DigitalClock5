// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "clock_window.hpp"

#include <QApplication>

#include <QGraphicsColorizeEffect>
#include <QGridLayout>
#include <QMenu>
#include <QScreen>

#include <QPainter>

#include <QContextMenuEvent>
#include <QMouseEvent>

#include "config/settings.hpp"
#include "platform/clock_native_window.hpp"

#include "clock_widget.hpp"

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

namespace {

static constexpr const char* const ORIGIN_POS_KEY = "origin_pos";
static constexpr const char* const ANCHOR_POINT_KEY = "anchor_point";
static constexpr const char* const VISIBILITY_KEY = "visible";

} // namespace

ClockWindow::ClockWindow(QWidget* parent)
  : QWidget(parent)
  , _clock(new ClockWidget(this))
  , _ctx_menu(new QMenu(this))
  , _data(std::make_unique<PlatformData>())
{
  setWindowFlags(Qt::FramelessWindowHint);
#ifdef Q_OS_MACOS
  setWindowFlag(Qt::NoDropShadowWindowHint);
#endif
#ifdef Q_OS_WINDOWS
  setWindowFlag(Qt::Tool);
#endif
  setAttribute(Qt::WA_TranslucentBackground);
  platformOneTimeFlags();

  _ctx_menu->addAction(QIcon(":/icons/configure.svg"), tr("Settings"),
                       this, &ClockWindow::settingsDialogRequested);
  addPositionMenu();
  _ctx_menu->addSeparator();
  _ctx_menu->addAction(QIcon(":/icons/help-about.svg"), tr("About"),
                       this, &ClockWindow::aboutDialogRequested);
  _ctx_menu->addSeparator();
  _ctx_menu->addAction(QIcon(":/icons/application-exit.svg"), tr("Quit"),
                       this, &ClockWindow::appExitRequested);

  auto l = new QGridLayout(this);
  l->addWidget(_clock);

  l->setSizeConstraint(QLayout::SetFixedSize);
}

ClockWindow::~ClockWindow() = default;

void ClockWindow::saveState(StateStorage& st) const
{
  st.setValue(ORIGIN_POS_KEY, _last_origin);
  st.setValue(ANCHOR_POINT_KEY, _anchor_point);
  st.setValue(VISIBILITY_KEY, isVisible());
}

void ClockWindow::loadState(const StateStorage& st)
{
  _last_origin = st.value(ORIGIN_POS_KEY, _last_origin).toPoint();
  _anchor_point = st.value(ANCHOR_POINT_KEY, _anchor_point).value<AnchorPoint>();
  setVisible(st.value(VISIBILITY_KEY, true).toBool());
}

void ClockWindow::setAnchorPoint(AnchorPoint ap)
{
  _anchor_point = ap;
  updateLastOrigin();
}

void ClockWindow::moveToPredefinedPos(Qt::Alignment a)
{
  auto tpos = pos();

  auto sg = screen()->availableGeometry();
  auto wg = frameGeometry();

  if (a & Qt::AlignLeft)
    tpos.setX(sg.left());
  if (a & Qt::AlignHCenter)
    tpos.setX(sg.center().x() - wg.width()/2);
  if (a & Qt::AlignRight)
    tpos.setX(sg.right() - wg.width());

  if (a & Qt::AlignTop)
    tpos.setY(sg.top());
  if (a & Qt::AlignVCenter)
    tpos.setY(sg.center().y() - wg.height()/2);
  if (a & Qt::AlignBottom)
    tpos.setY(sg.bottom() - wg.height());

  move(tpos);
  updateLastOrigin();
}

void ClockWindow::setStayOnTop(bool enabled)
{
  wrap_set_window_flag(this, Qt::WindowStaysOnTopHint, enabled);
  platformStayOnTop(enabled);
}

void ClockWindow::setFullscreenDetectEnabled(bool enabled)
{
  platformFullScreenDetect(enabled);
}

void ClockWindow::setTransparentForMouse(bool enabled)
{
  wrap_set_window_flag(this, Qt::WindowTransparentForInput, enabled);
}

void ClockWindow::setSnapToEdgeEnabled(bool enabled)
{
  _snap_to_edge = enabled;
}

void ClockWindow::setSnapThreshold(int threshold)
{
  _snap_threshold = threshold;
}

void ClockWindow::setPreventOutOfScreenEnabled(bool enabled)
{
  _keep_visible = enabled;
  preventOutOfScreenPos();
}

void ClockWindow::setChangeCursorOnHoverEnabled(bool enabled)
{
  _change_cursor_on_hover = enabled;
  setCursor(enabled ? Qt::OpenHandCursor : Qt::ArrowCursor);
}

void ClockWindow::setTransparentOnHoverEnabled(bool enabled)
{
  _transparent_on_hover = enabled;
}

void ClockWindow::setOpacityOnHover(qreal opacity)
{
  _opacity_on_hover = opacity;
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
void ClockWindow::setFrameVisible(bool vis)
{
  _frame_visible = vis;
  if (_frame_visible)
    setContentsMargins(contentsMargins() + QMargins(2, 2, 2, 2));
  else
    setContentsMargins(contentsMargins() - QMargins(2, 2, 2, 2));
  update();
}

void ClockWindow::setColorizationEnabled(bool enabled)
{
  if (enabled) {
    auto e = new QGraphicsColorizeEffect;
    e->setColor(_colorization_color);
    e->setStrength(_colorization_strength);
    setGraphicsEffect(e);
  } else {
    setGraphicsEffect(nullptr);
  }
}

void ClockWindow::setColorizationColor(const QColor& color)
{
  if (auto e = qobject_cast<QGraphicsColorizeEffect*>(graphicsEffect()))
    e->setColor(color);
  _colorization_color = color;
}

void ClockWindow::setColorizationStrength(qreal s)
{
  if (auto e = qobject_cast<QGraphicsColorizeEffect*>(graphicsEffect()))
    e->setStrength(s);
  _colorization_strength = s;
}

void ClockWindow::tick()
{
  platformTick();
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
    if (_change_cursor_on_hover) setCursor(Qt::ClosedHandCursor);
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
    if (_change_cursor_on_hover) setCursor(Qt::OpenHandCursor);
    event->accept();
  }
}

void ClockWindow::moveEvent(QMoveEvent* event)
{
  QWidget::moveEvent(event);
  if (!isVisible()) return;
  if (!_is_dragging) preventOutOfScreenPos();
}

void ClockWindow::resizeEvent(QResizeEvent* event)
{
  QWidget::resizeEvent(event);

  // do not apply anhoring logic during dragging
  if (_is_dragging) return;

  // state restore must happen before the first resize event!
  // previous origin and correct anchor point must be known here
  auto curr_origin = anchoredOrigin();
  move(pos() + _last_origin - curr_origin);
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

void ClockWindow::addPositionMenu()
{
  // *INDENT-OFF*
  auto pos_menu = _ctx_menu->addMenu(tr("Position"));
  auto t_menu = pos_menu->addMenu(QIcon(":/icons/align/top.svg"), tr("Top"));
  t_menu->addAction(QIcon(":/icons/align/top-left.svg"), tr("Left"), this,
                    [this]() { moveToPredefinedPos(Qt::AlignTop | Qt::AlignLeft); });
  t_menu->addAction(QIcon(":/icons/align/top.svg"), tr("Middle"), this,
                    [this]() { moveToPredefinedPos(Qt::AlignTop | Qt::AlignHCenter); });
  t_menu->addAction(QIcon(":/icons/align/top-right.svg"), tr("Right"), this,
                    [this]() { moveToPredefinedPos(Qt::AlignTop | Qt::AlignRight); });
  auto m_menu = pos_menu->addMenu(QIcon(":/icons/align/v-center.svg"), tr("Middle"));
  m_menu->addAction(QIcon(":/icons/align/left.svg"), tr("Left"), this,
                    [this]() { moveToPredefinedPos(Qt::AlignVCenter | Qt::AlignLeft); });
  m_menu->addAction(QIcon(":/icons/align/h-center.svg"), tr("Middle"), this,
                    [this]() { moveToPredefinedPos(Qt::AlignVCenter | Qt::AlignHCenter); });
  m_menu->addAction(QIcon(":/icons/align/right.svg"), tr("Right"), this,
                    [this]() { moveToPredefinedPos(Qt::AlignVCenter | Qt::AlignRight); });
  auto b_menu = pos_menu->addMenu(QIcon(":/icons/align/bottom.svg"), tr("Bottom"));
  b_menu->addAction(QIcon(":/icons/align/bottom-left.svg"), tr("Left"), this,
                    [this]() { moveToPredefinedPos(Qt::AlignBottom | Qt::AlignLeft); });
  b_menu->addAction(QIcon(":/icons/align/bottom.svg"), tr("Middle"), this,
                    [this]() { moveToPredefinedPos(Qt::AlignBottom | Qt::AlignHCenter); });
  b_menu->addAction(QIcon(":/icons/align/bottom-right.svg"), tr("Right"), this,
                    [this]() { moveToPredefinedPos(Qt::AlignBottom | Qt::AlignRight); });
  // *INDENT-ON*
}

QPoint ClockWindow::anchoredOrigin() const
{
  auto curr_origin = _clock->mapToGlobal(_clock->origin());
  auto wnd_rect = frameGeometry();

  switch (_anchor_point) {
    case AnchorLeft:
      curr_origin.setX(wnd_rect.x());
      break;
    case AnchorCenter:
      curr_origin.setX(wnd_rect.center().x());
      break;
    case AnchorRight:
      curr_origin.setX(wnd_rect.x() + wnd_rect.width());
      break;
  }

  return curr_origin;
}

void ClockWindow::updateLastOrigin()
{
  _last_origin = anchoredOrigin();
  emit saveStateRequested();
}

void ClockWindow::preventOutOfScreenPos()
{
  if (!_keep_visible)
    return;

  auto tpos = pos();

  auto sg = screen()->geometry();
  auto wg = frameGeometry();

  if (tpos.x() < sg.left())
    tpos.setX(sg.left());
  if (tpos.x() + wg.width() > sg.right())
    tpos.setX(sg.right() - wg.width());

  if (tpos.y() < sg.top())
    tpos.setY(sg.top());
  if (tpos.y() + wg.height() > sg.bottom())
    tpos.setY(sg.bottom() - wg.height());

  if (tpos != pos()) {
    move(tpos);
    updateLastOrigin();
  }
}
