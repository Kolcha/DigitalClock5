/*
 * SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "compat.hpp"

#include <qplatformdefs.h>

#ifdef Q_OS_MACOS
#include "platform/mac/autostart_mac_migration.hpp"
#endif

namespace compat {

void migrate()
{
#ifdef Q_OS_MACOS
  migrate_mac_autostart();
#endif
}

} // namespace compat
