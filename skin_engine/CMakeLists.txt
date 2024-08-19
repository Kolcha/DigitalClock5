# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(SkinEngine VERSION 2.0.0 LANGUAGES CXX)

qt_add_library(${PROJECT_NAME} SHARED
    skin/container.cpp
    skin/container.hpp
    skin/datetime_formatter.cpp
    skin/datetime_formatter.hpp
    skin/font_skin.cpp
    skin/font_skin.hpp
    skin/glyph.cpp
    skin/glyph.hpp
    skin/graphics.cpp
    skin/graphics.hpp
    skin/graphics_widgets.cpp
    skin/graphics_widgets.hpp
    skin/image_glyph.cpp
    skin/image_glyph.hpp
    skin/observable.hpp
    skin/skin.hpp
    skin_engine_global.hpp
)

add_win_rc_file(${PROJECT_NAME} skin_engine.rc)

target_compile_definitions(${PROJECT_NAME} PRIVATE SKIN_ENGINE_LIBRARY)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Qt::Widgets Qt::Svg)
