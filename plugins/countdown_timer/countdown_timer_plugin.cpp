/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "countdown_timer_plugin.hpp"

#include <QMessageBox>
#include <QMouseEvent>

#include <QPushButton>

#include <QMediaPlayer>

#include "gui/hotkeys_settings_widget.hpp"
#include "gui/timer_settings_widget.hpp"

class CountdownTimerWidget : public GraphicsTextWidget
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
    : GraphicsTextWidget(parent)
{
}

void CountdownTimerWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    emit clicked();
  }
  GraphicsTextWidget::mouseReleaseEvent(event);
}

void CountdownTimerWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    emit doubleClicked();
  }
  GraphicsTextWidget::mouseDoubleClickEvent(event);
}


CountdownTimerPlugin::CountdownTimerPlugin()
    : _impl(std::make_unique<CountdownTimerPluginImpl>())
{
}

CountdownTimerPlugin::~CountdownTimerPlugin() = default;

void CountdownTimerPlugin::saveState(StateClient& st) const
{
}

void CountdownTimerPlugin::loadState(const StateClient& st)
{
}

void CountdownTimerPlugin::initSettings(const SettingsClient& st)
{
  _impl->initSettings(st);
  _impl->updateWidgetText();
  WidgetPluginBase::initSettings(st);
}

void CountdownTimerPlugin::accepted()
{
  if (_impl->timer)
    _impl->initTimer();
  WidgetPluginBase::accepted();
}

void CountdownTimerPlugin::rejected()
{
  WidgetPluginBase::rejected();
}

void CountdownTimerPlugin::init()
{
  using countdown_timer::CountdownTimer;
  _impl->timer = std::make_unique<CountdownTimer>();

  connect(_impl->timer.get(), &CountdownTimer::timeLeftChanged, this, &CountdownTimerPlugin::onUpdate);
  connect(_impl->timer.get(), &CountdownTimer::timeout, this, &CountdownTimerPlugin::onTimeout);

  _player = std::make_unique<QMediaPlayer>();

  connect(_impl->pause_hotkey.get(), &QHotkey::activated, this, &CountdownTimerPlugin::onWidgetClicked);
  connect(_impl->restart_hotkey.get(), &QHotkey::activated, this, &CountdownTimerPlugin::onWidgetDblclicked);

  WidgetPluginBase::init();

  _impl->applySettings();
  _impl->initTimer();
}

void CountdownTimerPlugin::shutdown()
{
  if (_impl->timer->isActive())
    _impl->timer->stop();
  // shortcuts must be destroyed explicitly...
  // if not, it causes the crash for some reason
  _impl->pause_hotkey.reset();
  _impl->restart_hotkey.reset();

  WidgetPluginBase::shutdown();
}

QList<QWidget*> CountdownTimerPlugin::customConfigure(SettingsClient& s, StateClient& t)
{
  auto timer_tab = new TimerSettingsWidget(s, t, _impl.get());
  auto hotkeys_tab = new HotkeysSettingsWidget(s, t, _impl.get());
  return {timer_tab, hotkeys_tab};
}

std::shared_ptr<GraphicsWidgetBase> CountdownTimerPlugin::createWidget()
{
  auto w = std::make_shared<CountdownTimerWidget>();

  connect(w.get(), &CountdownTimerWidget::clicked, this, &CountdownTimerPlugin::onWidgetClicked);
  connect(w.get(), &CountdownTimerWidget::doubleClicked, this, &CountdownTimerPlugin::onWidgetDblclicked);

  _impl->widget = std::move(w);
  _impl->updateWidgetText();
  return _impl->widget;
}

void CountdownTimerPlugin::destroyWidget()
{
  _impl->widget.reset();
}

void CountdownTimerPlugin::onWidgetClicked()
{
  if (_impl->timer->isActive()) {
    _impl->timer->stop();
  } else {
    _impl->timer->start();
  }
}

void CountdownTimerPlugin::onWidgetDblclicked()
{
  if (_impl->restart_on_dbl_click) {
    _impl->timer->stop();
    _impl->initTimer();
    _impl->timer->start();
  }
}

void CountdownTimerPlugin::onUpdate()
{
  _impl->updateWidgetText();
}

void CountdownTimerPlugin::onTimeout()
{
  if (_impl->chime_on_timeout) {
    _player->setSource(QUrl::fromLocalFile(_impl->chime_sound));
    _player->play();
  }

  if (_impl->show_message) {
    QMessageBox mb(QMessageBox::Warning,
                   tr("Countdown timer"),
                   _impl->message_text);
    mb.addButton(QMessageBox::Ok)->setFocusPolicy(Qt::ClickFocus);
    mb.exec();
  }

  if (_impl->restart_on_timeout) {
    _impl->initTimer();
    _impl->timer->start();
  }
}


std::unique_ptr<ClockPluginBase> CountdownTimerPluginFactory::create() const
{
  return std::make_unique<CountdownTimerPlugin>();
}

QString CountdownTimerPluginFactory::description() const
{
  return tr("Just a countdown timer.");
}

#include "countdown_timer_plugin.moc"
