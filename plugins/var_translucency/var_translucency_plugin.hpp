/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/clock_plugin.hpp"

class OpacityChanger;

class VarTranslucencyPlugin : public ClockPluginInstance, public WidgetAccess
{
  Q_OBJECT

public:
  VarTranslucencyPlugin();
  ~VarTranslucencyPlugin();

  void init(QWidget* wnd) override;

public slots:
  void startup() override;
  void shutdown() override;

  void update(const QDateTime& dt) override;

private:
  QWidget* _wnd = nullptr;
  qreal _last_opacity = 1.0;

  std::unique_ptr<OpacityChanger> _changer;
};


class VarTranslucencyPluginFactory : public ClockPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "var_translucency.json")
  Q_INTERFACES(ClockPlugin)

public:
  void init(Context&& ctx) override {}

  QString name() const override { return tr("Variable translucency"); }
  QString description() const override;

  ClockPluginInstance* instance(size_t idx) override;

public slots:
  void configure(QWidget* parent, size_t idx) override {}

private:
  std::map<size_t, std::unique_ptr<VarTranslucencyPlugin>> _insts;
};
