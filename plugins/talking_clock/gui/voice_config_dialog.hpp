/*
 * SPDX-FileCopyrightText: 2016-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>

#include <QTextToSpeech>

namespace talking_clock {

namespace Ui {
class VoiceConfigDialog;
}

class VoiceConfigDialog : public QDialog
{
  Q_OBJECT

public:
  explicit VoiceConfigDialog(QWidget* parent = nullptr);
  ~VoiceConfigDialog();

  int     volume()    const;
  int     rate()      const;
  int     pitch()     const;
  QString engine()    const;
  int     language()  const;
  int     voice()     const;

public slots:
  void setVolume(int volume);
  void setRate(int rate);
  void setPitch(int pitch);
  void setEngine(const QString& engine);
  void setLanguage(int lang_idx);
  void setVoice(int voice_idx);

private slots:
  void speak();
  void stop();

  void stateChanged(QTextToSpeech::State state);
  void engineSelected(int index);
  void languageSelected(int language);
  void voiceSelected(int index);

  void localeChanged(const QLocale& locale);

private:
  Ui::VoiceConfigDialog* ui;
  QTextToSpeech* m_speech;
  QVector<QVoice> m_voices;
};

} // namespace talking_clock
