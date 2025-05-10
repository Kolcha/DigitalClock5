# SPDX-FileCopyrightText: 2023-2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: MIT

set(MSI_FILE "digital_clock_5-x64.msi")
set(MSI_SRC_DIR "${CMAKE_BINARY_DIR}/installer_files" CACHE STRING "MSI source dir")

set(WIX_SRC_FILES
    main.wxs
    app_files.wxs
    skins.wxs
    textures.wxs
)

set_property(GLOBAL PROPERTY JOB_POOLS single_job=1)

set(WIX_BUILD_FLAGS "-arch x64 -ext WixToolset.Util.wixext")
string(APPEND WIX_BUILD_FLAGS " -d AppVersion=${PROJECT_VERSION}")
string(APPEND WIX_BUILD_FLAGS " -d AppIcon=${CMAKE_SOURCE_DIR}/app/dist/digital_clock.ico")
string(APPEND WIX_BUILD_FLAGS " -d MySource=${MSI_SRC_DIR}")

list(TRANSFORM WIX_SRC_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

separate_arguments(WIX_BUILD_FLAGS)

add_custom_target(deploy_qt COMMENT "Deploying Qt")

# not strictly required to be here, but just convenient
add_custom_command(TARGET deploy_qt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_BINARY_DIR}/skins" "${MSI_SRC_DIR}/skins"
)

add_custom_command(TARGET deploy_qt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_BINARY_DIR}/textures" "${MSI_SRC_DIR}/textures"
)

function(target_win_deploy_qt _target _out_file)
    add_custom_command(
        OUTPUT ${MSI_SRC_DIR}/${_out_file}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${_target}> ${MSI_SRC_DIR}/${_out_file}
        DEPENDS ${_target}
    )

    add_custom_target(${_target}_deploy_qt
        COMMAND windeployqt --libdir . --plugindir "plugins" --translationdir "translations"
            --no-compiler-runtime
            --no-system-d3d-compiler
            --no-system-dxc-compiler
            --no-opengl-sw
            --skip-plugin-types generic
            --exclude-plugins ffmpegmediaplugin
            ${_out_file}
        WORKING_DIRECTORY ${MSI_SRC_DIR}
        DEPENDS ${MSI_SRC_DIR}/${_out_file}
        JOB_POOL single_job
    )

    add_dependencies(deploy_qt ${_target}_deploy_qt)
endfunction()

add_custom_command(
    OUTPUT ${MSI_FILE}
    COMMAND wix build ${WIX_BUILD_FLAGS} ${WIX_SRC_FILES} -o ${MSI_FILE}
    DEPENDS ${WIX_SRC_FILES} deploy_qt
    VERBATIM
    COMMAND_EXPAND_LISTS
)

add_custom_target(win_installer DEPENDS ${MSI_FILE} SOURCES ${WIX_SRC_FILES})
