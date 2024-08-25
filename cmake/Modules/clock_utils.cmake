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
