# SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

include(FetchContent)

FetchContent_Declare(
  QHotkey
  GIT_REPOSITORY https://github.com/Skycoder42/QHotkey.git
  GIT_TAG        4e3a244d87f1f7e741e1395f2ffe825f3a8ada45
)

FetchContent_MakeAvailable(QHotkey)
