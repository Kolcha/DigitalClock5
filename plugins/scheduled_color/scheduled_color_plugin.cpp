/*
 * SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "scheduled_color_plugin.hpp"
#include "gui/settings_dialog.hpp"

#include <algorithm>

using namespace scheduled_color;

ScheduledColorPlugin::ScheduledColorPlugin(SettingsStorage* settings)
  : _settings(settings)
{
}

void ScheduledColorPlugin::init(const InstanceOptionsHash& settings)
{
  _last_tt = settings.value(opt::TextureType).value<tx::TextureType>();
  _last_color = settings.value(opt::TextureColor).value<QColor>();
}

void ScheduledColorPlugin::startup()
{
  loadSettings();
}

void ScheduledColorPlugin::shutdown()
{
  restoreColor();
}

void ScheduledColorPlugin::update(const QDateTime& dt)
{
  if (_time_points.empty())
    return;

  QTime current_time = dt.time();

  if (_color_applied && !_active_until.isNull()) {
    if (current_time >= _active_until) {
      restoreColor();
      _color_applied = false;
      _current_active_point = -1;
      _active_until = QTime();
    }
  }

  if (!_color_applied) {
    int point_idx = findActiveTimePoint(current_time);

    if (point_idx >= 0) {
      const auto& point = _time_points[point_idx];
      applyColor(point.color);
      _current_active_point = point_idx;
      _color_applied = true;

      switch (point.mode) {
        case TimeColorPoint::FixedDuration:
          _active_until = current_time.addSecs(point.duration_seconds);
          break;

        case TimeColorPoint::UntilTime:
          _active_until = point.until_time;
          if (_active_until < current_time) {
            _active_until = _active_until.addSecs(24 * 3600);
          }
          break;

        case TimeColorPoint::UntilNextPoint:
          if (point_idx + 1 < static_cast<int>(_time_points.size())) {
            _active_until = _time_points[point_idx + 1].time;
          } else {
            _active_until = QTime();
          }
          break;
      }
    }
  }
}

void ScheduledColorPlugin::onOptionChanged(opt::InstanceOptions opt, const QVariant& val)
{
  Q_UNUSED(opt);
  Q_UNUSED(val);
}

void ScheduledColorPlugin::loadSettings()
{
  if (!_settings)
    return;

  _time_points.clear();

  int count = _settings->value("time_points_count", 0).toInt();

  for (int i = 0; i < count; ++i) {
    QString prefix = QString("point_%1_").arg(i);

    TimeColorPoint point;
    point.time = QTime::fromString(_settings->value(prefix + "time", "00:00").toString(), "HH:mm");
    point.color = QColor(_settings->value(prefix + "color", "#FFFFFF").toString());
    point.mode = static_cast<TimeColorPoint::DurationMode>(_settings->value(prefix + "mode", 2).toInt());
    point.until_time = QTime::fromString(_settings->value(prefix + "until_time", "00:00").toString(), "HH:mm");
    point.duration_seconds = _settings->value(prefix + "duration", 0).toInt();

    _time_points.push_back(point);
  }

  std::sort(_time_points.begin(), _time_points.end());
}

void ScheduledColorPlugin::saveSettings()
{
  if (!_settings)
    return;

  _settings->setValue("time_points_count", static_cast<int>(_time_points.size()));

  for (size_t i = 0; i < _time_points.size(); ++i) {
    QString prefix = QString("point_%1_").arg(i);
    const auto& point = _time_points[i];

    _settings->setValue(prefix + "time", point.time.toString("HH:mm"));
    _settings->setValue(prefix + "color", point.color.name());
    _settings->setValue(prefix + "mode", static_cast<int>(point.mode));
    _settings->setValue(prefix + "until_time", point.until_time.toString("HH:mm"));
    _settings->setValue(prefix + "duration", point.duration_seconds);
  }

  _settings->commit();
}

void ScheduledColorPlugin::applyColor(const QColor& color)
{
  emit optionChanged(opt::TextureColorUseTheme, false);
  emit optionChanged(opt::TextureType, QVariant::fromValue(tx::Color));
  emit optionChanged(opt::TextureColor, color);
}

void ScheduledColorPlugin::restoreColor()
{
  emit optionChanged(opt::TextureColor, _last_color);
  emit optionChanged(opt::TextureType, _last_tt);
}

int ScheduledColorPlugin::findActiveTimePoint(const QTime& current_time) const
{
  for (int i = static_cast<int>(_time_points.size()) - 1; i >= 0; --i) {
    if (auto ms = _time_points[i].time.msecsTo(current_time); 0 <= ms && ms < 1000) {
      return i;
    }
  }
  return -1;
}


QString ScheduledColorPluginFactory::description() const
{
  return tr("Changes clock color at specified times with configurable durations.");
}

void ScheduledColorPluginFactory::init(Context&& ctx)
{
  _first_idx = ctx.active_instances.front();
  _settings = std::move(ctx.settings);
}

void ScheduledColorPluginFactory::configure(QWidget* parent, size_t idx)
{
  Q_UNUSED(idx);

  auto* dialog = new SettingsDialog(parent);
  dialog->initControls(_settings.get());

  connect(dialog, &SettingsDialog::accepted, _settings.get(), &SettingsStorage::commit);
  connect(dialog, &SettingsDialog::rejected, _settings.get(), &SettingsStorage::discard);

  connect(dialog, &QDialog::finished, [this]() {
    if (_inst) {
      _inst->loadSettings();
    }
  });
  connect(dialog, &QDialog::finished, dialog, &QObject::deleteLater);

  dialog->show();
}

ClockPluginInstance* ScheduledColorPluginFactory::instance(size_t idx)
{
  if (idx != _first_idx)
    return nullptr;

  if (!_inst)
    _inst = std::make_unique<ScheduledColorPlugin>(_settings.get());

  return _inst.get();
}
