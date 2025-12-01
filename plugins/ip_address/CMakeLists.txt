# SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

project(ip_address VERSION 2.0.2 LANGUAGES CXX)

qt_add_plugin(${PROJECT_NAME} CLASS_NAME IpAddressPluginFactory
    gui/settings_widget.cpp
    gui/settings_widget.hpp
    gui/settings_widget.ui
    impl/ext_ip_detectors.cpp
    impl/ext_ip_detectors.hpp
    impl/ip_address_settings.cpp
    impl/ip_address_settings.hpp
    impl/network_interfaces_model.cpp
    impl/network_interfaces_model.hpp
    ip_address_plugin.cpp
    ip_address_plugin.hpp
)

add_plugin_icon(${PROJECT_NAME} 8726343_server_connection_server_connection.svg)
add_win_rc_file(${PROJECT_NAME} ${PROJECT_NAME}.rc)

set_clock_plugin_common_properties(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE PluginCore)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network)

configure_file(${PROJECT_NAME}.json.in ${PROJECT_NAME}.json @ONLY)

target_win_deploy_qt(${PROJECT_NAME} "plugins/${PROJECT_NAME}.dll")
