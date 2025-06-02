/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "stopwatch_plugin.hpp"

#include <QCoreApplication>
#include <QMouseEvent>

#include "gui/hotkeys_settings_widget.hpp"
#include "gui/timer_settings_widget.hpp"

namespace {

// THIS IS A TRICK!
// these variables exist just to survive after plugins
// reload on settings changes (it happens in any case)
// but plugin state is reset after application restart
static constexpr const char* const PROP_STATE_ELAPSED = "dcp_timetracker_state_last_elapsed";
static constexpr const char* const PROP_STATE_ACTIVE = "dcp_timetracker_state_last_active";

} // namespace

class StopwatchWidget : public SkinnedTextWidget
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
  : SkinnedTextWidget(parent)
{
}

void StopwatchWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    emit clicked();
  }
  SkinnedTextWidget::mouseReleaseEvent(event);
}

void StopwatchWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    emit doubleClicked();
  }
  SkinnedTextWidget::mouseDoubleClickEvent(event);
}


StopwatchPlugin::StopwatchPlugin(const StopwatchInstanceConfig* cfg, size_t idx)
  : TextPluginInstanceBase(*cfg)
  , _cfg(cfg)
{
  // TRICK! instance-dependant property names
  std::snprintf(_last_elapsed_prop, sizeof(_last_elapsed_prop), "%s_%zu", PROP_STATE_ELAPSED, idx);
  std::snprintf(_last_active_prop, sizeof(_last_active_prop), "%s_%zu", PROP_STATE_ACTIVE, idx);
}

StopwatchPlugin::~StopwatchPlugin() = default;    // TODO: remove

void StopwatchPlugin::startup()
{
  _tracker = std::make_unique<timetracker::Timetracker>();

  // TRICK! restore state from dynamic app properties
  QVariant prop_var = QCoreApplication::instance()->property(_last_elapsed_prop);
  if (prop_var.isValid()) _tracker->setElapsed(prop_var.toInt());
  prop_var = QCoreApplication::instance()->property(_last_active_prop);
  if (prop_var.isValid() && prop_var.toBool()) _tracker->start();

  // state is restored before widget initialization to prevent flickering
  updateWidgetText();

  TextPluginInstanceBase::startup();
}

void StopwatchPlugin::shutdown()
{
  if (_tracker) {
    // TRICK! save state to dynamic app properties
    QCoreApplication::instance()->setProperty(_last_active_prop, _tracker->isActive());
    QCoreApplication::instance()->setProperty(_last_elapsed_prop, _tracker->elapsed());
    _tracker->stop();
  }
  _tracker.reset();
  // shortcuts must be destroyed explicitly...
  // if not, it causes the crash for some reason
  _pause_hotkey.reset();
  _restart_hotkey.reset();

  TextPluginInstanceBase::shutdown();
}

void StopwatchPlugin::update(const QDateTime& dt)
{
  updateWidgetText();
  TextPluginInstanceBase::update(dt);
}

void StopwatchPlugin::applyTimerOption(timetracker::Options opt, const QVariant& val)
{
  auto init_hotkey = [](const auto& key_seq, auto receiver, auto method) {
    std::unique_ptr<QHotkey> hotkey;
    if (!key_seq.isEmpty()) {
      hotkey = std::make_unique<QHotkey>(QKeySequence(key_seq), true);
      QObject::connect(hotkey.get(), &QHotkey::activated, receiver, method);
    }
    return hotkey;
  };

  using timetracker::Timetracker;
  namespace cd = timetracker;
  switch (opt) {
    case cd::HideInactive:
      if (val.toBool()) {
        connect(_tracker.get(), &Timetracker::activityChanged, widget(), &QWidget::setVisible);
        widget()->setVisible(_tracker->isActive());
      } else {
        disconnect(_tracker.get(), &Timetracker::activityChanged, widget(), &QWidget::setVisible);
        widget()->setVisible(true);
      }
      break;

    case cd::PauseHotkey:
      _pause_hotkey = init_hotkey(val.toString(), this, &StopwatchPlugin::onWidgetClicked);
      break;
    case cd::RestartHotkey:
      _restart_hotkey = init_hotkey(val.toString(), _tracker.get(), &Timetracker::reset);
      break;
  }
}

SkinnedTextWidget* StopwatchPlugin::createWidget(QWidget* parent) const
{
  using timetracker::Timetracker;
  auto w = new StopwatchWidget(parent);
  connect(w, &StopwatchWidget::clicked, this, &StopwatchPlugin::onWidgetClicked);
  connect(w, &StopwatchWidget::doubleClicked, _tracker.get(), &Timetracker::reset);
  return w;
}

void StopwatchPlugin::pluginReloadConfig()
{
  // options may depend on widget
  namespace cd = timetracker;
  applyTimerOption(cd::HideInactive, _cfg->getHideInactive());

  applyTimerOption(cd::PauseHotkey, _cfg->getPauseHotkey());
  applyTimerOption(cd::RestartHotkey, _cfg->getRestartHotkey());
}

void StopwatchPlugin::onWidgetClicked()
{
  if (_tracker->isActive()) {
    _tracker->stop();
  } else {
    _tracker->start();
  }
}

void StopwatchPlugin::updateWidgetText()
{
  int s = _tracker->elapsed();
  int h = s / 3600;
  s -= h * 3600;
  int m = s / 60;
  s -= m * 60;
  _last_text = QString("%1:%2:%3").arg(h).arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
}


QString StopwatchPluginFactory::description() const
{
  return tr("Very simple stopwatch.\n"
            "Single click to start/pause.\n"
            "Double click to reset/restart.");
}

QVector<QWidget*> StopwatchPluginFactory::configPagesBeforeCommonPages()
{
  using timetracker::TimerSettingsWidget;
  auto page = new TimerSettingsWidget();
  connect(this, &StopwatchPluginFactory::instanceSwitched, page, [this, page](size_t idx) {
    page->initControls(qobject_cast<StopwatchInstanceConfig*>(instanceConfig(idx)));
    if (!hasInstances()) return;
    disconnect(page, &TimerSettingsWidget::optionChanged, nullptr, nullptr);
    auto inst = qobject_cast<StopwatchPlugin*>(instance(idx));
    connect(page, &TimerSettingsWidget::optionChanged, inst, &StopwatchPlugin::applyTimerOption);
  });
  return { page };
}

QVector<QWidget*> StopwatchPluginFactory::configPagesAfterCommonPages()
{
  using timetracker::HotkeysSettingsWidget;
  auto page = new HotkeysSettingsWidget();
  connect(this, &StopwatchPluginFactory::instanceSwitched, page, [this, page](size_t idx) {
    page->initControls(qobject_cast<StopwatchInstanceConfig*>(instanceConfig(idx)));
    if (!hasInstances()) return;
    disconnect(page, &HotkeysSettingsWidget::optionChanged, nullptr, nullptr);
    auto inst = qobject_cast<StopwatchPlugin*>(instance(idx));
    connect(page, &HotkeysSettingsWidget::optionChanged, inst, &StopwatchPlugin::applyTimerOption);
  });
  return { page };
}

#include "stopwatch_plugin.moc"
