# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(stopwatch VERSION 2.1.4 LANGUAGES CXX)

qt_add_plugin(${PROJECT_NAME} CLASS_NAME StopwatchPluginFactory
    gui/hotkeys_settings_widget.cpp
    gui/hotkeys_settings_widget.hpp
    gui/hotkeys_settings_widget.ui
    gui/timer_settings_widget.cpp
    gui/timer_settings_widget.hpp
    gui/timer_settings_widget.ui
    impl/stopwatch_settings.cpp
    impl/stopwatch_settings.hpp
    impl/timetracker.cpp
    impl/timetracker.hpp
    stopwatch_plugin.cpp
    stopwatch_plugin.hpp
)

add_plugin_icon(${PROJECT_NAME} 8726473_stopwatch.svg)
add_win_rc_file(${PROJECT_NAME} ${PROJECT_NAME}.rc)

set_clock_plugin_common_properties(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE PluginCore)
target_link_libraries(${PROJECT_NAME} PRIVATE QHotkey::QHotkey)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

configure_file(${PROJECT_NAME}.json.in ${PROJECT_NAME}.json @ONLY)

target_win_deploy_qt(${PROJECT_NAME} "plugins/${PROJECT_NAME}.dll")
