/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "any_zoom_plugin.hpp"

#include "gui/settings_widget.hpp"
#include "impl/any_zoom_plugin_impl.hpp"

AnyZoomPlugin::AnyZoomPlugin()
    : _impl(std::make_unique<AnyZoomPluginImpl>())
{
}

AnyZoomPlugin::~AnyZoomPlugin() = default;

void AnyZoomPlugin::initSharedSettings(const SharedSettings& s)
{
  _impl->clock_zoom = s[cs::ScalingH].toInt();  // H and V are the same
}

void AnyZoomPlugin::initSettings(PluginSettingsStorage& st)
{
  _impl->initSettings(st);
}

QWidget* AnyZoomPlugin::configure(PluginSettingsStorage& s, StateClient& t)
{
  return new SettingsWidget(s, t, _impl.get());
}

void AnyZoomPlugin::init()
{
  connect(_impl.get(), &AnyZoomPluginImpl::optionChanged, this, &AnyZoomPlugin::optionChanged);
  emit optionChanged(cs::ScalingH, _impl->zoom_x);
  emit optionChanged(cs::ScalingV, _impl->zoom_y);
}

void AnyZoomPlugin::shutdown()
{
  disconnect(_impl.get(), &AnyZoomPluginImpl::optionChanged, nullptr, nullptr);
  emit optionChanged(cs::ScalingH, _impl->clock_zoom);
  emit optionChanged(cs::ScalingV, _impl->clock_zoom);
}

void AnyZoomPlugin::onOptionChanged(clock_settings::SharedConfigKeys opt, const QVariant& val)
{
  Q_UNUSED(opt);
  Q_UNUSED(val);
}


std::unique_ptr<ClockPluginBase> AnyZoomPluginFactory::create() const
{
  return std::make_unique<AnyZoomPlugin>();
}

QString AnyZoomPluginFactory::description() const
{
  return tr("Allows to set any clock zoom, even different values for X and Y axis.");
}
