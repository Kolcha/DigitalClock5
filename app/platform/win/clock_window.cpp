/*
 * SPDX-FileCopyrightText: 2017-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "gui/clock_window.hpp"

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

void ClockWindow::runStayOnTopHacks()
{
  if (!_should_stay_on_top) return;

  // unfortunately, fullscreen invisible (transparent) windows may exist... we should ignore them
  // remember all fullscreen windows on app startup or window's screen change and ignore them later
  if (_last_screen != screen()) {
      _last_screen = screen();
      _fullscreen_ignore_list = dc::GetFullscreenWindowsOnSameMonitor(winId());
    }

  if (_detect_fullscreen && dc::IsFullscreenWndOnSameMonitor(winId(), _fullscreen_ignore_list)) {
    // don't stay above fullscreen windows
    if ((GetWindowLongPtr((HWND)winId(), GWL_EXSTYLE) & WS_EX_TOPMOST) != 0) {
      SetWindowPos((HWND)winId(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
    }
  } else {
    // always on top problem workaround
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

void ClockWindow::setStayOnTop(bool en)
{
  _should_stay_on_top = en;
  HWND pos = en ? HWND_TOPMOST : HWND_NOTOPMOST;
  SetWindowPos((HWND)winId(), pos, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
  SetSurviveWinDHackEnabled(winId(), !en);
  runStayOnTopHacks();
}

void ClockWindow::setTransparentForInput(bool en)
{
  LONG_PTR st = GetWindowLongPtr((HWND)winId(), GWL_EXSTYLE);
  if (en)
    st |= WS_EX_TRANSPARENT;
  else
    st &= ~WS_EX_TRANSPARENT;
  SetWindowLongPtr((HWND)winId(), GWL_EXSTYLE, (LONG_PTR)st);
}
