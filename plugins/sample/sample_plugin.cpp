/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "sample_plugin.hpp"

#include <QDebug>
#include <QBrush>
#include <QPushButton>

SamplePlugin::SamplePlugin()
{
  qDebug() << __FUNCTION__;
}

SamplePlugin::~SamplePlugin()
{
  qDebug() << __FUNCTION__;
}

void SamplePlugin::initSharedSettings(const SharedSettings& s)
{
  qDebug() << __FUNCTION__;
}

void SamplePlugin::saveState(StateClient& st) const
{
  st.setValue("testA", 84);
  qDebug() << __FUNCTION__;
}

void SamplePlugin::loadState(const StateClient& st)
{
  qDebug() << __FUNCTION__ << st.value("testA", 42);
}

void SamplePlugin::initSettings(const SettingsClient& st)
{
  qDebug() << __FUNCTION__ << st.value("V1", 25);
}

QWidget* SamplePlugin::configure(SettingsClient& s, StateClient& t)
{
  qDebug() << __FUNCTION__;
  s.setValue("V1", quintptr(this));
  return new QPushButton(QString::number(quintptr(this)));
}

void SamplePlugin::init()
{
  qDebug() << __FUNCTION__;
  emit optionChanged(cs::Texture, QBrush(Qt::red));
}

void SamplePlugin::shutdown()
{
  qDebug() << __FUNCTION__;
}

void SamplePlugin::tick()
{
  qDebug() << __FUNCTION__;
}

void SamplePlugin::onOptionChanged(clock_settings::SharedConfigKeys opt, const QVariant& val)
{
  qDebug() << __FUNCTION__ << opt << val;
}


SamplePluginFactory::SamplePluginFactory()
{
  qDebug() << __FUNCTION__;
}

SamplePluginFactory::~SamplePluginFactory()
{
  qDebug() << __FUNCTION__;
}

std::unique_ptr<ClockPluginBase> SamplePluginFactory::create() const
{
  qDebug() << __FUNCTION__;
  return std::make_unique<SamplePlugin>();
}

QString SamplePluginFactory::description() const
{
  return tr("Test plugin just for debugging and testing purposes. "
            "Also can be used as minimal example. Does nothing.");
}
