/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QLocale>
#include <QObject>

#include "plugin_config.hpp"
#include "skin/graphics_widgets.hpp"

class DatePluginImpl
{
  Q_GADGET

public:
  enum FormatType {System, Custom};
  Q_ENUM(FormatType);

  FormatType format_type = System;

  QLocale::FormatType sys_format = QLocale::LongFormat;
  QString str_format = QLocale().dateFormat();

  QDateTime last_dt = QDateTime::currentDateTime();

  std::shared_ptr<GraphicsDateTimeWidget> widget;

  void initSettings(PluginSettingsStorage& st);

  void updateWidget();
};
