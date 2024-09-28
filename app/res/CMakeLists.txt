# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

add_subdirectory(icons)
add_subdirectory(skins)

qt_add_resources(${PROJECT_NAME} "lang_conf" PREFIX "/" FILES langs.ini)
