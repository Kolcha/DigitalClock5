/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "app_config.hpp"

WindowConfig::WindowConfig(int id, AppConfig& cfg)
    : _id(id)
    , _cfg(cfg)
    , _appearance(title(u"appearance"), cfg.storage())
    , _generic(title(u"generic"), cfg.storage())
    , _state(QString("state/window/%1").arg(id), cfg.storage())
{
}

const SectionAppearance& WindowConfig::appearance() const
{
  if (!_cfg.global().getAppearancePerInstance() && this != &_cfg.window(0))
    return _cfg.window(0).appearance();
  return _appearance;
}

SectionAppearance& WindowConfig::appearance()
{
  return const_cast<SectionAppearance&>(std::as_const(*this).appearance());
}

QString WindowConfig::title(QStringView t) const
{
  return QString("Window%1/%2").arg(_id).arg(t);
}


PluginConfig::PluginConfig(QStringView id, AppConfig& cfg)
{
  // this is maybe a bit waste of space, as not every plugin has
  // per-instance configuration, but this significantly simplifies logic
  for (int i = 0; i < cfg.global().getNumInstances(); i++) {
    _configs.emplace_back(QString("plugin/%1/%2").arg(id).arg(i), cfg.storage());
    _states.emplace_back(QString("state/plugin/%1/%2").arg(id).arg(i), cfg.storage());
  }
}


AppConfig::AppConfig()
    : AppConfig(std::make_unique<SettingsStorage>())
{
}

AppConfig::AppConfig(const QString& filename)
    : AppConfig(std::make_unique<SettingsStorage>(filename))
{
}

PluginConfig& AppConfig::plugin(QString id)
{
  return _plugins.try_emplace(id, id, *this).first->second;
}

AppConfig::AppConfig(std::unique_ptr<SettingsStorage> st)
    : _st(std::move(st))
    , _global("app_global", *_st)
    , _limits("limits", *_st)
    , _state("state/app", *_st)
{
  for (int i = 0; i < _global.getNumInstances(); i++)
    _windows.push_back(WindowConfig(i, *this));
}
