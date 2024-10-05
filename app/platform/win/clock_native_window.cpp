/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "clock_native_window.hpp"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

#include "fullscreen_detect.hpp"

namespace dc = digital_clock;

// should be called once if stay on top is disabled
// window must be visible when this is called
static void surviveWinDHack(WId winId)
{
  // keep window visible after Win+D (show desktop)
  // https://stackoverflow.com/questions/35045060/how-to-keep-window-visible-at-all-times-but-not-force-it-to-be-on-top
  HWND hWndTmp = FindWindowEx(NULL, NULL, L"Progman", NULL);
  if (hWndTmp) {
    hWndTmp = FindWindowEx(hWndTmp, NULL, L"SHELLDLL_DefView", NULL);
    if (hWndTmp) {
      SetWindowLongPtr((HWND)winId, GWLP_HWNDPARENT, (LONG_PTR)hWndTmp);
    }
  }
}

static void SetSurviveWinDHackEnabled(WId winId, bool en)
{
  if (en)
    surviveWinDHack(winId);
  else
    SetWindowLongPtr((HWND)winId, GWLP_HWNDPARENT, (LONG_PTR)0);
}


ClockNativeWindow::ClockNativeWindow(QWidget* parent)
    : ClockWindow(parent)
{
}

void ClockNativeWindow::setStayOnTop(bool en)
{
  ClockWindow::setStayOnTop(en);

  SetSurviveWinDHackEnabled(winId(), !en);
  runStayOnTopHacks();
}

void ClockNativeWindow::runStayOnTopHacks()
{
  // calling winId() for invisible window causes move event from (0,0) to (289,160)
  // during startup (doesn't matter what saved coordinates were, each time the same),
  // as result real saved position will be overwritten even before it will be read
  // so do nothing if window is not visible
  if (!isVisible()) return;

  // unfortunately, fullscreen invisible (transparent) windows may exist... we should ignore them
  // remember all fullscreen windows on app startup or window's screen change and ignore them later
  if (_last_screen != screen()) {
    _last_screen = screen();
    _fullscreen_ignore_list = dc::GetFullscreenWindowsOnSameMonitor(winId());
  }

  if (!_should_stay_on_top) return;

  if (_detect_fullscreen && dc::IsFullscreenWndOnSameMonitor(winId(), _fullscreen_ignore_list)) {
    // don't stay above fullscreen windows
    if ((GetWindowLongPtr((HWND)winId(), GWL_EXSTYLE) & WS_EX_TOPMOST) != 0) {
      SetWindowPos((HWND)winId(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
    }
  } else {
    // always on top problem workaround
    // sometimes Qt somehow loses Qt::WindowStaysOnTopHint window flag, so set it again
    if (!(windowFlags() & Qt::WindowStaysOnTopHint)) {
      setStayOnTop(true);
    }
    // sometimes even window have Qt::WindowStaysOnTopHint window flag, it doesn't have WS_EX_TOPMOST flag,
    // so set it manually using WinAPI...
    if ((GetWindowLongPtr((HWND)winId(), GWL_EXSTYLE) & WS_EX_TOPMOST) == 0) {
      SetWindowPos((HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
    }
    // keep above the taskbar workaround
    // https://forum.qt.io/topic/28739/flags-windows-7-window-always-on-top-including-the-win7-taskbar-custom-error/4
    if (!isActiveWindow()) raise();
  }
}
