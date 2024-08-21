/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QVariantHash>

struct PluginInfo {
  QString path;
  QString id;
  QString title;
  QString description;
  bool configurable;
  bool multiinstance;
  bool enabled;
  QVariantHash metadata;
};
