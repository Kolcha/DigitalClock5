/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "stopwatch_plugin.hpp"

#include <QMouseEvent>

#include "gui/hotkeys_settings_widget.hpp"
#include "gui/timer_settings_widget.hpp"

class StopwatchWidget : public GraphicsTextWidget
{
  Q_OBJECT

public:
  explicit StopwatchWidget(QWidget* parent = nullptr);

signals:
  void clicked();
  void doubleClicked();

protected:
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseDoubleClickEvent(QMouseEvent* event) override;
};


StopwatchWidget::StopwatchWidget(QWidget* parent)
    : GraphicsTextWidget(parent)
{
}

void StopwatchWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    emit clicked();
  }
  GraphicsTextWidget::mouseReleaseEvent(event);
}

void StopwatchWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    emit doubleClicked();
  }
  GraphicsTextWidget::mouseDoubleClickEvent(event);
}


StopwatchPlugin::StopwatchPlugin()
    : _impl(std::make_unique<StopwatchPluginImpl>())
{
}

StopwatchPlugin::~StopwatchPlugin() = default;

void StopwatchPlugin::saveState(StateClient& st) const
{
  st.setValue("last_active", _impl->tracker->isActive());
  st.setValue("last_elapsed", _impl->tracker->elapsed());
}

void StopwatchPlugin::loadState(const StateClient& st)
{
  _impl->last_active = st.value("last_active", false).toBool();
  _impl->last_elapsed = st.value("last_elapsed", 0).toInt();
}

void StopwatchPlugin::initSettings(PluginSettingsStorage& st)
{
  _impl->initSettings(st);
  WidgetPluginBase::initSettings(st);
}

void StopwatchPlugin::init()
{
  _impl->tracker = std::make_unique<timetracker::Timetracker>();
  _impl->tracker->setElapsed(_impl->last_elapsed);
  if (_impl->last_active) _impl->tracker->start();

  connect(_impl->pause_hotkey.get(), &QHotkey::activated, this, &StopwatchPlugin::onWidgetClicked);
  connect(_impl->restart_hotkey.get(), &QHotkey::activated, _impl->tracker.get(), &timetracker::Timetracker::reset);

  WidgetPluginBase::init();

  _impl->applySettings();
}

void StopwatchPlugin::shutdown()
{
  // shortcuts must be destroyed explicitly...
  // if not, it causes the crash for some reason
  _impl->pause_hotkey.reset();
  _impl->restart_hotkey.reset();
  WidgetPluginBase::shutdown();
}

void StopwatchPlugin::tick()
{
  if (!_impl->widget) return;

  int s = _impl->tracker->elapsed();
  int h = s / 3600;
  s -= h * 3600;
  int m = s / 60;
  s -= m * 60;

  auto t = QString("%1:%2:%3").arg(h).arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
  _impl->widget->setText(std::move(t));

  WidgetPluginBase::tick();
}

QList<QWidget*> StopwatchPlugin::customConfigure(PluginSettingsStorage& s, StateClient& t)
{
  auto timer_tab = new TimerSettingsWidget(s, t, _impl.get());
  auto hotkeys_tab = new HotkeysSettingsWidget(s, t, _impl.get());
  return {timer_tab, hotkeys_tab};
}

std::shared_ptr<GraphicsWidgetBase> StopwatchPlugin::createWidget()
{
  auto w = std::make_shared<StopwatchWidget>();

  connect(w.get(), &StopwatchWidget::clicked, this, &StopwatchPlugin::onWidgetClicked);
  connect(w.get(), &StopwatchWidget::doubleClicked, _impl->tracker.get(), &timetracker::Timetracker::reset);

  _impl->widget = std::move(w);
  tick();
  return _impl->widget;
}

void StopwatchPlugin::destroyWidget()
{
  _impl->widget.reset();
}

void StopwatchPlugin::onWidgetClicked()
{
  if (_impl->tracker->isActive()) {
    _impl->tracker->stop();
  } else {
    _impl->tracker->start();
  }
}


std::unique_ptr<ClockPluginBase> StopwatchPluginFactory::create() const
{
  return std::make_unique<StopwatchPlugin>();
}

QString StopwatchPluginFactory::description() const
{
  return tr("Very simple stopwatch.\n"
            "Single click to start/pause.\n"
            "Double click to reset/restart.");
}

#include "stopwatch_plugin.moc"
