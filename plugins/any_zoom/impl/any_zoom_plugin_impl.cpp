/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "any_zoom_plugin_impl.hpp"

#include "any_zoom_settings.hpp"

AnyZoomPluginImpl::AnyZoomPluginImpl(QObject* parent)
    : QObject(parent)
{
}

void AnyZoomPluginImpl::initSettings(PluginSettingsStorage& st)
{
  plugin_impl::AnyZoomPluginSettings cfg(st);
  zoom_x = cfg.getZoomX();
  zoom_y = cfg.getZoomY();
  emit optionChanged(cs::ScalingH, zoom_x);
  emit optionChanged(cs::ScalingV, zoom_y);
}
