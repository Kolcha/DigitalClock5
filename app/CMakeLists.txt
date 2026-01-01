# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(${CLOCK_PROJECT_NAME} VERSION ${CLOCK_VERSION} LANGUAGES CXX)

qt_add_executable(${PROJECT_NAME}
    config/app_config.cpp
    config/app_config.hpp
    config/app_state.cpp
    config/app_state.hpp
    config/dc4_to_dc5.cpp
    config/dc4_to_dc5.hpp
    config/dc5_legacy.cpp
    config/dc5_legacy.hpp
    config/global_options.hpp
    config/serialization.cpp
    config/serialization.hpp
    config/settings_backend_impl.cpp
    config/settings_backend_impl.hpp
    core/build_date.cpp
    core/build_date.hpp
    core/clock_application.cpp
    core/clock_application.hpp
    core/clock_application_settings.cpp
    core/dialog_manager.hpp
    core/plugin_manager_impl.cpp
    core/plugin_manager_impl.hpp
    core/skin_manager_impl.cpp
    core/skin_manager_impl.hpp
    core/translation.cpp
    core/translation.hpp
    core/tray_icon_manager_impl.cpp
    core/tray_icon_manager_impl.hpp
    core/update_checker.cpp
    core/update_checker.hpp
    gui/about/about_dialog.cpp
    gui/about/about_dialog.hpp
    gui/about/about_dialog.ui
    gui/about/logo_label.cpp
    gui/about/logo_label.hpp
    gui/settings/plugin_info_dialog.cpp
    gui/settings/plugin_info_dialog.hpp
    gui/settings/plugin_info_dialog.ui
    gui/settings/plugin_list_item_widget.cpp
    gui/settings/plugin_list_item_widget.hpp
    gui/settings/plugin_list_item_widget.ui
    gui/settings/settings_dialog.cpp
    gui/settings/settings_dialog.hpp
    gui/settings/settings_dialog.ui
    gui/clock_icon_engine.cpp
    gui/clock_icon_engine.hpp
    gui/clock_tray_icon.cpp
    gui/clock_tray_icon.hpp
    gui/clock_widget.cpp
    gui/clock_widget.hpp
    gui/clock_window.cpp
    gui/clock_window.hpp
    gui/overlay_widget.cpp
    gui/overlay_widget.hpp
    skin/error_skin.cpp
    skin/error_skin.hpp
    skin/legacy_skin.cpp
    skin/legacy_skin.hpp
    skin/time_renderer.cpp
    skin/time_renderer.hpp
    main.cpp
)

set(APP_IDENTIFIER "com.github.kolcha.DigitalClock5")

add_subdirectory(dist)
add_subdirectory(res)

add_subdirectory(platform)

configure_file(version.hpp.in version.hpp @ONLY)

target_link_libraries(${PROJECT_NAME} PRIVATE gradient_dialog)
target_link_libraries(${PROJECT_NAME} PRIVATE PluginCore SkinEngine ClockCommon)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network Qt6::Svg)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME "Digital Clock 5"
    MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_IDENTIFIER}
    MACOSX_BUNDLE_COPYRIGHT "(c) 2013-2026 Nick Korotysh"
    MACOSX_BUNDLE_BUNDLE_VERSION ${BUILD_NUMBER}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
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
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${CMAKE_BINARY_DIR}/textures" "${PROJECT_NAME}.app/Contents/Resources/textures"
    )
endif()

target_win_deploy_qt(${PROJECT_NAME} ${PROJECT_NAME}.exe)
target_add_translations(${PROJECT_NAME} digital_clock)
