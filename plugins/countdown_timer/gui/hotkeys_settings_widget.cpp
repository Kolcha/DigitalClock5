/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "hotkeys_settings_widget.hpp"
#include "ui_hotkeys_settings_widget.h"

namespace countdown_timer {

HotkeysSettingsWidget::HotkeysSettingsWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HotkeysSettingsWidget)
{
  ui->setupUi(this);
}

HotkeysSettingsWidget::~HotkeysSettingsWidget()
{
  delete ui;
}

void HotkeysSettingsWidget::initControls(CountdownTimerInstanceConfig* icfg)
{
  Q_ASSERT(icfg);
  cfg = icfg;

  QSignalBlocker _(this);

  ui->pause_seq_edit->setKeySequence(QKeySequence(cfg->getPauseHotkey()));
  ui->restart_seq_edit->setKeySequence(QKeySequence(cfg->getRestartHotkey()));
}

void HotkeysSettingsWidget::on_pause_seq_edit_editingFinished()
{
  cfg->setPauseHotkey(ui->pause_seq_edit->keySequence().toString());
  emit optionChanged(PauseHotkey, cfg->getPauseHotkey());
}

void HotkeysSettingsWidget::on_restart_seq_edit_editingFinished()
{
  cfg->setRestartHotkey(ui->restart_seq_edit->keySequence().toString());
  emit optionChanged(RestartHotkey, cfg->getRestartHotkey());
}

} // namespace countdown_timer
