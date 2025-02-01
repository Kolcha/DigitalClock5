// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtCore/QObject>

#include <QtCore/QVersionNumber>

#include <map>
#include <memory>

#include "common/instance_options.hpp"
#include "common/skin_manager.hpp"
#include "common/tray_icon_manager.hpp"
#include "config/global_options.hpp"
#include "config/settings.hpp"
#include "config/settings_backend_impl.hpp"

#include "dialog_manager.hpp"

class QMenu;
class QTimer;
class QTranslator;

class AppConfig;
class AppState;
class ClockTrayIcon;
class ClockWindow;
class MouseTracker;
class PluginManager;
class UpdateChecker;

class ClockApplication : public QObject
{
  Q_OBJECT

public:
  explicit ClockApplication(QObject* parent = nullptr);
  ~ClockApplication();

  ClockWindow* window(size_t idx) const noexcept;

  AppConfig* config() const noexcept { return _cfg.get(); }
  SettingsBackendImpl* settings() const noexcept { return _settings.get(); }

  QString activeLang() const { return _active_lang; }

  TrayIconManager* getTrayIconManager() const noexcept { return _tm.get(); }
  SkinManager* getSkinManager() const noexcept { return _sm.get(); }
  PluginManager* getPluginManager() const noexcept { return _pm.get(); }

public slots:
  void reconfigureWindow(size_t i);
  void reloadConfig();

protected:
  bool eventFilter(QObject* obj, QEvent* e) override;

private slots:
  void tick();

  void saveWindowState();

  void showSettingsDialog();
  void showAboutDialog();

  void moveWindowToPredefinedPos();

  void maybeCheckForUpdate();
  void handleNewVersion(const QVersionNumber& v, const QDate& d, const QUrl& l);

  void onGlobalOptionChanged(opt::GlobalOptions opt, const QVariant& val);
  void onInstanceOptionChanged(size_t i, opt::InstanceOptions opt, const QVariant& val);

private:
  void initConfig();
  void loadTranslation();
  void initTray();
  void createWindows();
  void initUpdater();
  void startTimers();

  void retranslateUI();
  void updateInstances();

  std::unique_ptr<ClockWindow> createWindow(size_t i);

  size_t findWindow(ClockWindow* wnd) const;

  void addPositionMenu();

  void applyInstanceOption(size_t idx, opt::InstanceOptions opt, const QVariant& val);
  void applyWindowOption(size_t idx, opt::GlobalOptions opt, const QVariant& val);

private:
  std::unique_ptr<ClockTrayIcon> _tray_icon;
  std::unique_ptr<QMenu> _tray_menu;

  std::unique_ptr<SettingsBackendImpl> _settings;
  std::unique_ptr<AppConfig> _cfg;
  std::unique_ptr<AppState> _state;

  std::vector<std::unique_ptr<QTranslator>> _translators;
  QString _last_lang;             // value as in config
  QString _active_lang = "en_US"; // value from QTranslator

  std::unique_ptr<TrayIconManager> _tm;
  std::unique_ptr<SkinManager> _sm;
  DialogManager<quint64> _dm;

  std::map<size_t, std::unique_ptr<ClockWindow>> _windows;

  std::unique_ptr<QTimer> _tick_timer;

  std::unique_ptr<UpdateChecker> _update_checker;
  // plugin manager should be destroyed first
  std::unique_ptr<PluginManager> _pm;
  // platform-specific code
#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
  std::unique_ptr<MouseTracker> _mouse_tracker;
#endif
};
