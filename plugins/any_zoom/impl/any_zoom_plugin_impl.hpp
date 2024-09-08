/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QObject>

#include "plugin_config.hpp"
#include "shared_config_keys.hpp"

class AnyZoomPluginImpl : public QObject
{
  Q_OBJECT

public:
  explicit AnyZoomPluginImpl(QObject* parent = nullptr);

  void initSettings(PluginSettingsStorage& st);

  int clock_zoom = 100;

  int zoom_x = 100;
  int zoom_y = 100;

signals:
  void optionChanged(cs::SharedConfigKeys, const QVariant&);
};
