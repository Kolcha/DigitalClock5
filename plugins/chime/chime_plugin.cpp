/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "chime_plugin.hpp"

#include <QAudioOutput>
#include <QMediaPlayer>

#include "gui/settings_widget.hpp"
#include "impl/chime_plugin_impl.hpp"

ChimePlugin::ChimePlugin()
    : _impl(std::make_unique<ChimePluginImpl>())
{
}

ChimePlugin::~ChimePlugin() = default;

void ChimePlugin::initSettings(const SettingsClient& st)
{
  _impl->initSettings(st);
}

QWidget* ChimePlugin::configure(SettingsClient& s, StateClient& t)
{
  return new chime::SettingsWidget(s, t, _impl.get());
}

void ChimePlugin::init()
{
  _player = std::make_unique<QMediaPlayer>();
  auto ao = new QAudioOutput(_player.get());
  _player->setAudioOutput(ao);
}

void ChimePlugin::shutdown()
{
  _player->stop();
  _player->setSource(QUrl());
}

void ChimePlugin::tick()
{
  if (!_player) return;

  auto cur_time = QTime::currentTime();

  if (_impl->isQuietTime(cur_time)) return;

  if (cur_time.minute() == 0) {                                 // hour
    if (_impl->playback_allowed && _impl->every_hour_enabled) {
      _player->setSource(QUrl());
      int count = 1;
      if (_impl->every_hour_repeat == chime::Dynamic) {
        count = cur_time.hour();
        if (count == 0) count = 12;
        if (count > 12) count -= 12;
      }
      Q_ASSERT(count > 0);
      _player->setLoops(count);
      _player->setSource(_impl->every_hour_signal);
      _player->audioOutput()->setVolume(_impl->every_hour_volume / 100.);
      _player->play();
      _impl->playback_allowed = false;
    }
  }

  if (cur_time.minute() % 15 == 0 && cur_time.minute() != 0) {  // quarter
    if (_impl->playback_allowed && _impl->quarter_hour_enabled) {
      _player->setSource(QUrl());
      int count = 1;
      if (_impl->quarter_hour_repeat == chime::Dynamic) {
        count = cur_time.minute() / 15;
      }
      Q_ASSERT(count > 0);
      _player->setLoops(count);
      _player->setSource(_impl->quarter_hour_signal);
      _player->audioOutput()->setVolume(_impl->quarter_hour_volume / 100.);
      _player->play();
      _impl->playback_allowed = false;
    }
  }

  _impl->playback_allowed = (cur_time.minute() % 15 != 0);
}


std::unique_ptr<ClockPluginBase> ChimePluginFactory::create() const
{
  return std::make_unique<ChimePlugin>();
}

QString ChimePluginFactory::description() const
{
  return tr("Chimes with selected period.");
}
