/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "chime_plugin.hpp"

#include <QAudioOutput>
#include <QMediaPlayer>

#include "gui/settings_dialog.hpp"

namespace {

bool is_quiet_time(const QTime& c_time, const QTime& s_time, const QTime& e_time)
{
  if (s_time <= e_time)
    return (s_time <= c_time) && (c_time <= e_time);

  return ((s_time <= c_time) && (c_time <= QTime(23, 59, 59, 999))) ||
         ((QTime(0, 0) <= c_time) && (c_time <= e_time));
}

} // namespace

ChimePlugin::ChimePlugin(const ChimePluginConfig& cfg)
  : ClockPluginInstance()
  , _cfg(cfg)
{
}

ChimePlugin::~ChimePlugin() = default;

void ChimePlugin::startup()
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

void ChimePlugin::update(const QDateTime& dt)
{
  if (!_player) return;

  auto cur_time = dt.toLocalTime().time();

  if (isQuietTime(cur_time)) return;

  if (cur_time.minute() == 0) {                                 // hour
    if (_playback_allowed && _cfg.getEveryHourEnabled()) {
      _player->setSource(QUrl());
      int count = 1;
      if (_cfg.getEveryHourRepeat() == chime::Dynamic) {
        count = cur_time.hour();
        if (count == 0) count = 12;
        if (count > 12) count -= 12;
      }
      Q_ASSERT(count > 0);
      _player->setLoops(count);
      _player->setSource(_cfg.getEveryHourSignal());
      _player->audioOutput()->setVolume(_cfg.getEveryHourVolume() / 100.);
      _player->play();
      _playback_allowed = false;
    }
  }

  if (cur_time.minute() % 15 == 0 && cur_time.minute() != 0) {  // quarter
    if (_playback_allowed && _cfg.getQuarterHourEnabled()) {
      _player->setSource(QUrl());
      int count = 1;
      if (_cfg.getQuarterHourRepeat() == chime::Dynamic) {
        count = cur_time.minute() / 15;
      }
      Q_ASSERT(count > 0);
      _player->setLoops(count);
      _player->setSource(_cfg.getQuarterHourSignal());
      _player->audioOutput()->setVolume(_cfg.getQuarterHourVolume() / 100.);
      _player->play();
      _playback_allowed = false;
    }
  }

  _playback_allowed = (cur_time.minute() % 15 != 0);
}

bool ChimePlugin::isQuietTime(const QTime& t) const
{
  if (_cfg.getQuietHoursEnabled())
    return is_quiet_time(t, _cfg.getQuietHoursStartTime(), _cfg.getQuietHoursEndTime());
  return false;
}


void ChimePluginFactory::init(Context&& ctx)
{
  Q_ASSERT(!ctx.active_instances.empty());
  _first_idx = ctx.active_instances.front();
  _cfg = std::make_unique<ChimePluginConfig>(std::move(ctx.settings));
  _state = std::move(ctx.state);
}

QString ChimePluginFactory::description() const
{
  return tr("Chimes with selected period.");
}

ClockPluginInstance* ChimePluginFactory::instance(size_t idx)
{
  if (idx != _first_idx)
    return nullptr;

  if (!_inst)
    _inst = std::make_unique<ChimePlugin>(std::ref(*_cfg));

  return _inst.get();
}

void ChimePluginFactory::configure(QWidget* parent, size_t idx)
{
  Q_UNUSED(idx);
  using chime::SettingsDialog;
  auto d = new SettingsDialog(*_cfg, *_state, parent);

  connect(d, &SettingsDialog::accepted, _cfg.get(), &ChimePluginConfig::commit);
  connect(d, &SettingsDialog::rejected, _cfg.get(), &ChimePluginConfig::discard);

  connect(d, &SettingsDialog::finished, d, &QObject::deleteLater);

  d->show();
}
