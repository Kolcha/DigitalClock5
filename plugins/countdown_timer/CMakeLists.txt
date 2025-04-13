# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(countdown_timer VERSION 2.2.1 LANGUAGES CXX)

qt_add_plugin(${PROJECT_NAME} CLASS_NAME CountdownTimerPluginFactory
    gui/hotkeys_settings_widget.cpp
    gui/hotkeys_settings_widget.hpp
    gui/hotkeys_settings_widget.ui
    gui/timer_settings_widget.cpp
    gui/timer_settings_widget.hpp
    gui/timer_settings_widget.ui
    impl/countdown_timer.cpp
    impl/countdown_timer.hpp
    impl/countdown_timer_settings.cpp
    impl/countdown_timer_settings.hpp
    impl/utilities.cpp
    impl/utilities.hpp
    countdown_timer_plugin.cpp
    countdown_timer_plugin.hpp
)

add_plugin_icon(${PROJECT_NAME} if_timer_172599.svg)
add_win_rc_file(${PROJECT_NAME} ${PROJECT_NAME}.rc)

set_clock_plugin_common_properties(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE PluginCore)
target_link_libraries(${PROJECT_NAME} PRIVATE QHotkey::QHotkey)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Multimedia)

configure_file(${PROJECT_NAME}.json.in ${PROJECT_NAME}.json @ONLY)

target_win_deploy_qt(${PROJECT_NAME} "plugins/${PROJECT_NAME}.dll")
