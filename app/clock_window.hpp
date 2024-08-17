/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include <QGridLayout>

#include "skin/graphics_widgets.hpp"
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

  void addPluginWidget(GraphicsWidgetBase* w, int row, int column,
                       int row_span = 1, int column_span = 1);

  void saveState(State& s) const;
  void loadState(const State& s);

public slots:
  void setAnchorPoint(AnchorPoint ap);
  void setOriginPoint(QPoint origin);

  void enableSnapToEdge() { setSnapToEdge(true, snapThreshold()); }
  void disableSnapToEdge() { setSnapToEdge(false, snapThreshold()); }
  void setSnapToEdge(bool enable, int threshold);
  void setSnapThreshold(int thr) { setSnapToEdge(snapToEdge(), thr); }

  void setScaling(qreal sx, qreal sy);

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
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

private:
  AnchorPoint _anchor_point = AnchorLeft;
  QPoint _last_origin = {400, 400};

  QGridLayout* _layout = nullptr;
  GraphicsDateTimeWidget* _clock = nullptr;

  bool _is_dragging = false;
  QPoint _drag_pos;

  bool _snap_to_edge = true;
  int _snap_threshold = 15;

  qreal _sx = 1.0;
  qreal _sy = 1.0;

  QMenu* _ctx_menu;
};
