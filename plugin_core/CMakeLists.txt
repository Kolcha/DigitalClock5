# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(PluginCore VERSION 2.2.6 LANGUAGES CXX)

qt_add_library(${PROJECT_NAME} SHARED
    plugin/text/impl/appearance_settings_widget.cpp
    plugin/text/impl/appearance_settings_widget.hpp
    plugin/text/impl/appearance_settings_widget.ui
    plugin/text/impl/geometry_settings_widget.cpp
    plugin/text/impl/geometry_settings_widget.hpp
    plugin/text/impl/geometry_settings_widget.ui
    plugin/text/impl/settings_dialog.cpp
    plugin/text/impl/settings_dialog.hpp
    plugin/text/impl/settings_dialog.ui
    plugin/text/impl/shared_option_router.cpp
    plugin/text/impl/shared_option_router.hpp
    plugin/text/plugin_config.cpp
    plugin/text/plugin_config.hpp
    plugin/text/skinned_text_plugin_base.cpp
    plugin/text/skinned_text_plugin_base.hpp
    plugin/clock_plugin.cpp
    plugin/clock_plugin.hpp
    plugin/plugin_settings.cpp
    plugin/plugin_settings.hpp
    plugin_core_global.hpp
)

add_win_rc_file(${PROJECT_NAME} plugin_core.rc)

target_compile_definitions(${PROJECT_NAME} PRIVATE PLUGIN_CORE_LIBRARY)

target_link_libraries(${PROJECT_NAME} PRIVATE gradient_dialog)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC SkinEngine ClockCommon)

target_win_deploy_qt(${PROJECT_NAME} ${PROJECT_NAME}.dll)
target_add_translations(${PROJECT_NAME} plugin_core)
