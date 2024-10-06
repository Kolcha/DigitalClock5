/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "clock_plugin.hpp"

class SamplePlugin : public SettingsPlugin, public ConfigurablePlugin
{
  Q_OBJECT

public:
  SamplePlugin();
  ~SamplePlugin();

  void initSharedSettings(const SharedSettings& s) override;

  void initSettings(PluginSettingsStorage& st) override;

  QWidget* configure(PluginSettingsStorage& s, StateClient& t) override;

public slots:
  void init() override;
  void shutdown() override;

  void tick() override;

  void onOptionChanged(cs::SharedConfigKeys opt, const QVariant& val) override;
};


class SamplePluginFactory : public ClockPluginFactory
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPluginFactory_iid FILE "sample.json")
  Q_INTERFACES(ClockPluginFactory)

public:
  SamplePluginFactory();
  ~SamplePluginFactory();

  std::unique_ptr<ClockPluginBase> create() const override;

  QString title() const override { return tr("Sample Plugin"); }
  QString description() const override;

  bool perClockInstance() const override { return false; }
};
