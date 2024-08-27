/*
 * SPDX-FileCopyrightText: 2020-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mouse_tracker.hpp"

#include <Windows.h>

class MouseTrackerPrivate : public QObject
{
  Q_OBJECT

public:
  explicit MouseTrackerPrivate(QObject* parent = nullptr);
  ~MouseTrackerPrivate();

  LRESULT HandleMouseEvent(int nCode, WPARAM wParam, LPARAM lParam);

signals:
  void mousePosChanged(QPoint pos, Qt::KeyboardModifiers m);

private:
  HHOOK hHook = NULL;
};


static MouseTrackerPrivate* g_pdata = nullptr;

static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (g_pdata)
    return g_pdata->HandleMouseEvent(nCode, wParam, lParam);
  else
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


MouseTrackerPrivate::MouseTrackerPrivate(QObject* parent)
  : QObject(parent)
{
  g_pdata = this;
  hHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
}

MouseTrackerPrivate::~MouseTrackerPrivate()
{
  UnhookWindowsHookEx(hHook);
  g_pdata = nullptr;
}

LRESULT MouseTrackerPrivate::HandleMouseEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (wParam == WM_MOUSEMOVE) {
    MSLLHOOKSTRUCT* s = (MSLLHOOKSTRUCT*)lParam;
    Qt::KeyboardModifiers m = Qt::NoModifier;
    if (GetAsyncKeyState(VK_SHIFT) != 0)
      m |= Qt::ShiftModifier;
    if (GetAsyncKeyState(VK_CONTROL) != 0)
      m |= Qt::ControlModifier;
    if (GetAsyncKeyState(VK_MENU) != 0)
      m |= Qt::AltModifier;
    emit mousePosChanged({s->pt.x, s->pt.y}, m);
  }
  return CallNextHookEx(hHook, nCode, wParam, lParam);
}


MouseTracker::MouseTracker(QObject* parent)
  : QObject(parent)
  , m_impl(std::make_unique<MouseTrackerPrivate>())
{
  connect(m_impl.get(), &MouseTrackerPrivate::mousePosChanged, this, &MouseTracker::mousePositionChanged);
}

MouseTracker::~MouseTracker() = default;

#include "mouse_tracker.moc"
