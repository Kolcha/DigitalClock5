# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(skins_archive "${CMAKE_CURRENT_BINARY_DIR}/skins.zip")

if (NOT EXISTS "${skins_archive}")
    message("downloading skins archive...")
    file(DOWNLOAD "https://digitalclock4.sourceforge.io/media/skins.zip" "${skins_archive}" SHOW_PROGRESS)
endif()

if (NOT EXISTS "${CMAKE_BINARY_DIR}/skins")
    file(ARCHIVE_EXTRACT INPUT "${skins_archive}" DESTINATION "${CMAKE_BINARY_DIR}")
endif()

if (WIN32)
  add_subdirectory(msi)
endif()
