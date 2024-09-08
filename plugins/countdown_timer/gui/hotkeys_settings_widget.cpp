/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "hotkeys_settings_widget.hpp"
#include "ui_hotkeys_settings_widget.h"

HotkeysSettingsWidget::HotkeysSettingsWidget(PluginSettingsStorage& s, StateClient& t,
                                             CountdownTimerPluginImpl* impl, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HotkeysSettingsWidget)
    , cfg(s)
    , impl(impl)
{
  ui->setupUi(this);

  ui->pause_seq_edit->setKeySequence(QKeySequence(cfg.getPauseHotkey()));
  ui->restart_seq_edit->setKeySequence(QKeySequence(cfg.getRestartHotkey()));
}

HotkeysSettingsWidget::~HotkeysSettingsWidget()
{
  delete ui;
}

void HotkeysSettingsWidget::on_pause_seq_edit_editingFinished()
{
  cfg.setPauseHotkey(ui->pause_seq_edit->keySequence().toString());
  if (impl->pause_hotkey)
    impl->pause_hotkey->setShortcut(ui->pause_seq_edit->keySequence(), true);
}

void HotkeysSettingsWidget::on_restart_seq_edit_editingFinished()
{
  cfg.setRestartHotkey(ui->restart_seq_edit->keySequence().toString());
  if (impl->restart_hotkey)
    impl->restart_hotkey->setShortcut(ui->restart_seq_edit->keySequence(), true);
}
