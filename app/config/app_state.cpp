// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app_state.hpp"

AppState::AppState(SettingsBackend& st)
  : _st(st)
{
  auto gsst = std::make_unique<SettingsStorageImpl>(std::ref(_st));
  _global = std::make_unique<PrefixedSettingsStorage>(std::move(gsst), "state");
}

SettingsStorage* AppState::instance(size_t i) const
{
  auto& isst = _instances[i];
  if (!isst)
    isst = createInstance(i);
  return isst.get();
}

std::unique_ptr<SettingsStorage> AppState::createInstance(size_t i) const
{
  auto isst = std::make_unique<SettingsStorageImpl>(std::ref(_st));
  auto prefix = QString("state/instance_%1").arg(i);
  return std::make_unique<PrefixedSettingsStorage>(std::move(isst), prefix);
}
