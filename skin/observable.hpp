/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <vector>

template<class Observer>
class Observable {
public:
  void subscribe(Observer* observer)
  {
    _observers.push_back(observer);
  }

  void unsubscribe(Observer* observer)
  {
    std::erase(_observers, observer);
  }

protected:
  template<typename Method, typename... Args>
  void notify(Method method, Args&&... args) const
  {
    for (const auto& o : _observers)
      (*o.*method)(std::forward<Args>(args)...);
  }

private:
  std::vector<Observer*> _observers;
};
