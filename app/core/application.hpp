/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QApplication>

#include <memory>
#include <vector>

#include <QMenu>
#include <QTimer>

#include "config/app_config.hpp"

#include "gui/clock_tray_icon.hpp"
#include "gui/clock_window.hpp"
#include "dialog_manager.hpp"
#include "plugin_manager.hpp"
#include "settings_change_listener.hpp"
#include "skin_manager.hpp"
#include "update_checker.hpp"

class Application : public QApplication
{
  Q_OBJECT

public:
  Application(int& argc, char** argv);

  AppConfig& config() { return *_cfg; }

  ClockWindow* window(size_t i) { return _windows[i].get(); }
  size_t findWindow(ClockWindow* w) const;

  auto& windows() const { return _windows; }

  void configureWindow(size_t i);
  void configureWindows();

  SkinManager& skinManager() { return _sm; }
  PluginManager& pluginManager() { return _pm; }

  auto& settingsListeners() const { return _settings_rcv; }
  SettingsChangeListener* settingsListener(size_t i)
  {
    return _settings_rcv[i].get();
  }

  auto& settingsTransmitters() const { return _settings_tr; }
  SettingsChangeTransmitter* settingsTransmitter(size_t i)
  {
    return _settings_tr[i].get();
  }

private:
  void initConfig();
  void initTray();
  void createWindows();
  void initUpdater();
  void loadPlugins();
  void startTimers();

private slots:
  void saveWindowState();
  void onTimer();

  void showSettingsDialog();
  void showAboutDialog();

private:
  std::unique_ptr<AppConfig> _cfg;
  ClockTrayIcon _tray_icon;
  std::unique_ptr<QMenu> _tray_menu;
  // QWidget seems to be non-movable type :(
  std::vector<std::unique_ptr<ClockWindow>> _windows;
  QTimer _time_timer;
  QTimer _tick_timer;
  SkinManager _sm;
  PluginManager _pm;
  DialogManager<quint64> _dm;
  std::unique_ptr<UpdateChecker> _update_checker;
  // settings bridge
  std::vector<std::unique_ptr<SettingsChangeTransmitter>> _settings_tr;
  std::vector<std::unique_ptr<SettingsChangeListener>> _settings_rcv;
};