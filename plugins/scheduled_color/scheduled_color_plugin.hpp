/*
 * SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/clock_plugin.hpp"
#include "impl/scheduled_color_settings.hpp"

#include <QtGui/QColor>
#include <memory>
#include <vector>

class ScheduledColorPlugin : public SettingsPluginInstance
{
  Q_OBJECT

public:
  explicit ScheduledColorPlugin(SettingsStorage* settings);

  void init(const InstanceOptionsHash& settings) override;

public slots:
  void startup() override;
  void shutdown() override;

  void update(const QDateTime& dt) override;

  void onOptionChanged(opt::InstanceOptions opt, const QVariant& val) override;

  void loadSettings();

private:
  void saveSettings();
  void applyColor(const QColor& color);
  void restoreColor();
  int findActiveTimePoint(const QTime& current_time) const;

private:
  SettingsStorage* _settings;
  tx::TextureType _last_tt = tx::None;
  QColor _last_color;
  std::vector<scheduled_color::TimeColorPoint> _time_points;
  int _current_active_point = -1;
  QTime _active_until;
  bool _color_applied = false;
};


class ScheduledColorPluginFactory : public ClockPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "scheduled_color.json")
  Q_INTERFACES(ClockPlugin)

public:
  void init(Context&& ctx) override;

  QString name() const override { return tr("Scheduled Color"); }
  QString description() const override;

  ClockPluginInstance* instance(size_t idx) override;

public slots:
  void configure(QWidget* parent, size_t idx) override;

private:
  size_t _first_idx = 0;
  std::unique_ptr<SettingsStorage> _settings;
  std::unique_ptr<ScheduledColorPlugin> _inst;
};
