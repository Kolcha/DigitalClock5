# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${PROJECT_NAME} PRIVATE
    autostart.h
    clock_native_window.hpp
)

if (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE
        mouse_tracker.hpp
        win/autostart_win.cpp
        win/clock_native_window.cpp
        win/clock_native_window.hpp
        win/fullscreen_detect.cpp
        win/fullscreen_detect.hpp
        win/mouse_tracker.cpp
        win/system_theme_tracker.cpp
        win/system_theme_tracker.hpp
    )
endif()

if (APPLE)
    target_sources(${PROJECT_NAME} PRIVATE
        mouse_tracker.hpp
        mac/autostart.m
        mac/clock_native_window.hpp
        mac/clock_native_window.mm
        mac/mouse_tracker.mm
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework ServiceManagement"
    )
endif()
