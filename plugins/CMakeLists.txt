# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

if (WIN32)
    set(PLUGINS_OUT_DIR "${CMAKE_BINARY_DIR}/plugins")
endif()

if (APPLE)
    set(PLUGINS_OUT_DIR "${CMAKE_BINARY_DIR}/app/${CLOCK_PROJECT_NAME}.app/Contents/PlugIns")
endif()

function(set_clock_plugin_common_properties target)
    if (DEFINED PLUGINS_OUT_DIR)
        set_target_properties(${target} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${PLUGINS_OUT_DIR}"
        )
    endif()
    set_target_properties(${target} PROPERTIES PREFIX "")
    target_add_translations(${target} ${target})
endfunction()

add_subdirectory(any_zoom)
add_subdirectory(date)
add_subdirectory(quick_note)
add_subdirectory(random_position)
add_subdirectory(sample)
add_subdirectory(spectrum_clock)
add_subdirectory(stopwatch)
add_subdirectory(time_zone)
add_subdirectory(var_translucency)

if (TARGET Qt6::Multimedia)
    add_subdirectory(chime)
    add_subdirectory(countdown_timer)
endif()
