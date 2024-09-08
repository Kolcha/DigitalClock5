/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "stopwatch_plugin_impl.hpp"

#include "stopwatch_settings.hpp"

void StopwatchPluginImpl::initSettings(PluginSettingsStorage& s)
{
  pause_hotkey = std::make_unique<QHotkey>();
  restart_hotkey = std::make_unique<QHotkey>();

  plugin_impl::StopwatchPluginSettings cfg(s);

  if (!cfg.getPauseHotkey().isEmpty())
    pause_hotkey->setShortcut(QKeySequence(cfg.getPauseHotkey()), true);
  if (!cfg.getRestartHotkey().isEmpty())
    restart_hotkey->setShortcut(QKeySequence(cfg.getRestartHotkey()), true);

  hide_inactive = cfg.getHideInactive();
}

void StopwatchPluginImpl::applySettings()
{
  if (!tracker || !widget)
    return;

  if (hide_inactive)
    QObject::connect(tracker.get(), &timetracker::Timetracker::activityChanged,
                     widget.get(), &QWidget::setVisible);
  else
    QObject::disconnect(tracker.get(), &timetracker::Timetracker::activityChanged,
                        widget.get(), &QWidget::setVisible);
}
