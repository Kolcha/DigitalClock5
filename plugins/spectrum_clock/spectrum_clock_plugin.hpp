/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "clock_plugin.hpp"

#include <QBrush>

class SpectrumClockPlugin : public SettingsPlugin
{
  Q_OBJECT

public:
  void initSharedSettings(const SharedSettings& s) override;

public slots:
  void init() override;
  void shutdown() override;

  void tick() override;

  void onOptionChanged(cs::SharedConfigKeys opt, const QVariant& val) override;

private:
  bool _is_active = false;
  QBrush _last_texture;
  int _hue = 0;
};


class SpectrumClockPluginFactory : public ClockPluginFactory
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPluginFactory_iid FILE "spectrum_clock.json")
  Q_INTERFACES(ClockPluginFactory)

public:
  std::unique_ptr<ClockPluginBase> create() const override;

  QString title() const override { return tr("\"Spectrum clock\""); }
  QString description() const override;

  bool perClockInstance() const override { return true; }
};
