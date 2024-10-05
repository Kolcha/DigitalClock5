/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include <QGridLayout>

#include "skin/graphics_widgets.hpp"
#include "overlay_widget.hpp"
#include "state.hpp"

class ClockWindow : public QWidget
{
  Q_OBJECT

public:
  enum AnchorPoint { AnchorLeft, AnchorCenter, AnchorRight };
  Q_ENUM(AnchorPoint)

  explicit ClockWindow(QWidget* parent = nullptr);

  AnchorPoint anchorPoint() const { return _anchor_point; }
  QPoint originPoint() const { return _last_origin; }

  GraphicsDateTimeWidget* clock() const { return _clock; }

  bool snapToEdge() const { return _snap_to_edge; }
  int snapThreshold() const { return _snap_threshold; }

  bool keepVisible() const { return _keep_visible; }

  bool frameVisible() const { return _overlay->frameVisible(); }

  bool transparentOnHover() const { return _transparent_on_hover; }
  qreal opacityOnHover() const { return _opacity_on_hover; }

  void addPluginWidget(GraphicsWidgetBase* w, int row, int column,
                       int row_span = 1, int column_span = 1);

  void saveState(State& s) const;
  void loadState(const State& s);

public slots:
  void moveToPredefinedPos(Qt::Alignment a);

  void setAnchorPoint(AnchorPoint ap);
  void setOriginPoint(QPoint origin);

  void enableSnapToEdge() { setSnapToEdge(true, snapThreshold()); }
  void disableSnapToEdge() { setSnapToEdge(false, snapThreshold()); }
  void setSnapToEdge(bool enable, int threshold);
  void setSnapThreshold(int thr) { setSnapToEdge(snapToEdge(), thr); }

  void setKeepVisible(bool en);

  void setScaling(qreal sx, qreal sy);

  void enableFrame() { setFrameVisible(true); }
  void disableFrame() { setFrameVisible(false); }
  void setFrameVisible(bool vis);

  void setTransparentOnHover(bool en);
  void setOpacityOnHover(qreal o);

#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
  // if window is transparent for input, no input events are sent
  // even to application, so mouse should be tracked using platform API
  // handler should be public to keep only one instance of that tracker
  void handleMouseMove(const QPoint& global_pos);
#endif
  virtual void setFullscreenDetect(bool en) { _detect_fullscreen = en; }

  virtual void setStayOnTop(bool en);
  virtual void setTransparentForInput(bool en);

signals:
  void saveStateRequested();

  void settingsDialogRequested();
  void aboutDialogRequested();
  void appExitRequested();

protected:
  void contextMenuEvent(QContextMenuEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void moveEvent(QMoveEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

private:
  void updateLastOrigin();
  void updateWindowPos();
  void addPositionMenu();

private:
  AnchorPoint _anchor_point = AnchorLeft;
  QPoint _last_origin = {80, 120};

  QGridLayout* _layout = nullptr;
  GraphicsDateTimeWidget* _clock = nullptr;
  OverlayWidget* _overlay = nullptr;

  bool _is_dragging = false;
  QPoint _drag_pos;

  bool _snap_to_edge = true;
  int _snap_threshold = 15;

  bool _keep_visible = true;

  qreal _sx = 1.0;
  qreal _sy = 1.0;

  QMenu* _ctx_menu;

  bool _transparent_on_hover = true;
  qreal _opacity_on_hover = 0.15;

protected:
  // used by stay on top hacks, but types are not platform-dependent
  QScreen* _last_screen = nullptr;
  QSet<QString> _fullscreen_ignore_list;
  bool _detect_fullscreen = true;
  bool _should_stay_on_top = true;
};
