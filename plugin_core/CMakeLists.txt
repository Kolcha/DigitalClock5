# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(PluginCore VERSION 2.0.0 LANGUAGES CXX)

qt_add_library(${PROJECT_NAME} SHARED
    plugin_core_global.hpp
    clock_plugin.hpp
    clock_plugin.cpp
    settings_bridge.cpp
    settings_bridge.hpp
    shared_config_keys.hpp
)

add_win_rc_file(${PROJECT_NAME} plugin_core.rc)

target_compile_definitions(${PROJECT_NAME} PRIVATE PLUGIN_CORE_LIBRARY)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ClockCommon)
