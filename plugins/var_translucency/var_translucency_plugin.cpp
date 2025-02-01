/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "var_translucency_plugin.hpp"

#include <QWidget>

class OpacityChanger
{
public:
  void setOpacity(qreal opacity)
  {
    angle_ = qRadiansToDegrees(qAcos((opacity - 0.55) / 0.45) / k);
  }

  qreal opacity() const
  {
    return 0.55 + 0.45 * qCos(k * qDegreesToRadians(angle_));
  }

  OpacityChanger& operator ++()
  {
    angle_ += 0.5;
    if (angle_ >= 360) angle_ -= 360;
    return *this;
  }

private:
  qreal angle_ = 0.0;
  const qreal k = 2.5;
};


VarTranslucencyPlugin::VarTranslucencyPlugin() = default;
VarTranslucencyPlugin::~VarTranslucencyPlugin() = default;

void VarTranslucencyPlugin::init(QWidget* wnd)
{
  _wnd = wnd->window();
  _last_opacity = _wnd->windowOpacity();
}

void VarTranslucencyPlugin::startup()
{
  _changer = std::make_unique<OpacityChanger>();
  _changer->setOpacity(_last_opacity);
}

void VarTranslucencyPlugin::shutdown()
{
  _changer.reset();
  _wnd->setWindowOpacity(_last_opacity);
}

void VarTranslucencyPlugin::update(const QDateTime& dt)
{
  if (!_changer || !_wnd)
    return;

  ++(*_changer);

  _wnd->setWindowOpacity(_changer->opacity());
}


QString VarTranslucencyPluginFactory::description() const
{
  return tr("Changes clock opacity level with time.");
}

ClockPluginInstance* VarTranslucencyPluginFactory::instance(size_t idx)
{
  auto& inst = _insts[idx];
  if (!inst)
    inst = std::make_unique<VarTranslucencyPlugin>();
  return inst.get();
}
