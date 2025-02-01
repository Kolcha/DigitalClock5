// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "plugin_settings.hpp"

QString instance_prefix(size_t idx)
{
  return QString("instance_%1").arg(idx);
}
