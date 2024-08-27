/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "spectrum_clock_plugin.hpp"

void SpectrumClockPlugin::initSharedSettings(const SharedSettings& s)
{
  _last_texture = s[cs::Texture].value<QBrush>();
}

void SpectrumClockPlugin::init()
{
  _is_active = true;
  tick();
}

void SpectrumClockPlugin::shutdown()
{
  _is_active = false;
  emit optionChanged(cs::Texture, _last_texture);
}

void SpectrumClockPlugin::tick()
{
  if (!_is_active) return;

  auto color = QColor::fromHsv(_hue, 255, 255);

  if (++_hue == 360) _hue = 0;

  emit optionChanged(cs::Texture, color);
}

void SpectrumClockPlugin::onOptionChanged(clock_settings::SharedConfigKeys opt, const QVariant& val)
{
  Q_UNUSED(opt);
  Q_UNUSED(val);
}


std::unique_ptr<ClockPluginBase> SpectrumClockPluginFactory::create() const
{
  return std::make_unique<SpectrumClockPlugin>();
}

QString SpectrumClockPluginFactory::description() const
{
  return tr("Changes clock color during time.");
}
