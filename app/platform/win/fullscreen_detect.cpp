/*
 * SPDX-FileCopyrightText: 2017-2020 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "fullscreen_detect.hpp"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

namespace digital_clock {

typedef BOOL (*enum_fullscreen_proc)(HWND, void*);

struct enum_fullscreen_data_t {
  HMONITOR monitor;
  LONG width;
  LONG height;
  enum_fullscreen_proc proc_func;
  void* proc_data;
};

// parts of solution was found there:
// http://stackoverflow.com/questions/3797802/how-to-check-if-an-other-program-is-running-in-fullscreen-mode-eg-a-media-play
// http://stackoverflow.com/questions/7009080/detecting-full-screen-mode-in-windows

static BOOL CALLBACK CheckFullscreen(HWND hwnd, LPARAM lParam)
{
  enum_fullscreen_data_t* data = (enum_fullscreen_data_t*)lParam;
  // skip windows from other monitors
  HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
  if (hMonitor != data->monitor) return TRUE;

  // skip desktop window
  if (hwnd == GetDesktopWindow() || hwnd == GetShellWindow()) return TRUE;

  // skip any invisible windows (Windows has a lot of them! Ugh...)
  if (!(GetWindowLongPtr(hwnd, GWL_STYLE) & WS_VISIBLE)) return TRUE;

  RECT r;
  if (!GetWindowRect(hwnd, &r)) return TRUE;

  if ((r.right - r.left == data->width) && (r.bottom - r.top == data->height))
    return data->proc_func(hwnd, data->proc_data);

  return TRUE;
}

static void EnumFullscreenWindowsOnSameScreen(HWND hwnd, enum_fullscreen_proc proc_func, void* proc_data)
{
  enum_fullscreen_data_t data;
  data.monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

  MONITORINFO info;
  info.cbSize = sizeof(MONITORINFO);
  if (GetMonitorInfo(data.monitor, &info)) {
    data.width = info.rcMonitor.right - info.rcMonitor.left;
    data.height = info.rcMonitor.bottom - info.rcMonitor.top;
  }

  data.proc_func = proc_func;
  data.proc_data = proc_data;

  EnumWindows(CheckFullscreen, (LPARAM)(&data));
}


static QString GetWindowClassName(HWND hwnd)
{
  WCHAR class_name[256];
  GetClassName(hwnd, class_name, 256);
  return QString::fromWCharArray(class_name);
}


struct is_fullscreen_data_t {
  QSet<QString> ignore_list;
  bool fullscreen_found;
};

static BOOL IsFullscreenProc(HWND hwnd, void* data)
{
  is_fullscreen_data_t* fdata = reinterpret_cast<is_fullscreen_data_t*>(data);

  if (fdata->ignore_list.contains(GetWindowClassName(hwnd)))
    return TRUE;

  fdata->fullscreen_found = true;
  return FALSE;
}

bool IsFullscreenWndOnSameMonitor(WId wid, const QSet<QString>& ignore_list)
{
  is_fullscreen_data_t proc_data;
  proc_data.ignore_list = ignore_list;
  proc_data.fullscreen_found = false;

  EnumFullscreenWindowsOnSameScreen((HWND)wid, &IsFullscreenProc, &proc_data);

  return proc_data.fullscreen_found;
}


static BOOL StoreFullscreenProc(HWND hwnd, void* data)
{
  reinterpret_cast<QSet<QString>*>(data)->insert(GetWindowClassName(hwnd));
  return TRUE;
}

QSet<QString> GetFullscreenWindowsOnSameMonitor(WId wid)
{
  QSet<QString> fullscreen_list;
  EnumFullscreenWindowsOnSameScreen((HWND)wid, &StoreFullscreenProc, &fullscreen_list);
  return fullscreen_list;
}

} // namespace digital_clock
