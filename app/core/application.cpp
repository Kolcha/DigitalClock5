/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "application.hpp"

#include <QDir>
#include <QGraphicsColorizeEffect>

#include "gui/about_dialog.hpp"
#include "gui/settings_dialog.hpp"

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv)
    , _pm(this)
{
  initConfig();
  initTray();
  createWindows();
  initUpdater();
  loadPlugins();
  startTimers();
}

void Application::configureWindow(size_t i)
{
  // set all window appearance settings
  auto wnd = window(i);

  auto& wcfg = _cfg->window(i);
  auto& acfg = wcfg.appearance();
  auto& gcfg = wcfg.generic();

  StateImpl wst(wcfg.state());
  wnd->loadState(wst);

  wnd->setAnchorPoint(static_cast<ClockWindow::AnchorPoint>(gcfg.getAnchorPoint()));
  wnd->setSnapToEdge(gcfg.getSnapToEdge(), gcfg.getSnapThreshold());

  wnd->setScaling(acfg.getScalingH()/100., acfg.getScalingV()/100.);
  wnd->setWindowOpacity(acfg.getOpacity()/100.);

  auto clock = wnd->clock();
  clock->setCharSpacing(acfg.getSpacingH());
  clock->setLineSpacing(acfg.getSpacingV());

  clock->setTimeZone(gcfg.getTimeZone());

  // load skin
  if (i == 0 || _cfg->global().getAppearancePerInstance()) {
    auto skin = acfg.getUseFontInsteadOfSkin() ?
                    _sm.loadSkin(acfg.getFont()) :
                    _sm.loadSkin(acfg.getSkin());
    clock->setSkin(std::move(skin));
  } else {
    // share skin
    clock->setSkin(_windows[0]->clock()->skin());
  }

  clock->setUseAlternateSeparator(clock->skin()->hasAlternateSeparator());

  clock->setFlashSeparator(acfg.getSeparatorFlashes());
  clock->setUseCustomSeparators(acfg.getUseCustomSeparators());
  clock->setCustomSeparators(acfg.getCustomSeparators());

  clock->setFormat(acfg.getTimeFormat());
  clock->setLayoutConfig(acfg.getLayoutConfig());
  clock->setSecondsScaleFactor(acfg.getSecondsScaleFactor()/100.);

  clock->setTexture(acfg.getTexture(),
                    acfg.getTextureStretch(),
                    acfg.getTexturePerCharacter());
  clock->setBackground(acfg.getBackground(),
                       acfg.getBackgroundStretch(),
                       acfg.getBackgroundPerCharacter());

  if (acfg.getApplyColorization()) {
    auto effect = new QGraphicsColorizeEffect;
    effect->setColor(acfg.getColorizationColor());
    effect->setStrength(acfg.getColorizationStrength());
    wnd->setGraphicsEffect(effect);
  } else {
    wnd->setGraphicsEffect(nullptr);
  }

  wnd->setTransparentOnHover(_cfg->global().getTransparentOnHover());
  wnd->setOpacityOnHover(_cfg->global().getOpacityOnHover() / 100.);
}

void Application::configureWindows()
{
  for (size_t i = 0; i < _windows.size(); i++) configureWindow(i);
}

size_t Application::findWindow(ClockWindow* w) const
{
  for (size_t i = 0; i < _windows.size(); i++)
    if (_windows[i].get() == w)
      return i;
  return 0;   // always return valid index
}

void Application::initConfig()
{
  using namespace Qt::Literals::StringLiterals;
  QDir app_dir(QApplication::applicationDirPath());
  if (app_dir.exists(u"portable.txt"_s) || app_dir.exists(u".portable"_s))
    _cfg = std::make_unique<AppConfig>(app_dir.absoluteFilePath(u"settings.ini"_s));
  else
    _cfg = std::make_unique<AppConfig>();
  _sm.setSkinBaseSize(_cfg->limits().getBaseSize());
}

void Application::initTray()
{
  _tray_icon.setVisible(true);

  _tray_menu = std::make_unique<QMenu>();
  _tray_icon.setContextMenu(_tray_menu.get());

  using namespace Qt::Literals::StringLiterals;
  _tray_menu->addAction(QIcon::fromTheme(u"configure"_s), tr("Settings"),
                        this, &Application::showSettingsDialog);
  _tray_menu->addSeparator();
  _tray_menu->addAction(QIcon::fromTheme(u"help-about"_s), tr("About"),
                        this, &Application::showAboutDialog);
  _tray_menu->addSeparator();
  _tray_menu->addAction(QIcon::fromTheme(u"application-exit"_s),
                        tr("Quit"), this, &QApplication::quit);

  _tray_icon.setToolTip(applicationDisplayName());
}

