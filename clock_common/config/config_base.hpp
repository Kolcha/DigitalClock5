// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "clock_common_global.hpp"
#include "settings.hpp"

#define CONFIG_OPTION(name, key, type, def_value)                     \
  void set##name(const type& val) { ConfigBase::setValue(key, val); } \
  type get##name() const { return ConfigBase::value(key, def_value); }

class CLOCK_COMMON_EXPORT ConfigBase
{
public:
  explicit ConfigBase(std::unique_ptr<SettingsStorage> st) noexcept
    : _st(std::move(st))
  {}

  virtual ~ConfigBase() = default;

protected:
  SettingsStorage* storage() const noexcept { return _st.get(); }

  template<typename T>
  struct toVariant {
    QVariant operator()(const T& v) { return QVariant::fromValue(v); }
  };

  template<typename T>
  struct fromVariant {
    T operator()(const QVariant& v) { return qvariant_cast<T>(v); }
  };

  template<typename T>
  void setValue(const QString& key, const T& value)
  {
    _st->setValue(key, toVariant<T>{}(value));
  }

  template<typename T>
  T value(const QString& key, const T& def_value) const
  {
    return fromVariant<T>{}(_st->value(key, toVariant<T>{}(def_value)));
  }

private:
  std::unique_ptr<SettingsStorage> _st;
};
