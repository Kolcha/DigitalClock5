# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

function(add_win_rc_file _target _file)
  if (WIN32)
    set(BUILD_NUMBER 0)
    if (DEFINED ENV{GITHUB_RUN_NUMBER})
      set(BUILD_NUMBER $ENV{GITHUB_RUN_NUMBER})
    endif()
    configure_file(${_file}.in ${_file} NEWLINE_STYLE CRLF)
    target_sources(${_target} PRIVATE
      ${CMAKE_CURRENT_BINARY_DIR}/${_file}
    )
  endif()
endfunction()

if (NOT WIN32)
  function(target_win_deploy_qt _target _out_file)
    # just a stub function, to avoid if statements
    # Win32 implementation is in MSI build code
  endfunction()
endif()

function(target_add_translations _target _base)
  qt_add_translations(TARGETS ${_target} TS_FILE_DIR i18n TS_FILE_BASE ${_base}
    LUPDATE_OPTIONS -no-obsolete
  )
  file(GLOB ts_files i18n/*.ts)
  set_source_files_properties(${ts_files} PROPERTIES OUTPUT_LOCATION i18n)
endfunction()
