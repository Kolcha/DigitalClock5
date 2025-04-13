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

namespace {

// postpone applying window flags for a few ticks
// it is known that even isVisible() returns true,
// manipulations on winId() may have no effect...
static constexpr const int max_ticks = 2;

} // namespace

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

static QPoint wndPosWinAPI(WId winId)
{
  RECT r;
  GetWindowRect((HWND)winId, &r);
  return {r.left, r.top};
}

void ClockWindow::platformOneTimeFlags()
{
}

void ClockWindow::platformTick()
{
  // calling winId() for invisible window causes move event from (0,0) to (289,160)
  // during startup (doesn't matter what saved coordinates were, each time the same),
  // as result real saved position will be overwritten even before it will be read
  // so do nothing if window is not visible
  if (!isVisible()) return;

  // unfortunately, fullscreen invisible (transparent) windows may exist... we should ignore them
  // remember all fullscreen windows on app startup or window's screen change and ignore them later
  if (_data->last_screen != screen()) {
    _data->last_screen = screen();
    _data->fullscreen_ignore_list = dc::GetFullscreenWindowsOnSameMonitor(winId());
  }

  if (_data->should_stay_on_top) {
    // disable hack required to survive Win+D
    if (GetWindowLongPtr((HWND)winId(), GWLP_HWNDPARENT) != 0) {
      SetSurviveWinDHackEnabled(winId(), false);
    }
    if (_data->detect_fullscreen && dc::IsFullscreenWndOnSameMonitor(winId(), _data->fullscreen_ignore_list)) {
      // don't stay above fullscreen windows
      if (windowFlags() & Qt::WindowStaysOnTopHint) {
        wrap_set_window_flag(this, Qt::WindowStaysOnTopHint, false);
        lower();
      }
    } else {
      // always on top problem workaround
      // sometimes Qt somehow loses Qt::WindowStaysOnTopHint window flag, so set it again
      if (!(windowFlags() & Qt::WindowStaysOnTopHint)) {
        wrap_set_window_flag(this, Qt::WindowStaysOnTopHint, true);
      }
      // sometimes even window have Qt::WindowStaysOnTopHint window flag, it doesn't have WS_EX_TOPMOST flag,
      // so set it manually using WinAPI...
      if (!(GetWindowLongPtr((HWND)winId(), GWL_EXSTYLE) & WS_EX_TOPMOST)) {
        SetWindowPos((HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
      }
      // https://forum.qt.io/topic/28739/flags-windows-7-window-always-on-top-including-the-win7-taskbar-custom-error/4
      if (!isActiveWindow()) raise();
    }
  } else {
    // ensure that window doesn't have WS_EX_TOPMOST flag
    if (GetWindowLongPtr((HWND)winId(), GWL_EXSTYLE) & WS_EX_TOPMOST) {
      SetWindowPos((HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
    }
    if (_data->should_apply_win_d_hack && _data->ticks_count++ >= max_ticks) {
      _data->should_apply_win_d_hack = false;
      _data->ticks_count = 0;
      SetSurviveWinDHackEnabled(winId(), true);
    }
  }

  // there were reports that clock position is wrong on startup or after state restore
  // there is no obvious explanation for this, the only thing is some Windows quirks...
  // just detect possible "position mismatch" case and move the window explicitly
  // https://github.com/Kolcha/DigitalClock5/discussions/32
  if (auto dpos = desiredPosition(); (wndPosWinAPI(winId()) / devicePixelRatio() != dpos) && !_is_dragging) {
    move(dpos);
  }
}

void ClockWindow::platformStayOnTop(bool enabled)
{
  _data->should_stay_on_top = enabled;
  _data->should_apply_win_d_hack = !enabled;
}

void ClockWindow::platformFullScreenDetect(bool enabled)
{
  _data->detect_fullscreen = enabled;
}
