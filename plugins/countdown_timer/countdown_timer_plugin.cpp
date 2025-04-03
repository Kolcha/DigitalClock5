/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "countdown_timer_plugin.hpp"

#include <QMessageBox>
#include <QMouseEvent>

#include <QPushButton>

#include <QAudioOutput>
#include <QMediaPlayer>

#include "gui/hotkeys_settings_widget.hpp"
#include "gui/timer_settings_widget.hpp"
#include "impl/utilities.hpp"

class CountdownTimerWidget : public SkinnedTextWidget
{
  Q_OBJECT

public:
  explicit CountdownTimerWidget(QWidget* parent = nullptr);

signals:
  void clicked();
  void doubleClicked();

protected:
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseDoubleClickEvent(QMouseEvent* event) override;
};


CountdownTimerWidget::CountdownTimerWidget(QWidget* parent)
  : SkinnedTextWidget(parent)
{
}

void CountdownTimerWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    emit clicked();
  }
  SkinnedTextWidget::mouseReleaseEvent(event);
}

void CountdownTimerWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    emit doubleClicked();
  }
  SkinnedTextWidget::mouseDoubleClickEvent(event);
}


CountdownTimerPlugin::CountdownTimerPlugin(const CountdownTimerInstanceConfig* cfg)
  : TextPluginInstanceBase(*cfg)
  , _cfg(cfg)
{
}

void CountdownTimerPlugin::init(const InstanceOptionsHash& settings)
{
  _local_time = settings[opt::ShowLocalTime].toBool();
  _last_tz = settings[opt::TimeZone].value<QTimeZone>();
  TextPluginInstanceBase::init(settings);
}

CountdownTimerPlugin::~CountdownTimerPlugin() = default;

void CountdownTimerPlugin::startup()
{
  using countdown_timer::CountdownTimer;
  _timer = std::make_unique<CountdownTimer>();

  connect(_timer.get(), &CountdownTimer::timeLeftChanged, this, &CountdownTimerPlugin::updateWidgetText);
  connect(_timer.get(), &CountdownTimer::timeout, this, &CountdownTimerPlugin::onTimeout);

  _player = std::make_unique<QMediaPlayer>();
  auto ao = new QAudioOutput(_player.get());
  _player->setAudioOutput(ao);

  TextPluginInstanceBase::startup();
}

void CountdownTimerPlugin::shutdown()
{
  if (_timer->isActive())
    _timer->stop();

  _timer.reset();
  _player.reset();
  // shortcuts must be destroyed explicitly...
  // if not, it causes the crash for some reason
  _pause_hotkey.reset();
  _restart_hotkey.reset();

  TextPluginInstanceBase::shutdown();
}

void CountdownTimerPlugin::onOptionChanged(opt::InstanceOptions o, const QVariant& v)
{
  switch (o) {
    case opt::ShowLocalTime:
      _local_time = v.toBool();
      restartTimer();
      break;
    case opt::TimeZone:
      _last_tz = v.value<QTimeZone>();
      restartTimer();
      break;
    default:
      break;
  }

  TextPluginInstanceBase::onOptionChanged(o, v);
}

void CountdownTimerPlugin::applyTimerOption(countdown_timer::Options opt, const QVariant& val)
{
  auto init_hotkey = [](auto key_seq, auto receiver, auto method) {
    std::unique_ptr<QHotkey> hotkey;
    if (!key_seq.isEmpty()) {
      hotkey = std::make_unique<QHotkey>(QKeySequence(key_seq), true);
      QObject::connect(hotkey.get(), &QHotkey::activated, receiver, method);
    }
    return hotkey;
  };

  namespace cd = countdown_timer;
  switch (opt) {
    case cd::IntervalHours:
    case cd::IntervalMinutes:
    case cd::IntervalSeconds:

    case cd::UseTargetDateTime:
    case cd::TargetDateTime:
      _timer->stop();
      initTimer();
      break;

    case cd::ChimeOnTimeout:
    case cd::ChimeSoundFile:
      // implicitly handled
      break;

    case cd::ShowMessage:
    case cd::MessageText:
      // implicitly handled
      break;

    case cd::ShowLeadingZero:
    case cd::HideDaysThreshold:
    case cd::AlsoHideHours:
      updateWidgetText();
      repaintWidget();
      break;

    case cd::RestartOnDblClick:
    case cd::RestartOnTimeout:
      // implicitly handled
      break;

    case cd::HideInactive:
      using countdown_timer::CountdownTimer;
      if (val.toBool()) {
        connect(_timer.get(), &CountdownTimer::activityChanged, widget(), &QWidget::setVisible);
        widget()->setVisible(_timer->isActive());
      } else {
        disconnect(_timer.get(), &CountdownTimer::activityChanged, widget(), &QWidget::setVisible);
        widget()->setVisible(true);
      }
      break;

    case cd::ReverseCounting:
      updateWidgetText();
      repaintWidget();
      break;

    case cd::PauseHotkey:
      _pause_hotkey = init_hotkey(val.toString(), this, &CountdownTimerPlugin::pauseTimer);
      break;
    case cd::RestartHotkey:
      _restart_hotkey = init_hotkey(val.toString(), this, &CountdownTimerPlugin::restartTimer);
      break;
  }
}

