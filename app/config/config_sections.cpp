/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "config_sections.hpp"

// complex default values should be implemneted here

QFont SectionAppearance::default_font()
{
#ifdef Q_OS_WINDOWS
  return QFont("Comic Sans MS", 72);
#endif
  return QFont("Comic Sans MS", 96);
}