void Application::createWindows()
{
  for (int i = 0; i < _cfg->global().getNumInstances(); i++) {
    auto wnd = std::make_unique<ClockWindow>();

    wnd->setOriginPoint(wnd->originPoint() + QPoint(i*15, i*15));

    wnd->setWindowFlag(Qt::NoDropShadowWindowHint);
    wnd->setWindowFlag(Qt::FramelessWindowHint);
    wnd->setAttribute(Qt::WA_TranslucentBackground);

    if (_cfg->global().getStayOnTop()) {
      wnd->setWindowFlag(Qt::WindowStaysOnTopHint);
      wnd->setWindowFlag(Qt::BypassWindowManagerHint);
    }
    if (_cfg->global().getTransparentForMouse())
      wnd->setWindowFlag(Qt::WindowTransparentForInput);
#ifdef Q_OS_WINDOWS
    wnd->setWindowFlag(Qt::Tool);   // trick to hide app icon from taskbar (Windows only)
#endif

    wnd->setWindowTitle(QString("%1 %2").arg(applicationDisplayName()).arg(i));

    connect(wnd.get(), &ClockWindow::saveStateRequested, this, &Application::saveWindowState);

    connect(wnd.get(), &ClockWindow::settingsDialogRequested, this, &Application::showSettingsDialog);
    connect(wnd.get(), &ClockWindow::aboutDialogRequested, this, &Application::showAboutDialog);
    connect(wnd.get(), &ClockWindow::appExitRequested, this, &QApplication::quit);

    connect(&_tick_timer, &QTimer::timeout, wnd->clock(), &GraphicsDateTimeWidget::updateSeparatorsState);
#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
    connect(&_mouse_tracker, &MouseTracker::mousePositionChanged, wnd.get(), &ClockWindow::handleMouseMove);
#endif
#if defined(Q_OS_WINDOWS)
    if (_cfg->global().getStayOnTop()) {
      connect(&_time_timer, &QTimer::timeout, wnd.get(), &ClockWindow::runStayOnTopHacks);
    }
#endif

    _windows.push_back(std::move(wnd));

    configureWindow(_windows.size() - 1);

    _settings_rcv.push_back(std::make_unique<SettingsChangeListener>(this, i));
    _settings_tr.push_back(std::make_unique<SettingsChangeTransmitter>());
  }

  std::ranges::for_each(_windows, [](auto& w) { w->show(); });
}

void Application::initUpdater()
{
  if (!_cfg->global().getCheckForUpdates())
    return;

  _update_checker = std::make_unique<UpdateChecker>();
  _update_checker->setCheckForBeta(_cfg->global().getCheckForBetaVersion());

  auto save_last_update = [this]() {
    StateImpl app_state(_cfg->state());
    app_state.setValue("LastUpdateCheck", QDateTime::currentDateTime());
  };

  connect(_update_checker.get(), &UpdateChecker::newVersion, this, save_last_update);
  connect(_update_checker.get(), &UpdateChecker::upToDate, this, save_last_update);

  const StateImpl app_state(_cfg->state());
  const auto& last_update = app_state.value("LastUpdateCheck",
                                            QDateTime(QDate(2013, 6, 18), QTime(11, 20, 5), QTimeZone::utc()))
                                .toDateTime();
  const auto update_period = _cfg->global().getUpdatePeriodDays();
  if (last_update.daysTo(QDateTime::currentDateTime()) >= update_period)
    _update_checker->checkForUpdates();
}

void Application::loadPlugins()
{
  _pm.init();
  const auto plugins = _cfg->global().getPlugins();
  for (const auto& p : plugins) _pm.loadPlugin(p);
}

void Application::startTimers()
{
  connect(&_time_timer, &QTimer::timeout, this, &Application::onTimer);
  _time_timer.start(500);
  _tick_timer.start(500);
}

void Application::saveWindowState()
{
  auto wnd = qobject_cast<ClockWindow*>(sender());
  Q_ASSERT(wnd);
  auto idx = findWindow(wnd);

  StateImpl wst(_cfg->window(idx).state());
  wnd->saveState(wst);
}

void Application::onTimer()
{
  auto now = QDateTime::currentDateTime();
  for (const auto& w : _windows)
    w->clock()->setDateTime(now);
}

void Application::showSettingsDialog()
{
  auto wnd = qobject_cast<ClockWindow*>(sender());
  int idx = wnd ? findWindow(wnd) : 0;
  _dm.maybeCreateAndShowDialog<SettingsDialog>(0xbd480f59, this, idx);
}

void Application::showAboutDialog()
{
  _dm.maybeCreateAndShowDialog<AboutDialog>(0x41697269);
}
