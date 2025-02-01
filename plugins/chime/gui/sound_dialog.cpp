/*
 * SPDX-FileCopyrightText: 2016-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sound_dialog.hpp"
#include "ui_sound_dialog.h"

#include <QAudioOutput>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>

namespace chime {

SoundDialog::SoundDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::SoundDialog)
{
  ui->setupUi(this);

  ui->defaults_box->blockSignals(true);
  ui->defaults_box->addItem(tr("sound 1"), QUrl("qrc:/chime/hour_signal.wav"));
  ui->defaults_box->addItem(tr("sound 2"), QUrl("qrc:/chime/quarter_signal.wav"));
  ui->defaults_box->addItem(tr("sound 3"), QUrl("qrc:/chime/big_ben_strikes_1.mp3"));
  ui->defaults_box->addItem(tr("sound 4"), QUrl("qrc:/chime/quarter_hour_chime_beg.mp3"));
  ui->defaults_box->blockSignals(false);

  auto ao = new QAudioOutput(this);
  _player = new QMediaPlayer(this);
  _player->setAudioOutput(ao);
  connect(_player, &QMediaPlayer::playbackStateChanged, this, &SoundDialog::onPlayerStateChanged);
  connect(_player, &QMediaPlayer::sourceChanged, this, &SoundDialog::onPlayerMediaChanged);
  connect(ui->preview_btn, &QToolButton::clicked, _player, &QMediaPlayer::play);
  connect(ui->volume_slider, &QSlider::valueChanged, ao, [ao](int val) { ao->setVolume(val/100.); });
}

SoundDialog::~SoundDialog()
{
  _player->stop();
  _player->setSource(QUrl());
  delete ui;
}

QUrl SoundDialog::sound() const
{
  return _player->source();
}

int SoundDialog::volume() const
{
  return _player->audioOutput()->volume() * 100;
}

void SoundDialog::setSound(const QUrl& last_sound)
{
  ui->default_snd_rb->setChecked(last_sound.scheme() == "qrc");
  if (ui->default_snd_rb->isChecked()) {
    for (int i = 0; i < ui->defaults_box->count(); ++i) {
      if (ui->defaults_box->itemData(i, Qt::UserRole).toUrl() == last_sound) {
        ui->defaults_box->setCurrentIndex(i);
        break;
      }
    }
  }
  ui->custom_snd_rb->setChecked(last_sound.scheme() == "file");
  _player->setSource(last_sound);
}

void SoundDialog::setVolume(const int last_volume)
{
  ui->volume_slider->setValue(last_volume);
  _player->audioOutput()->setVolume(last_volume);
}

void SoundDialog::setLastFile(const QString& file)
{
  _last_cust_file = file;
}

void SoundDialog::onPlayerStateChanged(QMediaPlayer::PlaybackState new_state)
{
  if (new_state == QMediaPlayer::PlayingState) {
    disconnect(ui->preview_btn, &QToolButton::clicked, _player, &QMediaPlayer::play);
    connect(ui->preview_btn, &QToolButton::clicked, _player, &QMediaPlayer::stop);
    ui->preview_btn->setIcon(QIcon(":/chime/stop.svg.p"));
  } else {
    disconnect(ui->preview_btn, &QToolButton::clicked, _player, &QMediaPlayer::stop);
    connect(ui->preview_btn, &QToolButton::clicked, _player, &QMediaPlayer::play);
    ui->preview_btn->setIcon(QIcon(":/chime/play.svg.p"));
  }
}

void SoundDialog::onPlayerMediaChanged(const QUrl& newmedia)
{
  ui->current_file_value->setText(newmedia.fileName());
}

void SoundDialog::on_default_snd_rb_clicked(bool checked)
{
  if (checked) _player->setSource(ui->defaults_box->currentData(Qt::UserRole).toUrl());
}

void SoundDialog::on_defaults_box_activated(int index)
{
  _player->setSource(ui->defaults_box->itemData(index, Qt::UserRole).toUrl());
}

void SoundDialog::on_custom_snd_rb_clicked(bool checked)
{
  if (checked) _player->setSource(_last_cust_file.isEmpty() ? QUrl() : QUrl::fromLocalFile(_last_cust_file));
}

void SoundDialog::on_custom_browse_btn_clicked()
{
  QString last_path = QDir::homePath();
  if (!_last_cust_file.isEmpty()) {
    QFileInfo last_cust_info(_last_cust_file);
    last_path = last_cust_info.absolutePath();
  }
  // *INDENT-OFF*
  QString new_sound = QFileDialog::getOpenFileName(this, tr("Select sound"),
                                                   last_path,
                                                   tr("Sounds (*.wav *.mp3 *.ogg *.oga *.m4a);;All files (*.*)"));
  // *INDENT-ON*
  if (new_sound.isEmpty()) return;
  _last_cust_file = new_sound;
  _player->setSource(QUrl::fromLocalFile(new_sound));
}

} // namespace chime
