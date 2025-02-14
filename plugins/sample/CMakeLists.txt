# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: MIT

project(sample VERSION 0.1.2 LANGUAGES CXX)

qt_add_plugin(${PROJECT_NAME} CLASS_NAME SamplePluginFactory
    sample_plugin.cpp
    sample_plugin.hpp
)

add_plugin_icon(${PROJECT_NAME} 8726206_plug_plug.svg)
add_win_rc_file(${PROJECT_NAME} ${PROJECT_NAME}.rc)

set_clock_plugin_common_properties(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE PluginCore)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

configure_file(${PROJECT_NAME}.json.in ${PROJECT_NAME}.json @ONLY)
