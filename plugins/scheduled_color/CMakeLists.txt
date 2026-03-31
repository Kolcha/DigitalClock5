# SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(scheduled_color VERSION 1.0.0 LANGUAGES CXX)

qt_add_plugin(${PROJECT_NAME} CLASS_NAME ScheduledColorPluginFactory
    scheduled_color_plugin.cpp
    scheduled_color_plugin.hpp
    gui/settings_dialog.cpp
    gui/settings_dialog.hpp
    gui/settings_dialog.ui
    impl/scheduled_color_settings.hpp
)

add_win_rc_file(${PROJECT_NAME} ${PROJECT_NAME}.rc)

set_clock_plugin_common_properties(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE PluginCore)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

configure_file(${PROJECT_NAME}.json.in ${PROJECT_NAME}.json @ONLY)

target_win_deploy_qt(${PROJECT_NAME} "plugins/${PROJECT_NAME}.dll")
