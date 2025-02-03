# SPDX-FileCopyrightText: Copyright (c) 2016, Felix Barz
# SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause

# rewritten the original one, mostly to drop the very annoying
# warning about too old required CMake version and CPack stuff

cmake_minimum_required(VERSION 3.16)

project(qhotkey
    VERSION 1.5.0
    DESCRIPTION "Global hotkey library for Qt software"
    HOMEPAGE_URL "https://skycoder42.github.io/QHotkey/"
    LANGUAGES CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.2.0 COMPONENTS Core Gui REQUIRED)

add_library(qhotkey QHotkey/QHotkey/qhotkey.cpp)
add_library(QHotkey::QHotkey ALIAS qhotkey)
target_link_libraries(qhotkey PUBLIC Qt6::Core Qt6::Gui)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(qhotkey PRIVATE QHOTKEY_LIBRARY)
    target_compile_definitions(qhotkey PUBLIC QHOTKEY_SHARED)
endif()

if(APPLE)
    find_library(CARBON_LIBRARY Carbon)
    mark_as_advanced(CARBON_LIBRARY)

    target_sources(qhotkey PRIVATE QHotkey/QHotkey/qhotkey_mac.cpp)
    target_link_libraries(qhotkey PRIVATE ${CARBON_LIBRARY})
elseif(WIN32)
    target_sources(qhotkey PRIVATE QHotkey/QHotkey/qhotkey_win.cpp)
else()
    find_package(X11 REQUIRED)
    if(QT_DEFAULT_MAJOR_VERSION GREATER_EQUAL 6)
        target_link_libraries(qhotkey PRIVATE ${X11_LIBRARIES})
    else()
        find_package(Qt6 COMPONENTS X11Extras REQUIRED)
        target_link_libraries(qhotkey
            PRIVATE
                ${X11_LIBRARIES}
                Qt6::X11Extras)
    endif()

    include_directories(${X11_INCLUDE_DIR})
    target_sources(qhotkey PRIVATE QHotkey/QHotkey/qhotkey_x11.cpp)
endif()

target_include_directories(qhotkey
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/QHotkey/QHotkey>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set_target_properties(qhotkey PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION}
    INTERFACE_QHotkey_MAJOR_VERSION ${PROJECT_VERSION_MAJOR}
    COMPATIBLE_INTERFACE_STRING QHotkey_MAJOR_VERSION)
