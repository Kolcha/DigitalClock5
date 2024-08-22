/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "config/settings_storage.hpp"
#include "config/custom_converters.hpp"

namespace plugin_impl {
// convenient settings client class for usage in plugins
// it can be used as is or inherited and used with macros below
template<class C>
class PluginConfig {
public:
  template<typename T>
  using toVariant = C::template toVariant<T>;

  template<typename T>
  using fromVariant = C::template fromVariant<T>;

  explicit PluginConfig(C& c) : _c(c) {}

  template<typename T>
  T get(const QString& k, const QVariant& d) const
  {
    return fromVariant<T>{}(_c.value(k, d));
  }

  template<typename T>
  void set(const QString& k, const T& v)
  {
    _c.setValue(k, toVariant<T>{}(v));
  }

private:
  C& _c;
};

#define PLG_CONFIG_OPTION(type, name, key, def_value) \
void set##name(const type& val)                       \
  { this->template set<type>(key, val); }             \
type get##name() const                                \
  { return this->template get<type>(key, def_value); }

#define PLG_CONFIG_OPTION_Q(type, name, def_value)    \
  PLG_CONFIG_OPTION(type, name, QLatin1String(#name), def_value)

#define PLG_CONFIG_SEC_OPTION_Q(sec, name, type, def_value)     \
  PLG_CONFIG_OPTION(type, name, QLatin1String(#sec"/"#name), def_value)

} // namespace plugin_impl
