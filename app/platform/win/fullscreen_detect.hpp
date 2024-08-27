/*
 * SPDX-FileCopyrightText: 2017-2020 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QSet>
#include <QString>
#include <QWidget>

namespace digital_clock {

/*!
 * Finds fullscreen windows on the same monitor as @a wid.
 * @param ignore_list - list of fullscreen windows to ignore
 * @return true if fullscreen window was found
 */
bool IsFullscreenWndOnSameMonitor(WId wid, const QSet<QString>& ignore_list);

/*!
 * Finds fullscreen windows on the same monitor as @a wid.
 * @return list of fullscreen windows class names
 */
QSet<QString> GetFullscreenWindowsOnSameMonitor(WId wid);

} // namespace digital_clock
