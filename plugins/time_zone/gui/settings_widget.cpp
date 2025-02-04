/*
 * SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_widget.hpp"
#include "ui_settings_widget.h"

#include <QDesktopServices>
#include <QUrl>

namespace plugin::time_zone {

SettingsWidget::SettingsWidget(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::SettingsWidget)
{
  ui->setupUi(this);
  ui->str_format_help_btn->setIcon(QIcon(":/icons/help-hint.svg"));
}

SettingsWidget::~SettingsWidget()
{
  delete ui;
}

void SettingsWidget::initControls(TimeZonePluginInstanceConfig* icfg)
{
  Q_ASSERT(icfg);
  cfg = icfg;

  QSignalBlocker _(this);

  ui->str_format_edit->setCurrentText(cfg->getFormatStr());
}

void SettingsWidget::on_str_format_edit_editTextChanged(const QString& arg1)
{
  cfg->setFormatStr(arg1);
  emit tzFormatChanged();
}

void SettingsWidget::on_str_format_help_btn_clicked()
{
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClock5/wiki/Supported-date-time-formats"));
}

} // namespace plugin::time_zone
