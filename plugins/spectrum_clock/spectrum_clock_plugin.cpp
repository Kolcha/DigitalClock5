/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "spectrum_clock_plugin.hpp"

void SpectrumClockPlugin::init(const InstanceOptionsHash& settings)
{
  _last_tt = settings.value(opt::TextureType).value<tx::TextureType>();
  _last_color = settings.value(opt::TextureColor).value<QColor>();
}

void SpectrumClockPlugin::startup()
{
  emit optionChanged(opt::TextureType, QVariant::fromValue(tx::Color));
  emit optionChanged(opt::TextureColorUseTheme, false);
}

void SpectrumClockPlugin::shutdown()
{
  emit optionChanged(opt::TextureColor, _last_color);
  emit optionChanged(opt::TextureType, _last_tt);
}

void SpectrumClockPlugin::update(const QDateTime& dt)
{
  Q_UNUSED(dt);

  auto color = QColor::fromHsv(_hue, 255, 255);

  if (++_hue == 360) _hue = 0;

  emit optionChanged(opt::TextureColor, color);
}

void SpectrumClockPlugin::onOptionChanged(opt::InstanceOptions opt, const QVariant& val)
{
  Q_UNUSED(opt);
  Q_UNUSED(val);
}


QString SpectrumClockPluginFactory::description() const
{
  return tr("Changes clock color with time.");
}

ClockPluginInstance* SpectrumClockPluginFactory::instance(size_t idx)
{
  auto& inst = _insts[idx];
  if (!inst)
    inst = std::make_unique<SpectrumClockPlugin>();
  return inst.get();
}
