# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(ClockCommon VERSION 2.0.1 LANGUAGES CXX)

qt_add_library(${PROJECT_NAME} SHARED
    config/appearance_base.cpp
    config/appearance_base.hpp
    config/settings_storage.hpp
    config/settings_storage.cpp
    config/custom_converters.hpp
    config/sample_brushes.cpp
    config/sample_brushes.hpp
    clock_common_global.hpp
    state.cpp
    state.hpp
)

add_win_rc_file(${PROJECT_NAME} clock_common.rc)

target_compile_definitions(${PROJECT_NAME} PRIVATE CLOCK_COMMON_LIBRARY)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Gui)

target_win_deploy_qt(${PROJECT_NAME} ${PROJECT_NAME}.dll)
