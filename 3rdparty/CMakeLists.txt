# SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

add_subdirectory(GradientDialog)
add_subdirectory(paletteicon)

set(QT_DEFAULT_MAJOR_VERSION 6 CACHE STRING "Qt version for QHotkey")

add_subdirectory(QHotkey)
