/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "sample_plugin.hpp"

#include <QDebug>

SamplePlugin::SamplePlugin(const PluginInstanceConfig& cfg)
  : TextPluginInstanceBase(cfg)
{
  qDebug() << __FUNCTION__;
}

SamplePlugin::~SamplePlugin()
{
  qDebug() << __FUNCTION__;
}


SamplePluginFactory::SamplePluginFactory()
{
  qDebug() << __FUNCTION__;
}

SamplePluginFactory::~SamplePluginFactory()
{
  qDebug() << __FUNCTION__;
}

QString SamplePluginFactory::description() const
{
  return tr("Test plugin just for debugging and testing purposes. "
            "Also can be used as minimal example. Does nothing.");
}
