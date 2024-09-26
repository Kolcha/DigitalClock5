/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_widget.hpp"
#include "ui_settings_widget.h"

#include <QDesktopServices>
#include <QUrl>

SettingsWidget::SettingsWidget(PluginSettingsStorage& s, StateClient& t,
                               DatePluginImpl* impl, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SettingsWidget)
    , cfg(s)
    , impl(impl)
{
  ui->setupUi(this);

  ui->sys_format_rbtn->setChecked(cfg.getFormatType() == DatePluginImpl::System);
  ui->str_format_rbtn->setChecked(cfg.getFormatType() == DatePluginImpl::Custom);

  ui->sys_format_box->addItem(tr("long system format"), QLocale::LongFormat);
  ui->sys_format_box->addItem(tr("short system format"), QLocale::ShortFormat);

  ui->sys_format_box->setCurrentIndex(cfg.getFormatSys() == QLocale::LongFormat ? 0 : 1);

  ui->str_format_edit->setCurrentText(cfg.getFormatStr());
}

SettingsWidget::~SettingsWidget()
{
  delete ui;
}

void SettingsWidget::on_sys_format_rbtn_clicked()
{
  cfg.setFormatType(DatePluginImpl::System);
  impl->format_type = DatePluginImpl::System;
  impl->updateWidget();
}

void SettingsWidget::on_sys_format_box_activated(int index)
{
  if (index < 0) return;
  QLocale::FormatType fmt = ui->sys_format_box->itemData(index).value<QLocale::FormatType>();
  cfg.setFormatSys(fmt);
  impl->sys_format = fmt;
  impl->updateWidget();
}

void SettingsWidget::on_str_format_rbtn_clicked()
{
  cfg.setFormatType(DatePluginImpl::Custom);
  impl->format_type = DatePluginImpl::Custom;
  impl->updateWidget();
}

void SettingsWidget::on_str_format_edit_editTextChanged(const QString& arg1)
{
  cfg.setFormatStr(arg1);
  impl->str_format = arg1;
  impl->updateWidget();
}

void SettingsWidget::on_str_format_help_btn_clicked()
{
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClock5/wiki/Supported-date-time-formats"));
}
