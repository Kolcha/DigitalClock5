# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: MIT

project(sample VERSION 0.1.1 LANGUAGES CXX)

qt_add_plugin(${PROJECT_NAME} CLASS_NAME SamplePluginFactory
    sample_plugin.cpp
    sample_plugin.hpp
)

qt_add_resources(${PROJECT_NAME} "resources"
    PREFIX "/icons"
    FILES ${PROJECT_NAME}.svg
)

add_win_rc_file(${PROJECT_NAME} ${PROJECT_NAME}.rc)

set_clock_plugin_common_properties(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE PluginCore)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Widgets)

configure_file(${PROJECT_NAME}.json.in ${PROJECT_NAME}.json @ONLY)
