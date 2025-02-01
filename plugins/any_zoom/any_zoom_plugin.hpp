/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/clock_plugin.hpp"

#include "plugin/plugin_settings.hpp"
#include "scale_factor.hpp"

#include "impl/any_zoom_settings.hpp"

using any_zoom::AnyZoomPluginInstanceConfig;

class AnyZoomPlugin : public SettingsPluginInstance
{
  Q_OBJECT

public:
  AnyZoomPlugin(const AnyZoomPluginInstanceConfig& cfg);

  void init(const InstanceOptionsHash& settings) override;

public slots:
  void startup() override;
  void shutdown() override;

  void update(const QDateTime& dt) override { Q_UNUSED(dt); }

  void onOptionChanged(opt::InstanceOptions opt, const QVariant& val) override;

  void onScaleFactorChanged(const ScaleFactor& f);
  void reloadConfig();

private:
  const AnyZoomPluginInstanceConfig& _cfg;
  int _clock_scaling = 100;
};


class AnyZoomPluginFactory : public ClockPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "any_zoom.json")
  Q_INTERFACES(ClockPlugin)

public:
  void init(Context&& ctx) override;

  QString name() const override { return tr("Any zoom"); }
  QString description() const override;

  ClockPluginInstance* instance(size_t idx) override;

public slots:
  void configure(QWidget* parent, size_t idx) override;

private:
  std::unique_ptr<AnyZoomPlugin> createInstance(size_t idx);
  AnyZoomPluginInstanceConfig* instanceConfig(size_t idx);

private:
  std::unique_ptr<PluginSettingsBackend> _st;
  std::map<size_t, std::unique_ptr<AnyZoomPlugin>> _insts;
  std::map<size_t, std::unique_ptr<AnyZoomPluginInstanceConfig>> _cfgs;
};
