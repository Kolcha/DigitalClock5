# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(quick_note VERSION 2.1.3 LANGUAGES CXX)

qt_add_plugin(${PROJECT_NAME} CLASS_NAME QuickNotePluginFactory
    quick_note_plugin.cpp
    quick_note_plugin.hpp
)

add_plugin_icon(${PROJECT_NAME} 8726310_notes_notes.svg)
add_win_rc_file(${PROJECT_NAME} ${PROJECT_NAME}.rc)

set_clock_plugin_common_properties(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE PluginCore)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

configure_file(${PROJECT_NAME}.json.in ${PROJECT_NAME}.json @ONLY)

target_win_deploy_qt(${PROJECT_NAME} "plugins/${PROJECT_NAME}.dll")
