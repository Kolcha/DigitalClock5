# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${PROJECT_NAME} PRIVATE
    autostart.h
    mouse_tracker.hpp
)

if (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE
        win/autostart_win.cpp
        win/clock_window.cpp
        win/fullscreen_detect.cpp
        win/fullscreen_detect.hpp
        win/mouse_tracker.cpp
        win/system_theme_tracker.cpp
        win/system_theme_tracker.hpp
    )
endif()

if (APPLE)
    target_sources(${PROJECT_NAME} PRIVATE
        mac/autostart_mac.cpp
        mac/clock_window.mm
        mac/mouse_tracker.mm
    )
endif()
