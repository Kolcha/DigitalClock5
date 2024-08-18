# SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(WIN_ICO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/digital_clock.ico")
    set(BUILD_NUMBER 0)
    if (DEFINED ENV{GITHUB_RUN_NUMBER})
        set(BUILD_NUMBER $ENV{GITHUB_RUN_NUMBER})
    endif()
    configure_file(digital_clock.rc.in digital_clock.rc NEWLINE_STYLE CRLF)
    target_sources(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/digital_clock.rc
    )
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
        MACOSX_BUNDLE_ICON_FILE "digital_clock.icns"
    )

    target_sources(${PROJECT_NAME} PRIVATE "digital_clock.icns")
    set_source_files_properties(
        "digital_clock.icns"
            TARGET_DIRECTORY ${PROJECT_NAME}
            PROPERTIES
                MACOSX_PACKAGE_LOCATION Resources
    )
endif()
