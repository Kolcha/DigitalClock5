/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "random_position_plugin.hpp"

#include <QScreen>
#include <QWidget>

void RandomPositionPlugin::initWindow(QWidget* wnd)
{
  _wnd = wnd->window();
  _last_pos = _wnd->pos();
}

void RandomPositionPlugin::init()
{
  std::srand(std::time(0));
  _is_active = true;
}

void RandomPositionPlugin::shutdown()
{
  _is_active = false;
  _wnd->move(_last_pos);
}

void RandomPositionPlugin::tick()
{
  if (!_is_active || !_wnd || _interval_counter-- > 0)
    return;

  _interval_counter = std::rand() % 600 + 60;

  auto screen = _wnd->screen();
  Q_ASSERT(screen);
  const auto& sg = screen->availableGeometry();
  const auto& wg = _wnd->frameGeometry();
  _wnd->move(sg.x() + std::rand() % (sg.width() - wg.width()),
             sg.y() + std::rand() % (sg.height() - wg.height()));
}


std::unique_ptr<ClockPluginBase> RandomPositionPluginFactory::create() const
{
  return std::make_unique<RandomPositionPlugin>();
}

QString RandomPositionPluginFactory::description() const
{
  return tr("Randomly changes clock position during time.");
}
