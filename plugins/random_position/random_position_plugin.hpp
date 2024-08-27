/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "clock_plugin.hpp"

class RandomPositionPlugin : public ClockPluginBase, public WindowAccessExtension
{
  Q_OBJECT

public:
  void initWindow(QWidget* wnd) override;

public slots:
  void init() override;
  void shutdown() override;

  void tick() override;

private:
  bool _is_active = false;
  QWidget* _wnd = nullptr;
  QPoint _last_pos;
  int _interval_counter = 60;
};


class RandomPositionPluginFactory : public ClockPluginFactory
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPluginFactory_iid FILE "random_position.json")
  Q_INTERFACES(ClockPluginFactory)

public:
  std::unique_ptr<ClockPluginBase> create() const override;

  QString title() const override { return tr("Random position"); }
  QString description() const override;

  bool perClockInstance() const override { return true; }
};
