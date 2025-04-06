/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/text/skinned_text_plugin_base.hpp"

#include <QHotkey>

#include "impl/countdown_timer.hpp"
#include "impl/countdown_timer_settings.hpp"

using countdown_timer::CountdownTimerInstanceConfig;

class QMediaPlayer;

class CountdownTimerPlugin : public plugin::text::TextPluginInstanceBase
{
  Q_OBJECT

public:
  CountdownTimerPlugin(const CountdownTimerInstanceConfig* cfg);
  ~CountdownTimerPlugin();

  void init(const InstanceOptionsHash& settings) override;

public slots:
  void startup() override;
  void shutdown() override;

  void update(const QDateTime& dt) override;

  void onOptionChanged(opt::InstanceOptions o, const QVariant& v) override;

  void applyTimerOption(countdown_timer::Options opt, const QVariant& val);

protected:
  QString text() const override { return _last_text; }

  SkinnedTextWidget* createWidget(QWidget* parent) const override;

  void pluginReloadConfig() override;

private slots:
  void onWidgetClicked();
  void onWidgetDblclicked();

  void initTimer();

  void updateWidgetText();
  void onTimeout();

  void pauseTimer();
  void restartTimer();

private:
  QTimeZone currentTimeZone() const;

private:
  std::unique_ptr<countdown_timer::CountdownTimer> _timer;
  std::unique_ptr<QMediaPlayer> _player;

  const CountdownTimerInstanceConfig* _cfg;
  QString _last_text;

  bool _local_time = true;
  QTimeZone _last_tz;
  // used to detect system tz change
  QTimeZone _last_clock_tz;

  std::unique_ptr<QHotkey> _pause_hotkey;
  std::unique_ptr<QHotkey> _restart_hotkey;
};


class CountdownTimerPluginFactory : public plugin::text::TextPluginBase
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "countdown_timer.json")
  Q_INTERFACES(ClockPlugin)

public:
  QString name() const override { return tr("Countdown timer"); }
  QString description() const override;

protected:
  Instance createInstance(size_t idx) const override
  {
    auto cfg = qobject_cast<CountdownTimerInstanceConfig*>(instanceConfig(idx));
    Q_ASSERT(cfg);
    return std::make_unique<CountdownTimerPlugin>(cfg);
  }

  std::unique_ptr<plugin::text::PluginConfig> createConfig(
    std::unique_ptr<PluginSettingsBackend> b) const override
  {
    using countdown_timer::CountdownTimerPluginConfig;
    return std::make_unique<CountdownTimerPluginConfig>(std::move(b));
  }

  QVector<QWidget*> configPagesBeforeCommonPages() override;
  QVector<QWidget*> configPagesAfterCommonPages() override;
};
