# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(date VERSION 2.0.0 LANGUAGES CXX)

qt_add_plugin(${PROJECT_NAME} CLASS_NAME DatePlugin
    gui/settings_widget.hpp
    gui/settings_widget.cpp
    gui/settings_widget.ui
    impl/date_plugin_impl.cpp
    impl/date_plugin_impl.hpp
    impl/date_settings.hpp
    date_plugin.cpp
    date_plugin.hpp
)

add_win_rc_file(${PROJECT_NAME} ${PROJECT_NAME}.rc)

set_clock_plugin_common_properties(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE PluginCore)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Widgets)

configure_file(${PROJECT_NAME}.json.in ${PROJECT_NAME}.json @ONLY)
