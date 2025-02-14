# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(SkinEngine VERSION 2.1.1 LANGUAGES CXX)

qt_add_library(${PROJECT_NAME} SHARED
    renderer/font_skin.cpp
    renderer/font_skin.hpp
    renderer/image_skin.cpp
    renderer/image_skin.hpp
    renderer/item.cpp
    renderer/item.hpp
    renderer/layout.cpp
    renderer/layout.hpp
    renderer/painter_guard.hpp
    renderer/renderable.cpp
    renderer/renderable.hpp
    renderer/renderer.cpp
    renderer/renderer.hpp
    renderer/skin.cpp
    renderer/skin.hpp
    renderer/skin_base.cpp
    renderer/skin_base.hpp
    utilities/layout_config_parser.cpp
    utilities/layout_config_parser.hpp
    utilities/texture.cpp
    utilities/texture.hpp
    scale_factor.hpp
    skin_engine_global.hpp
    skinned_widget.cpp
    skinned_widget.hpp
)

add_win_rc_file(${PROJECT_NAME} skin_engine.rc)

target_compile_definitions(${PROJECT_NAME} PRIVATE SKIN_ENGINE_LIBRARY)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Widgets Qt6::Svg)

target_win_deploy_qt(${PROJECT_NAME} ${PROJECT_NAME}.dll)
