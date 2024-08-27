/*
 * SPDX-FileCopyrightText: 2015-2020 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef AUTOSTART_H
#define AUTOSTART_H

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

#endif // AUTOSTART_H
