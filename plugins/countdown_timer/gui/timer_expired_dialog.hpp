/*
 * SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>

namespace countdown_timer {

namespace Ui {
class TimerExpiredDialog;
}

class TimerExpiredDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TimerExpiredDialog(QWidget *parent = nullptr);
  ~TimerExpiredDialog();

  bool useCustomTimeout() const;
  qint64 customTimeout() const;

  QString message() const;

public slots:
  void setUseCustomTimeout(bool use);
  void setCustomTimeout(qint64 t);

  void setMessage(const QString& msg);

private:
  Ui::TimerExpiredDialog *ui;
};

} // namespace countdown_timer
