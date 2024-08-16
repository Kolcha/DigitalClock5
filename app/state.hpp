/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "config/settings_storage.hpp"

class State {
public:
  virtual ~State() = default;

  virtual void setValue(QString key, QVariant val) = 0;
  virtual QVariant value(QString key, QVariant def) const = 0;
};


// this is maybe overkill, but window should not know
// about any config implementation details
class StateImpl : public State {
public:
  explicit StateImpl(StateClient& scl) : _scl(scl) {}

  void setValue(QString key, QVariant val) override
  {
    _scl.setValue(std::move(key), std::move(val));
  }

  QVariant value(QString key, QVariant def) const override
  {
    return _scl.value(std::move(key), std::move(def));
  }

public:
  StateClient& _scl;
};
