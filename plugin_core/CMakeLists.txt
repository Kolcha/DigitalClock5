# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(PluginCore VERSION 2.1.2 LANGUAGES CXX)

qt_add_library(${PROJECT_NAME} SHARED
    plugin_core_global.hpp
    clock_plugin.hpp
    clock_plugin.cpp
    settings_bridge.cpp
    settings_bridge.hpp
    shared_config_keys.hpp
    widget_plugin_base.cpp
    widget_plugin_base.hpp
    plugin_config.cpp
    plugin_config.hpp
    impl/plugin_section_appearance.hpp
    impl/appearance_settings_widget.cpp
    impl/appearance_settings_widget.hpp
    impl/appearance_settings_widget.ui
    impl/geometry_settings_widget.cpp
    impl/geometry_settings_widget.hpp
    impl/geometry_settings_widget.ui
    impl/widget_plugin_base_impl.cpp
    impl/widget_plugin_base_impl.hpp
)

add_win_rc_file(${PROJECT_NAME} plugin_core.rc)

target_compile_definitions(${PROJECT_NAME} PRIVATE PLUGIN_CORE_LIBRARY)

target_link_libraries(${PROJECT_NAME} PRIVATE gradient_dialog)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC SkinEngine ClockCommon)

target_win_deploy_qt(${PROJECT_NAME} ${PROJECT_NAME}.dll)
target_add_translations(${PROJECT_NAME} plugin_core)
