/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>
#include <QLabel>

class ClickableLabel : public QLabel
{
  Q_OBJECT

public:
  explicit ClickableLabel(QWidget* parent = nullptr);

signals:
  void clicked();

protected:
  void mouseReleaseEvent(QMouseEvent* event) override;
};

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AboutDialog(QWidget* parent = nullptr);
  ~AboutDialog();

protected:
  void showEvent(QShowEvent* event) override;

private:
  Ui::AboutDialog* ui;
  ClickableLabel* _u_btn;
};
