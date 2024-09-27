/*
 * SPDX-FileCopyrightText: 2020-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "system_theme_tracker.hpp"

#include <Windows.h>

struct SystemThemeTracker::Impl {
  bool is_light_theme = false;
  bool should_stop = false;
  HKEY opened_key = NULL;
  HANDLE reg_event = NULL;
};

SystemThemeTracker::SystemThemeTracker(QObject* parent)
  : QThread(parent)
  , _impl(std::make_unique<Impl>())
{
  RegOpenKeyEx(HKEY_CURRENT_USER,
               TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"),
               0,
               KEY_QUERY_VALUE | KEY_NOTIFY,
               &_impl->opened_key);
  _impl->is_light_theme = isLightTheme();
  _impl->reg_event = CreateEvent(NULL, FALSE, FALSE, NULL);
}

SystemThemeTracker::~SystemThemeTracker()
{
  RegCloseKey(_impl->opened_key);
  CloseHandle(_impl->reg_event);
}

bool SystemThemeTracker::isLightTheme() const
{
  unsigned char buffer[sizeof(DWORD)];
  DWORD size = sizeof(buffer);
  LONG ec = RegQueryValueEx(_impl->opened_key,
                            TEXT("SystemUsesLightTheme"),
                            NULL, NULL,
                            (LPBYTE)buffer, &size);
  DWORD value = 0;
  if (ec == ERROR_SUCCESS)
    value = *reinterpret_cast<DWORD*>(buffer);
  return value != 0;
}

void SystemThemeTracker::start()
{
  QThread::start(QThread::LowPriority);
}

void SystemThemeTracker::stop()
{
  _impl->should_stop = true;
  SetEvent(_impl->reg_event);
}

void SystemThemeTracker::run()
{
  while (!_impl->should_stop) {
    if (!_impl->reg_event)
      break;

    LONG ec = RegNotifyChangeKeyValue(_impl->opened_key,
                                      FALSE,
                                      REG_NOTIFY_CHANGE_LAST_SET,
                                      _impl->reg_event, TRUE);
    if (ec != ERROR_SUCCESS)
      break;

    if (WaitForSingleObject(_impl->reg_event, INFINITE) != WAIT_OBJECT_0)
      break;

    bool is_light_theme = isLightTheme();
    if (is_light_theme != _impl->is_light_theme) {
      _impl->is_light_theme = is_light_theme;
      emit themeChanged(is_light_theme);
    }
  }
  _impl->should_stop = false;
}
