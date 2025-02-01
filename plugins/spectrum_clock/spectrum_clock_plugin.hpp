/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/clock_plugin.hpp"

#include <QtGui/QColor>

class SpectrumClockPlugin : public SettingsPluginInstance
{
  Q_OBJECT

public:
  void init(const InstanceOptionsHash& settings) override;

public slots:
  void startup() override;
  void shutdown() override;

  void update(const QDateTime& dt) override;

  void onOptionChanged(opt::InstanceOptions opt, const QVariant& val) override;

private:
  tx::TextureType _last_tt = tx::None;
  QColor _last_color;
  int _hue = 0;
};


class SpectrumClockPluginFactory : public ClockPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "spectrum_clock.json")
  Q_INTERFACES(ClockPlugin)

public:
  void init(Context&& ctx) override {}

  QString name() const override { return QString("\"Spectrum clock\""); }
  QString description() const override;

  ClockPluginInstance* instance(size_t idx) override;

public slots:
  void configure(QWidget* parent, size_t idx) override {}

private:
  std::map<size_t, std::unique_ptr<SpectrumClockPlugin>> _insts;
};
