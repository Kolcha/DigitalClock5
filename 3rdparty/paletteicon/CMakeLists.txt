# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

# wrapper CMakeLists.txt for paletteicon module, as it is qmake-based

cmake_minimum_required(VERSION 3.19)

project(paletteicon VERSION 1.3.1 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Svg)

qt_standard_project_setup()

qt_add_plugin(paletteicon CLASS_NAME PaletteIconPlugin
    paletteicon/palette_icon_engine.cpp
    paletteicon/palette_icon_engine.hpp
    paletteicon/plugin.cpp
)

target_link_libraries(paletteicon PRIVATE Qt6::Gui Qt6::Svg)

target_compile_definitions(paletteicon PRIVATE QT_DEPRECATED_WARNINGS)

if (WIN32)
    target_sources(paletteicon PRIVATE paletteicon/paletteicon.rc)
endif()


# clock specific - place plugin close to the application executable
if (WIN32)
    set(PLUGINS_OUT_DIR "${CMAKE_BINARY_DIR}/plugins/iconengines")
endif()

if (APPLE)
    set(PLUGINS_OUT_DIR "${CMAKE_BINARY_DIR}/app/${CLOCK_PROJECT_NAME}.app/Contents/PlugIns/iconengines")
endif()

if (DEFINED PLUGINS_OUT_DIR)
    set_target_properties(paletteicon PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${PLUGINS_OUT_DIR}"
    )
endif()

target_win_deploy_qt(${PROJECT_NAME} "plugins/iconengines/${PROJECT_NAME}.dll")
