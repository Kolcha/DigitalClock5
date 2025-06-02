/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "random_position_plugin.hpp"

#include <QScreen>
#include <QWidget>

void RandomPositionPlugin::init(QWidget* wnd)
{
  _wnd = wnd->window();
  _last_pos = _wnd->pos();
}

void RandomPositionPlugin::startup()
{
  std::srand(std::time(nullptr));
}

void RandomPositionPlugin::shutdown()
{
  _wnd->move(_last_pos);
}

void RandomPositionPlugin::update(const QDateTime& dt)
{
  if (!_wnd || _interval_counter-- > 0)
    return;

  _interval_counter = std::rand() % 600 + 60;

  auto screen = _wnd->screen();
  Q_ASSERT(screen);
  const auto& sg = screen->availableGeometry();
  const auto& wg = _wnd->frameGeometry();
  _wnd->move(sg.x() + std::rand() % (sg.width() - wg.width()),
             sg.y() + std::rand() % (sg.height() - wg.height()));
}


QString RandomPositionPluginFactory::description() const
{
  return tr("Randomly changes clock position with time.");
}

ClockPluginInstance* RandomPositionPluginFactory::instance(size_t idx)
{
  auto& inst = _insts[idx];
  if (!inst)
    inst = std::make_unique<RandomPositionPlugin>();
  return inst.get();
}
