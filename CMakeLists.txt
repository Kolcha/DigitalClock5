# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.19)

project(DigitalClock5 VERSION 5.0.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Network Svg)

qt_add_executable(${PROJECT_NAME}
    main.cpp
    skin/glyph.hpp
    skin/container.hpp
    skin/skin.hpp
    skin/font_skin.hpp
    skin/container.cpp
    skin/font_skin.cpp
    skin/glyph.cpp
    skin/datetime_formatter.hpp
    skin/datetime_formatter.cpp
    skin/graphics.hpp
    skin/graphics.cpp
    skin/graphics_widgets.hpp
    skin/graphics_widgets.cpp
    config/settings_storage.hpp
    config/settings_storage.cpp
    config/custom_converters.hpp
    config/config_sections.hpp
    config/config_sections.cpp
    config/app_config.hpp
    config/app_config.cpp
    app/state.hpp
    app/clock_window.hpp
    app/clock_window.cpp
    app/application.hpp
    app/application.cpp
    app/skin_manager.hpp
    app/skin_manager.cpp
    app/clock_icon_engine.hpp
    app/clock_icon_engine.cpp
    app/clock_tray_icon.hpp
    app/clock_tray_icon.cpp
    app/update_checker.hpp
    app/update_checker.cpp
    app/build_date.hpp
    app/build_date.cpp
    app/logo_label.hpp
    app/logo_label.cpp
    app/about_dialog.ui
    app/about_dialog.hpp
    app/about_dialog.cpp
    app/dialog_manager.hpp
    app/settings_dialog.hpp
    app/settings_dialog.cpp
    app/settings_dialog.ui
    skin/image_glyph.hpp
    skin/image_glyph.cpp
    skin/legacy_skin.hpp
    skin/legacy_skin.cpp
)

set(APP_IDENTIFIER "com.github.kolcha.DigitalClock5")

add_subdirectory(deps)
add_subdirectory(dist)
add_subdirectory(res)

configure_file(version.hpp.in version.hpp @ONLY)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE gradient_dialog)
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
