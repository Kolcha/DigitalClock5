/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "impl/countdown_timer_plugin_impl.hpp"
#include "impl/countdown_timer_settings.hpp"

namespace Ui {
class HotkeysSettingsWidget;
}

class HotkeysSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  explicit HotkeysSettingsWidget(PluginSettingsStorage& s, StateClient& t,
                                 CountdownTimerPluginImpl* impl, QWidget* parent = nullptr);
  ~HotkeysSettingsWidget();

private slots:
  void on_pause_seq_edit_editingFinished();
  void on_restart_seq_edit_editingFinished();

private:
  Ui::HotkeysSettingsWidget* ui;
  plugin_impl::CountdownTimerPluginSettings cfg;
  CountdownTimerPluginImpl* impl;
};
