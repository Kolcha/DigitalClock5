# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(any_zoom VERSION 3.0.1 LANGUAGES CXX)

qt_add_plugin(${PROJECT_NAME} CLASS_NAME AnyZoomPluginFactory
    gui/settings_widget.cpp
    gui/settings_widget.hpp
    gui/settings_widget.ui
    impl/any_zoom_plugin_impl.cpp
    impl/any_zoom_plugin_impl.hpp
    impl/any_zoom_settings.hpp
    any_zoom_plugin.cpp
    any_zoom_plugin.hpp
)

add_win_rc_file(${PROJECT_NAME} ${PROJECT_NAME}.rc)

set_clock_plugin_common_properties(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE PluginCore)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

configure_file(${PROJECT_NAME}.json.in ${PROJECT_NAME}.json @ONLY)

target_win_deploy_qt(${PROJECT_NAME} "plugins/${PROJECT_NAME}.dll")
