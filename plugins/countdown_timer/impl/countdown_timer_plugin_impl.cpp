/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "countdown_timer_plugin_impl.hpp"

#include "countdown_timer_settings.hpp"
#include "utilities.hpp"

void CountdownTimerPluginImpl::initSettings(const SettingsClient& s)
{
  pause_hotkey = std::make_unique<QHotkey>();
  restart_hotkey = std::make_unique<QHotkey>();

  plugin_impl::CountdownTimerPluginSettings cfg(s);

  interval_hours = cfg.getIntervalHours();
  interval_minutes = cfg.getIntervalMinutes();
  interval_seconds = cfg.getIntervalSeconds();

  use_target_datetime = cfg.getUseTargetDateTime();
  target_date_time = cfg.getTargetDateTime();

  chime_on_timeout = cfg.getChimeOnTimeout();
  chime_sound = cfg.getChimeSoundFile();

  show_message = cfg.getShowMessage();
  message_text = cfg.getMessageText();

  hide_days_threshold = cfg.getHideDaysThreshold();
  also_hide_hours = cfg.getAlsoHideHours();

  restart_on_dbl_click = cfg.getRestartOnDblClick();
  restart_on_timeout = cfg.getChimeOnTimeout();

  if (!cfg.getPauseHotkey().isEmpty())
    pause_hotkey->setShortcut(QKeySequence(cfg.getPauseHotkey()), true);
  if (!cfg.getRestartHotkey().isEmpty())
    restart_hotkey->setShortcut(QKeySequence(cfg.getRestartHotkey()), true);

  hide_inactive = cfg.getHideInactive();
  reverse_counting = cfg.getReverseCounting();
}

void CountdownTimerPluginImpl::applySettings()
{
  if (!timer || !widget)
    return;

  if (hide_inactive)
    QObject::connect(timer.get(), &countdown_timer::CountdownTimer::activityChanged,
                     widget.get(), &QWidget::setVisible);
  else
    QObject::disconnect(timer.get(), &countdown_timer::CountdownTimer::activityChanged,
                        widget.get(), &QWidget::setVisible);
}

void CountdownTimerPluginImpl::initTimer()
{
  if (!timer) return;

  if (use_target_datetime) {
    auto now = QDateTime::currentDateTime();
    now = now.addMSecs(-now.time().msec());
    if (target_date_time < now) {
      target_date_time = plugin_impl::default_target_date();
      // update config
    }
    if (target_date_time > now) {
      timer->setInterval(now.secsTo(target_date_time));
      timer->start();
    }
  } else {
    qint64 timeout = interval_seconds;
    timeout += 60 * interval_minutes;
    timeout += 3600 * interval_hours;
    timer->setInterval(timeout);
  }
}

QString CountdownTimerPluginImpl::widgetText() const
{
  qint64 counter = reverse_counting ? timer->interval() - timer->timeLeft() : timer->timeLeft();
  return ::countdown_timer::format_time(counter, hide_days_threshold, also_hide_hours);
}

void CountdownTimerPluginImpl::updateWidgetText()
{
  if (!widget) return;
  widget->setText(widgetText());
}
