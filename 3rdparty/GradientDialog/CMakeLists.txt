# SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

include(FetchContent)

FetchContent_Declare(
  GradientDialog
  GIT_REPOSITORY https://github.com/Kolcha/GradientDialog.git
  GIT_TAG        86389141a7dfea0f5dfde93c6b5ebe0bac1d7e68
)

FetchContent_MakeAvailable(GradientDialog)
