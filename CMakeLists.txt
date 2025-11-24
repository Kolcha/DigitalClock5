# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.21.1)

project(DigitalClock5 VERSION 5.1.7 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Svg
             OPTIONAL_COMPONENTS Multimedia LinguistTools)

set(tr_langs
    en
    ru
    zh_CN
    tr_TR
    pt_BR
)

qt_standard_project_setup(REQUIRES 6.7 I18N_TRANSLATED_LANGUAGES ${tr_langs})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(clock_utils)

set(CLOCK_PROJECT_NAME ${PROJECT_NAME})
set(CLOCK_VERSION ${PROJECT_VERSION})

set(CLOCK_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CLOCK_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CLOCK_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# dist should be included first, as it defines some important variables
add_subdirectory(dist)

add_subdirectory(3rdparty)
add_subdirectory(app)
add_subdirectory(clock_common)
add_subdirectory(plugin_core)
add_subdirectory(plugins)
add_subdirectory(skin_engine)
