/*
 * SPDX-FileCopyrightText: 2016-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>

#include <QUrl>
#include <QMediaPlayer>

namespace chime {

namespace Ui {
class SoundDialog;
}

class SoundDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SoundDialog(QWidget* parent = nullptr);
  ~SoundDialog();

  QUrl sound() const;
  int volume() const;

public slots:
  void setSound(const QUrl& last_sound);
  void setVolume(const int last_volume);

  void setLastFile(const QString& file);

private slots:
  void onPlayerStateChanged(QMediaPlayer::PlaybackState new_state);
  void onPlayerMediaChanged(const QUrl& newmedia);

  void on_default_snd_rb_clicked(bool checked);
  void on_defaults_box_activated(int index);
  void on_custom_snd_rb_clicked(bool checked);
  void on_custom_browse_btn_clicked();

private:
  Ui::SoundDialog* ui;

  QString _last_cust_file;

  QMediaPlayer* _player;
};

} // namespace chime
