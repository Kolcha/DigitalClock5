# SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

add_subdirectory(GradientDialog)
add_subdirectory(paletteicon)

set(QT_DEFAULT_MAJOR_VERSION 6 CACHE STRING "Qt version for QHotkey")
set(QHOTKEY_INSTALL OFF CACHE BOOL "disable QHotkey install rule")

add_subdirectory(QHotkey)

# on Linux -fPIC became required for this static library for some reason...
set_property(TARGET gradient_dialog PROPERTY POSITION_INDEPENDENT_CODE ON)
