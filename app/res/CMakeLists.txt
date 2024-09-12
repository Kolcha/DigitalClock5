# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

# target qt_add_resources() variant can't be used with CMAKE_AUTORCC ON,
# this leads to duplicated resource implementation
# so, use variable qt_add_resources() variant and set CMAKE_AUTORCC OFF
set(app_res_files "")
qt_add_resources(app_res_files
    icons.qrc
    skins.qrc
)

# due to changes introduced in CMake 3.18, it is impossible to use variable
# qt_add_resources() variant directly in subdirectories, see link for details
# https://cmake.org/cmake/help/latest/command/set_source_files_properties.html
# so, create custom target to generate sources from resource files
add_custom_target(gen_resources SOURCES ${app_res_files})

set_source_files_properties(${app_res_files}
    TARGET_DIRECTORY ${PROJECT_NAME}
    PROPERTIES
        GENERATED TRUE
)
target_sources(${PROJECT_NAME} PRIVATE ${app_res_files})

add_dependencies(${PROJECT_NAME} gen_resources)

qt_add_resources(${PROJECT_NAME} "lang_conf" PREFIX "/" FILES langs.ini)
