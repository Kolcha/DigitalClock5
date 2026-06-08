/*
 * SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_widget.hpp"
#include "ui_settings_widget.h"

namespace plugin::quick_note {

SettingsWidget::SettingsWidget(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::SettingsWidget)
{
  ui->setupUi(this);
}

SettingsWidget::~SettingsWidget()
{
  delete ui;
}

void SettingsWidget::initControls(QuickNotePluginInstanceConfig* icfg)
{
  Q_ASSERT(icfg);
  cfg = icfg;

  QSignalBlocker _(this);

  ui->noteEdit->setText(icfg->getLastText());
}

void SettingsWidget::on_noteEdit_textEdited(const QString &arg1)
{
  cfg->setLastText(arg1);
  emit textChanged(arg1);
}

} // namespace plugin::quick_note
