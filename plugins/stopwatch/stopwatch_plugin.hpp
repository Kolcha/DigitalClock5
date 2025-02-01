/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/text/skinned_text_plugin_base.hpp"

#include <QHotkey>

#include "impl/stopwatch_settings.hpp"
#include "impl/timetracker.hpp"

using timetracker::StopwatchInstanceConfig;

class StopwatchPlugin : public plugin::text::TextPluginInstanceBase
{
  Q_OBJECT

public:
  StopwatchPlugin(const StopwatchInstanceConfig* cfg, size_t idx);
  ~StopwatchPlugin();

public slots:
  void startup() override;
  void shutdown() override;

  void update(const QDateTime& dt) override;

  void applyTimerOption(timetracker::Options opt, const QVariant& val);

protected:
  QString text() const override { return _last_text; }

  SkinnedTextWidget* createWidget(QWidget* parent) const override;

private slots:
  void onWidgetClicked();

private:
  void updateWidgetText();

private:
  std::unique_ptr<timetracker::Timetracker> _tracker;

  const StopwatchInstanceConfig* _cfg;
  QString _last_text;

  std::unique_ptr<QHotkey> _pause_hotkey;
  std::unique_ptr<QHotkey> _restart_hotkey;

  // TRICK! dynamic properties names
  char _last_elapsed_prop[40];
  char _last_active_prop[40];
};


class StopwatchPluginFactory : public plugin::text::TextPluginBase
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "stopwatch.json")
  Q_INTERFACES(ClockPlugin)

public:
  QString name() const override { return tr("Stopwatch"); }
  QString description() const override;

protected:
  Instance createInstance(size_t idx) const override
  {
    auto cfg = qobject_cast<StopwatchInstanceConfig*>(instanceConfig(idx));
    Q_ASSERT(cfg);
    return std::make_unique<StopwatchPlugin>(cfg, idx);
  }

  std::unique_ptr<plugin::text::PluginConfig> createConfig(
      std::unique_ptr<PluginSettingsBackend> b) const override
  {
    using timetracker::StopwatchPluginConfig;
    return std::make_unique<StopwatchPluginConfig>(std::move(b));
  }

  QVector<QWidget*> configPagesBeforeCommonPages() override;
  QVector<QWidget*> configPagesAfterCommonPages() override;
};
