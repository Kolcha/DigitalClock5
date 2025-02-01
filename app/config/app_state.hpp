// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <map>
#include <memory>

#include "common/settings.hpp"

class AppState final
{
public:
  explicit AppState(SettingsBackend& st);

  // Qt style: const function returns non-const pointer
  SettingsStorage* global() const noexcept { return _global.get(); }
  SettingsStorage* instance(size_t i) const;

private:
  std::unique_ptr<SettingsStorage> createInstance(size_t i) const;

private:
  SettingsBackend& _st;
  std::unique_ptr<SettingsStorage> _global;
  mutable std::map<size_t, std::unique_ptr<SettingsStorage>> _instances;
};
