# SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(talking_clock VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_AUTORCC ON)

qt_add_plugin(${PROJECT_NAME} CLASS_NAME TalkingClockPluginFactory
    gui/settings_dialog.cpp
    gui/settings_dialog.hpp
    gui/settings_dialog.ui
    gui/voice_config_dialog.cpp
    gui/voice_config_dialog.hpp
    gui/voice_config_dialog.ui
    impl/talking_clock_settings.hpp
    talking_clock.qrc
    talking_clock_plugin.cpp
    talking_clock_plugin.hpp
)

add_win_rc_file(${PROJECT_NAME} ${PROJECT_NAME}.rc)

set_clock_plugin_common_properties(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE PluginCore)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::TextToSpeech)

configure_file(${PROJECT_NAME}.json.in ${PROJECT_NAME}.json @ONLY)

target_win_deploy_qt(${PROJECT_NAME} "plugins/${PROJECT_NAME}.dll")
