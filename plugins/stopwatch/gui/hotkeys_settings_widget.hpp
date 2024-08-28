/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "impl/stopwatch_plugin_impl.hpp"
#include "impl/stopwatch_settings.hpp"

namespace Ui {
class HotkeysSettingsWidget;
}

class HotkeysSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  explicit HotkeysSettingsWidget(SettingsClient& s, StateClient& t,
                                 StopwatchPluginImpl* impl, QWidget* parent = nullptr);
  ~HotkeysSettingsWidget();

private slots:
  void on_pause_seq_edit_editingFinished();
  void on_restart_seq_edit_editingFinished();

private:
  Ui::HotkeysSettingsWidget* ui;
  plugin_impl::StopwatchPluginSettings<SettingsClient> cfg;
  StopwatchPluginImpl* impl;
};
