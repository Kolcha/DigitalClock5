/*
 * SPDX-FileCopyrightText: 2014-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

namespace Ui {
class PluginListItemWidget;
}

class PluginListItemWidget : public QWidget
{
  Q_OBJECT

public:
  explicit PluginListItemWidget(QWidget* parent = nullptr);
  ~PluginListItemWidget();

public slots:
  void SetDisplayName(const QString& name);
  void SetVersion(const QString& version);
  void SetConfigurable(bool configable);
  void SetChecked(bool checked);

signals:
  void StateChanged(bool new_state);
  void ConfigureRequested();
  void InfoDialogRequested();

private:
  Ui::PluginListItemWidget* ui;
};
