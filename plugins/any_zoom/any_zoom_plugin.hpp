/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "clock_plugin.hpp"

class AnyZoomPluginImpl;

class AnyZoomPlugin : public SettingsPlugin, public ConfigurablePlugin
{
  Q_OBJECT

public:
  AnyZoomPlugin();
  ~AnyZoomPlugin();

  void initSharedSettings(const SharedSettings& s) override;

  void saveState(StateClient& st) const override { Q_UNUSED(st); }
  void loadState(const StateClient& st) override { Q_UNUSED(st); }

  void initSettings(PluginSettingsStorage& st) override;

  QWidget* configure(PluginSettingsStorage& s, StateClient& t) override;

public slots:
  void init() override;
  void shutdown() override;

  void onOptionChanged(cs::SharedConfigKeys opt, const QVariant& val) override;

private:
  std::unique_ptr<AnyZoomPluginImpl> _impl;
};


class AnyZoomPluginFactory : public ClockPluginFactory
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPluginFactory_iid FILE "any_zoom.json")
  Q_INTERFACES(ClockPluginFactory)

public:
  std::unique_ptr<ClockPluginBase> create() const override;

  QString title() const override { return tr("Any zoom"); }
  QString description() const override;

  bool perClockInstance() const noexcept override { return true; }
};
