cmake_minimum_required(VERSION 3.16)

project(untitled VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(untitled
    MANUAL_FINALIZATION
    main.cpp
    skin/glyph.hpp
    skin/container.hpp
    skin/skin.hpp
    skin/font_skin.hpp
    skin/container.cpp
    skin/font_skin.cpp
    skin/glyph.cpp
    skin/datetime_formatter.hpp
    skin/datetime_formatter.cpp
    skin/graphics.hpp
    skin/graphics.cpp
    skin/graphics_widgets.hpp
    skin/graphics_widgets.cpp
    config/settings_storage.hpp
    config/settings_storage.cpp
    config/custom_converters.hpp
    config/config_sections.hpp
    config/config_sections.cpp
    config/app_config.hpp
    config/app_config.cpp
    app/state.hpp
    app/clock_window.hpp
    app/clock_window.cpp
    app/application.hpp
    app/application.cpp
    app/skin_manager.hpp
    app/skin_manager.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(untitled PRIVATE Qt::Widgets)

set_target_properties(untitled PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS untitled
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(untitled)
