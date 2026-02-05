# SPDX-FileCopyrightText: 2024-2026 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.21.1)

project(DigitalClock5 VERSION 5.1.9 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FetchContent)

FetchContent_Declare(
  GradientDialog
  GIT_REPOSITORY https://github.com/Kolcha/GradientDialog.git
  GIT_TAG        86389141a7dfea0f5dfde93c6b5ebe0bac1d7e68
)
FetchContent_MakeAvailable(GradientDialog)
# on Linux -fPIC became required for this static library for some reason...
set_property(TARGET gradient_dialog PROPERTY POSITION_INDEPENDENT_CODE ON)

set(QT_DEFAULT_MAJOR_VERSION 6 CACHE STRING "Qt version for QHotkey")
set(QHOTKEY_INSTALL OFF CACHE BOOL "Enable QHotkey install rule")
FetchContent_Declare(
  QHotkey
  GIT_REPOSITORY https://github.com/Skycoder42/QHotkey.git
  GIT_TAG        4e3a244d87f1f7e741e1395f2ffe825f3a8ada45
)
FetchContent_MakeAvailable(QHotkey)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Svg
             OPTIONAL_COMPONENTS Multimedia TextToSpeech LinguistTools)

set(tr_langs
    en
    ru
    zh_CN
    tr_TR
    pt_BR
    pt_PT
)

qt_standard_project_setup(REQUIRES 6.7 I18N_TRANSLATED_LANGUAGES ${tr_langs})

include(cmake/clock_utils.cmake)

set(CLOCK_PROJECT_NAME ${PROJECT_NAME})
set(CLOCK_VERSION ${PROJECT_VERSION})

set(CLOCK_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CLOCK_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CLOCK_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# dist should be included first, as it defines some important variables
add_subdirectory(dist)

add_subdirectory(app)
add_subdirectory(clock_common)
add_subdirectory(plugin_core)
add_subdirectory(plugins)
add_subdirectory(skin_engine)
