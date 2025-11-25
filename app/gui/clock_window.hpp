// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWidget>

class ClockWidget;
class OverlayWidget;
class StateStorage;

void wrap_set_window_flag(QWidget* wnd, Qt::WindowType flag, bool set);

class ClockWindow : public QWidget
{
  Q_OBJECT

public:
  enum AnchorPoint { AnchorLeft, AnchorCenter, AnchorRight };
  Q_ENUM(AnchorPoint)

  explicit ClockWindow(QWidget* parent = nullptr);
  ~ClockWindow();

  ClockWidget* clock() const noexcept { return _clock; }

  AnchorPoint anchorPoint() const noexcept { return _anchor_point; }

  void saveState(StateStorage& st) const;
  void loadState(const StateStorage& st);

public slots:
  void setAnchorPoint(AnchorPoint ap);

  void moveToPredefinedPos(Qt::Alignment a);

  void setStayOnTop(bool enabled);
  void setFullscreenDetectEnabled(bool enabled);
  void setTransparentForMouse(bool enabled);

  void setSnapToEdgeEnabled(bool enabled);
  void setSnapThreshold(int threshold);
  void setPreventOutOfScreenEnabled(bool enabled);

  void setChangeCursorOnHoverEnabled(bool enabled);

  void setTransparentOnHoverEnabled(bool enabled);
  void setOpacityOnHover(qreal opacity);
#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
  // if window is transparent for input, no input events are sent
  // even to application, so mouse should be tracked using platform API
  // handler should be public to keep only one instance of that tracker
  void handleMouseMove(const QPoint& global_pos);
#endif
  void enableFrame() { setFrameVisible(true); }
  void disableFrame() { setFrameVisible(false); }
  void setFrameVisible(bool vis);

  void setColorizationEnabled(bool enabled);
  inline void enableColorization() { setColorizationEnabled(true); }
  inline void disableColorization() { setColorizationEnabled(false); }

  void setColorizationColor(const QColor& color);
  void setColorizationStrength(qreal s);

  void tick();

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

  void platformOneTimeFlags();
  void platformTick();

  void platformStayOnTop(bool enabled);
  void platformFullScreenDetect(bool enabled);

private:
  void addPositionMenu();

  QPoint anchoredOrigin() const;
  QPoint desiredPosition() const;

  void preventOutOfScreenPos();

private:
  ClockWidget* _clock;
  QMenu* _ctx_menu;
  OverlayWidget* _overlay = nullptr;

  AnchorPoint _anchor_point = AnchorLeft;
  QPoint _last_origin = {80, 120};

  bool _is_dragging = false;
  QPoint _drag_pos;

  bool _snap_to_edge = true;
  int _snap_threshold = 15;

  bool _keep_visible = true;

  bool _change_cursor_on_hover = false;

  bool _transparent_on_hover = false;
  qreal _opacity_on_hover = 0.15;

  QColor _colorization_color = QColor(85, 0, 255);
  qreal _colorization_strength = 0.8;

  struct PlatformData;
  std::unique_ptr<PlatformData> _data;
};
