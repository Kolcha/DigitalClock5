/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_widget.hpp"
#include "ui_settings_widget.h"

#include <QDesktopServices>
#include <QUrl>

namespace plugin::date {

SettingsWidget::SettingsWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SettingsWidget)
{
  ui->setupUi(this);
  ui->str_format_help_btn->setIcon(QIcon(":/icons/help-hint.svg"));

  QSignalBlocker _(ui->sys_format_box);
  ui->sys_format_box->addItem(tr("long system format"), QLocale::LongFormat);
  ui->sys_format_box->addItem(tr("short system format"), QLocale::ShortFormat);
}

SettingsWidget::~SettingsWidget()
{
  delete ui;
}

void SettingsWidget::initControls(DatePluginInstanceConfig* icfg)
{
  Q_ASSERT(icfg);
  cfg = icfg;

  QSignalBlocker _(this);

  ui->sys_format_rbtn->setChecked(cfg->getFormatType() == DatePluginInstanceConfig::System);
  ui->str_format_rbtn->setChecked(cfg->getFormatType() == DatePluginInstanceConfig::Custom);

  ui->sys_format_box->setCurrentIndex(cfg->getFormatSys() == QLocale::LongFormat ? 0 : 1);

  ui->str_format_edit->setCurrentText(cfg->getFormatStr());
}

void SettingsWidget::on_sys_format_rbtn_clicked()
{
  cfg->setFormatType(DatePluginInstanceConfig::System);
  emit dateFormatChanged();
}

void SettingsWidget::on_sys_format_box_activated(int index)
{
  cfg->setFormatSys(ui->sys_format_box->itemData(index).value<QLocale::FormatType>());
  emit dateFormatChanged();
}

void SettingsWidget::on_str_format_rbtn_clicked()
{
  cfg->setFormatType(DatePluginInstanceConfig::Custom);
  emit dateFormatChanged();
}

void SettingsWidget::on_str_format_edit_editTextChanged(const QString& arg1)
{
  cfg->setFormatStr(arg1);
  emit dateFormatChanged();
}

void SettingsWidget::on_str_format_help_btn_clicked()
{
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClock5/wiki/Supported-date-time-formats"));
}

} // namespace plugin::date
