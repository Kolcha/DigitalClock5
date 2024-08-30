# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(${CLOCK_PROJECT_NAME} VERSION ${CLOCK_VERSION} LANGUAGES CXX)

qt_add_executable(${PROJECT_NAME}
    main.cpp
    config/config_sections.hpp
    config/config_sections.cpp
    config/app_config.hpp
    config/app_config.cpp
    gui/clock_window.hpp
    gui/clock_window.cpp
    core/application.hpp
    core/application.cpp
    core/skin_manager.hpp
    core/skin_manager.cpp
    gui/clock_icon_engine.hpp
    gui/clock_icon_engine.cpp
    gui/clock_tray_icon.hpp
    gui/clock_tray_icon.cpp
    core/update_checker.hpp
    core/update_checker.cpp
    core/build_date.hpp
    core/build_date.cpp
    gui/logo_label.hpp
    gui/logo_label.cpp
    gui/about_dialog.ui
    gui/about_dialog.hpp
    gui/about_dialog.cpp
    core/dialog_manager.hpp
    gui/settings_dialog.hpp
    gui/settings_dialog.cpp
    gui/settings_dialog.ui
    skin/legacy_skin.hpp
    skin/legacy_skin.cpp
    core/plugin_manager.hpp
    core/plugin_manager.cpp
    gui/plugin_list_item_widget.cpp
    gui/plugin_list_item_widget.hpp
    gui/plugin_list_item_widget.ui
    gui/plugin_info_dialog.cpp
    gui/plugin_info_dialog.hpp
    gui/plugin_info_dialog.ui
    core/plugin_info.hpp
    core/settings_change_listener.cpp
    core/settings_change_listener.hpp
    gui/plugin_settings_dialog.hpp
    gui/plugin_settings_dialog.cpp
    gui/plugin_settings_dialog.ui
    core/translation.cpp
    core/translation.hpp
)

set(APP_IDENTIFIER "com.github.kolcha.DigitalClock5")

add_subdirectory(dist)
add_subdirectory(res)

add_subdirectory(platform)

configure_file(version.hpp.in version.hpp @ONLY)

target_link_libraries(${PROJECT_NAME} PRIVATE gradient_dialog)
target_link_libraries(${PROJECT_NAME} PRIVATE PluginCore SkinEngine ClockCommon)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Widgets Qt::Network Qt::Svg)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME "Digital Clock"
    MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_IDENTIFIER}
    MACOSX_BUNDLE_COPYRIGHT "(c) 2013-2024 Nick Korotysh"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if (APPLE)
    # unfortunately, this can't be in any other CMakeLists.txt in subdirectory :(
    # also cmake copies the content of the directory, not the directory itself :(
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${CMAKE_BINARY_DIR}/skins" "${PROJECT_NAME}.app/Contents/Resources/skins"
    )
endif()

target_win_deploy_qt(${PROJECT_NAME} ${PROJECT_NAME}.exe)
target_add_translations(${PROJECT_NAME} digital_clock)
