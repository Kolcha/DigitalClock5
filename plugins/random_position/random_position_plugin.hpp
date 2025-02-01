/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/clock_plugin.hpp"

#include <QPoint>

class RandomPositionPlugin : public ClockPluginInstance, public WidgetAccess
{
  Q_OBJECT

public:
  void init(QWidget* wnd) override;

public slots:
  void startup() override;
  void shutdown() override;

  void update(const QDateTime& dt) override;

private:
  QWidget* _wnd = nullptr;
  QPoint _last_pos;
  int _interval_counter = 60;
};


class RandomPositionPluginFactory : public ClockPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "random_position.json")
  Q_INTERFACES(ClockPlugin)

public:
  void init(Context&& ctx) override {}

  QString name() const override { return tr("Random position"); }
  QString description() const override;

  ClockPluginInstance* instance(size_t idx) override;

public slots:
  void configure(QWidget* parent, size_t idx) override {}

private:
  std::map<size_t, std::unique_ptr<RandomPositionPlugin>> _insts;
};
