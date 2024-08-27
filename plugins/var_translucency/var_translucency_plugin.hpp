/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "clock_plugin.hpp"

class OpacityChanger;

class VarTranslucencyPlugin : public ClockPluginBase, public WindowAccessExtension
{
  Q_OBJECT

public:
  VarTranslucencyPlugin();
  ~VarTranslucencyPlugin();

  void initWindow(QWidget* wnd) override;

public slots:
  void init() override;
  void shutdown() override;

  void tick() override;

private:
  QWidget* _wnd = nullptr;
  qreal _last_opacity = 1.0;

  std::unique_ptr<OpacityChanger> _changer;
};


class VarTranslucencyPluginFactory : public ClockPluginFactory
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPluginFactory_iid FILE "var_translucency.json")
  Q_INTERFACES(ClockPluginFactory)

public:
  std::unique_ptr<ClockPluginBase> create() const override;

  QString title() const override { return tr("Variable translucency"); }
  QString description() const override;

  bool perClockInstance() const override { return true; }
};
