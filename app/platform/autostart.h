/*
 * SPDX-FileCopyrightText: 2015-2026 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef AUTOSTART_H
#define AUTOSTART_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Check autostart status.
 * @return true if enabled, false if disabled
 */
bool IsAutoStartEnabled();
/*!
 * Change autostart status (enable/disable).
 * @param enable - true to enable, false to disable
 */
void SetAutoStart(bool enable);

#ifdef __cplusplus
}
#endif

#endif // AUTOSTART_H
