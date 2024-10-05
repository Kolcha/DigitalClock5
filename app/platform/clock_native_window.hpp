/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QtGlobal>

#if defined(Q_OS_WINDOWS)
#define HAVE_NATIVE_WIN_IMPL
#include "win/clock_native_window.hpp"
#endif

#if defined(Q_OS_MACOS)
#define HAVE_NATIVE_WIN_IMPL
#include "mac/clock_native_window.hpp"
#endif

#if !defined(HAVE_NATIVE_WIN_IMPL)
#include "gui/clock_window.hpp"
using ClockNativeWindow = ClockWindow;
#endif
