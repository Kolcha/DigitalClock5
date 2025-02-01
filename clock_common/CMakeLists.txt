# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(ClockCommon VERSION 2.0.3 LANGUAGES CXX)

qt_add_library(${PROJECT_NAME} SHARED
    common/config_base.cpp
    common/config_base.hpp
    common/datetime_formatter.cpp
    common/datetime_formatter.hpp
    common/instance_options.hpp
    common/instance_options_hash.hpp
    common/settings.cpp
    common/settings.hpp
    common/skin_manager.cpp
    common/skin_manager.hpp
    common/tray_icon_manager.cpp
    common/tray_icon_manager.hpp
    common/variant_converters.hpp
    clock_common_global.hpp
)

add_win_rc_file(${PROJECT_NAME} clock_common.rc)

target_compile_definitions(${PROJECT_NAME} PRIVATE CLOCK_COMMON_LIBRARY)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Gui)

target_win_deploy_qt(${PROJECT_NAME} ${PROJECT_NAME}.dll)