SkinnedTextWidget* CountdownTimerPlugin::createWidget(QWidget* parent) const
{
  auto w = new CountdownTimerWidget(parent);
  connect(w, &CountdownTimerWidget::clicked, this, &CountdownTimerPlugin::onWidgetClicked);
  connect(w, &CountdownTimerWidget::doubleClicked, this, &CountdownTimerPlugin::onWidgetDblclicked);
  return w;
}

void CountdownTimerPlugin::pluginReloadConfig()
{
  initTimer();
  updateWidgetText();

  // options may depend on widget
  namespace cd = countdown_timer;
  applyTimerOption(cd::HideInactive, _cfg->getHideInactive());

  applyTimerOption(cd::PauseHotkey, _cfg->getPauseHotkey());
  applyTimerOption(cd::RestartHotkey, _cfg->getRestartHotkey());
}

void CountdownTimerPlugin::onWidgetClicked()
{
  pauseTimer();
}

void CountdownTimerPlugin::onWidgetDblclicked()
{
  if (_cfg->getRestartOnDblClick())
    restartTimer();
}

void CountdownTimerPlugin::initTimer()
{
  if (_cfg->getUseTargetDateTime()) {
    QDateTime now = QDateTime::currentDateTime().toTimeZone(currentTimeZone());
    now = now.addMSecs(-now.time().msec());
    QDateTime target = _cfg->getTargetDateTime();
    if (target < now) {
      target = countdown_timer::default_target_date();
      // settings_->SetOption(OPT_TARGET_DATETIME, target);   // ?
    }
    target.setTimeZone(currentTimeZone());
    if (target > now) {
      _timer->setInterval(now.secsTo(target));
      _timer->start();
    }
  } else {
    qint64 timeout = _cfg->getIntervalSeconds();
    timeout += 60LL * _cfg->getIntervalMinutes();
    timeout += 3600LL * _cfg->getIntervalHours();
    _timer->setInterval(timeout);
  }
}

void CountdownTimerPlugin::updateWidgetText()
{
  qint64 counter = _cfg->getReverseCounting() ? _timer->interval() - _timer->timeLeft() : _timer->timeLeft();
  const countdown_timer::fmt_options options = {
    .days_threshold = _cfg->getHideDaysThreshold(),
    .hide_hours = _cfg->getAlsoHideHours(),
    .leading_zero = _cfg->getShowLeadingZero(),
  };
  _last_text = countdown_timer::format_time(counter, options);
}

void CountdownTimerPlugin::onTimeout()
{
  if (_cfg->getChimeOnTimeout()) {
    _player->setSource(QUrl::fromLocalFile(_cfg->getChimeSoundFile()));
    _player->play();
  }

  if (_cfg->getShowMessage()) {
    QMessageBox mb(QMessageBox::Warning,
                   tr("Countdown timer"),
                   _cfg->getMessageText());
    mb.addButton(QMessageBox::Ok)->setFocusPolicy(Qt::ClickFocus);
    mb.exec();
  }

  if (_cfg->getRestartOnTimeout()) {
    initTimer();
    _timer->start();
  }
}

void CountdownTimerPlugin::pauseTimer()
{
  if (_timer->isActive())
    _timer->stop();
  else
    _timer->start();
}

void CountdownTimerPlugin::restartTimer()
{
  _timer->stop();
  initTimer();
  _timer->start();
}

QTimeZone CountdownTimerPlugin::currentTimeZone() const
{
  return _local_time ? QTimeZone::systemTimeZone() : _last_tz;
}


QString CountdownTimerPluginFactory::description() const
{
  return tr("Just a countdown timer.");
}

QVector<QWidget*> CountdownTimerPluginFactory::configPagesBeforeCommonPages()
{
  using countdown_timer::TimerSettingsWidget;
  auto page = new TimerSettingsWidget();
  connect(this, &CountdownTimerPluginFactory::instanceSwitched, page, [this, page](size_t idx) {
    page->initControls(qobject_cast<CountdownTimerInstanceConfig*>(instanceConfig(idx)));
    if (!hasInstances()) return;
    disconnect(page, &TimerSettingsWidget::optionChanged, nullptr, nullptr);
    auto inst = qobject_cast<CountdownTimerPlugin*>(instance(idx));
    connect(page, &TimerSettingsWidget::optionChanged, inst, &CountdownTimerPlugin::applyTimerOption);
  });
  return { page };
}

QVector<QWidget*> CountdownTimerPluginFactory::configPagesAfterCommonPages()
{
  using countdown_timer::HotkeysSettingsWidget;
  auto page = new HotkeysSettingsWidget();
  connect(this, &CountdownTimerPluginFactory::instanceSwitched, page, [this, page](size_t idx) {
    page->initControls(qobject_cast<CountdownTimerInstanceConfig*>(instanceConfig(idx)));
    if (!hasInstances()) return;
    disconnect(page, &HotkeysSettingsWidget::optionChanged, nullptr, nullptr);
    auto inst = qobject_cast<CountdownTimerPlugin*>(instance(idx));
    connect(page, &HotkeysSettingsWidget::optionChanged, inst, &CountdownTimerPlugin::applyTimerOption);
  });
  return { page };
}

#include "countdown_timer_plugin.moc"
