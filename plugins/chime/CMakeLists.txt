# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(chime VERSION 2.0.0 LANGUAGES CXX)

qt_add_plugin(${PROJECT_NAME} CLASS_NAME ChimePlugin
    gui/settings_widget.cpp
    gui/settings_widget.hpp
    gui/settings_widget.ui
    gui/sound_dialog.cpp
    gui/sound_dialog.hpp
    gui/sound_dialog.ui
    impl/chime_plugin_impl.cpp
    impl/chime_plugin_impl.hpp
    impl/chime_settings.hpp
    chime.qrc
    chime_plugin.cpp
    chime_plugin.hpp
)

add_win_rc_file(${PROJECT_NAME} ${PROJECT_NAME}.rc)

set_clock_plugin_common_properties(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE PluginCore)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Widgets Qt::Multimedia)

configure_file(${PROJECT_NAME}.json.in ${PROJECT_NAME}.json @ONLY)

target_win_deploy_qt(${PROJECT_NAME} "plugins/${PROJECT_NAME}.dll")
