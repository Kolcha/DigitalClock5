/*
 * SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "impl/quick_note_settings.hpp"

namespace plugin::quick_note {

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
  Q_OBJECT

public:
  SettingsWidget(QWidget* parent = nullptr);
  ~SettingsWidget();

  void initControls(QuickNotePluginInstanceConfig* icfg);

signals:
  void textChanged(const QString&);

private slots:
  void on_noteEdit_textEdited(const QString& arg1);

private:
  Ui::SettingsWidget* ui;
  QuickNotePluginInstanceConfig* cfg = nullptr;
};

} // namespace plugin::quick_note
