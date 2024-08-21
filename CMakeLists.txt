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

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Network Svg)

qt_standard_project_setup()

set(CLOCK_VERSION ${PROJECT_VERSION})

set(CLOCK_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CLOCK_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CLOCK_VERSION_PATCH ${PROJECT_VERSION_PATCH})

add_subdirectory(3rdparty)
add_subdirectory(app)
add_subdirectory(clock_common)
add_subdirectory(plugin_core)
add_subdirectory(skin_engine)
