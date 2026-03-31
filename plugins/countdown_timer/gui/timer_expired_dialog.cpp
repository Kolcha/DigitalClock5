/*
 * SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "timer_expired_dialog.hpp"
#include "ui_timer_expired_dialog.h"

namespace countdown_timer {

TimerExpiredDialog::TimerExpiredDialog(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::TimerExpiredDialog)
{
  ui->setupUi(this);

  ui->msg_label->setVisible(false);
}

TimerExpiredDialog::~TimerExpiredDialog()
{
  delete ui;
}

bool TimerExpiredDialog::useCustomTimeout() const
{
  return ui->cust_timeout->isChecked();
}

qint64 TimerExpiredDialog::customTimeout() const
{
  qint64 h = ui->h_edit->value();
  qint64 m = ui->m_edit->value();
  qint64 s = ui->s_edit->value();

  return h * 3600 + m * 60 + s;
}

QString TimerExpiredDialog::message() const
{
  return ui->msg_label->text();
}

void TimerExpiredDialog::setUseCustomTimeout(bool use)
{
  ui->cust_timeout->setChecked(use);
}

void TimerExpiredDialog::setCustomTimeout(qint64 t)
{
  int h = t / 3600;
  t -= h * 3600;
  int m = t / 60;
  t -= m * 60;
  int s = t;

  ui->h_edit->setValue(h);
  ui->m_edit->setValue(m);
  ui->s_edit->setValue(s);
}

void TimerExpiredDialog::setMessage(const QString &msg)
{
  ui->msg_label->setText(msg);
  ui->msg_label->setHidden(msg.isEmpty());
}

} // namespace countdown_timer
