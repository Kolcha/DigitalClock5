/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "plugin/text/skinned_text_plugin_base.hpp"

using namespace plugin::text;

class SamplePlugin : public TextPluginInstanceBase
{
  Q_OBJECT

public:
  SamplePlugin(const PluginInstanceConfig& cfg);
  ~SamplePlugin();

protected:
  QString text() const override { return "Hello from plugin!"; }
  void pluginReloadConfig() override { /* does nothing */ }
};


class SamplePluginFactory : public TextPluginBase
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "sample.json")
  Q_INTERFACES(ClockPlugin)

public:
  SamplePluginFactory();
  ~SamplePluginFactory();

  QString name() const override { return tr("Sample Plugin"); }
  QString description() const override;

protected:
  Instance createInstance(size_t idx) const override
  {
    return std::make_unique<SamplePlugin>(*instanceConfig(idx));
  }
};
