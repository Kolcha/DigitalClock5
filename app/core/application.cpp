/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "application.hpp"

#include <QDir>
#include <QGraphicsColorizeEffect>
#include <QDesktopServices>

#include <QLibraryInfo>

#include "gui/about_dialog.hpp"
#include "gui/settings_dialog.hpp"

#include "translation.hpp"

namespace {

// replace link color with regular text color, as it is blue in most cases
// and this looks terrible, especially with dark backgrounds/themes
// unfortunately, palette must be set at application level to affect links
bool updatePaletteLinkColors()
{
  if (auto pal = QApplication::palette(); pal.link() != pal.windowText()) {
    pal.setBrush(QPalette::Link, pal.windowText());
    pal.setBrush(QPalette::LinkVisited, pal.link());
    QApplication::setPalette(pal);
    return true;
  }
  return false;
}

} // namespace

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv)
    , _pm(this)
{
  updatePaletteLinkColors();
  initConfig();
  loadTranslation();
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
  wnd->setSnapToEdge(_cfg->global().getSnapToEdge(), _cfg->global().getSnapThreshold());

  wnd->setScaling(acfg.getScalingH()/100., acfg.getScalingV()/100.);
  wnd->setWindowOpacity(acfg.getOpacity()/100.);

  wnd->layout()->setSpacing(acfg.getLayoutSpacing());

  auto clock = wnd->clock();
  clock->setCharSpacing(acfg.getSpacingH());
  clock->setLineSpacing(acfg.getSpacingV());
  clock->setMargins(acfg.getTextMargins());

  clock->setIgnoreAX(acfg.getIgnoreAX());
  clock->setIgnoreAY(acfg.getIgnoreAY());

  clock->setTimeZone(gcfg.getTimeZone());

  // load skin
  if (i == 0 || _cfg->global().getAppearancePerInstance()) {
    auto skin = acfg.getUseFontInsteadOfSkin() ?
                    _sm.loadSkin(acfg.getFont()) :
                    _sm.loadSkin(acfg.getSkin());
    skin->setMargins(acfg.getCharMargins());
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
  clock->setUseSystemForeground(acfg.shouldUseSystemForeground());
  clock->setBackground(acfg.getBackground(),
                       acfg.getBackgroundStretch(),
                       acfg.getBackgroundPerCharacter());
  clock->setUseSystemBackground(acfg.shouldUseSystemBackground());

  if (acfg.getApplyColorization()) {
    auto effect = new QGraphicsColorizeEffect;
    effect->setColor(acfg.getColorizationColor());
    effect->setStrength(acfg.getColorizationStrength());
    wnd->setGraphicsEffect(effect);
  } else {
    wnd->setGraphicsEffect(nullptr);
  }

  wnd->setFullscreenDetect(_cfg->global().getFullscreenDetect());
  setPlatformWindowFlags(wnd);

  wnd->setTransparentOnHover(_cfg->global().getTransparentOnHover());
  wnd->setOpacityOnHover(_cfg->global().getOpacityOnHover() / 100.);
}

void Application::configureWindows()
{
  for (size_t i = 0; i < _windows.size(); i++) configureWindow(i);
}

void Application::retranslateUI()
{
  std::ranges::for_each(_translators, [](auto& t) { QApplication::removeTranslator(t.get()); });
  _translators.clear();
  _active_lang = "en_US";

  const auto& plugins = _cfg->global().getPlugins();
  std::ranges::for_each(plugins, [this](auto& p) { _pm.unloadPlugin(p); });
  loadTranslation();
  _pm.enumeratePlugins();
  std::ranges::for_each(plugins, [this](auto& p) { _pm.loadPlugin(p); });
}

bool Application::event(QEvent* e)
{
  if (e->type() == QEvent::ApplicationPaletteChange) {
    if (updatePaletteLinkColors())
      return true;
  }
  return QApplication::event(e);
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

void Application::loadTranslation()
{
  auto locale = _cfg->global().getLocale();
  const bool detect = locale == "auto";

  auto app_translator = detect ? findTranslation(u"digital_clock") : ::loadTranslation(u"digital_clock", locale);
  if (app_translator) {
    QApplication::installTranslator(app_translator.get());

    auto core_translator = detect ? findTranslation(u"plugin_core") : ::loadTranslation(u"plugin_core", locale);
    if (core_translator) {
      QApplication::installTranslator(core_translator.get());
      _translators.push_back(std::move(core_translator));
    }

    auto qt_translator = std::make_unique<QTranslator>();
    if (qt_translator->load(QLatin1String("qt_") + app_translator->language(),
                            QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
      QApplication::installTranslator(qt_translator.get());
      _translators.push_back(std::move(qt_translator));
    }

    _active_lang = app_translator->language();
    _translators.push_back(std::move(app_translator));
  }
}

void Application::initTray()
{
  _tray_icon.setVisible(true);

  _tray_menu = std::make_unique<QMenu>();
  _tray_icon.setContextMenu(_tray_menu.get());

  using namespace Qt::Literals::StringLiterals;
  _tray_menu->addAction(QIcon(":/icons/configure.svg"), tr("Settings"),
                        this, &Application::showSettingsDialog);
  if (_cfg->global().getNumInstances() == 1)
    addPositionMenu();
  _tray_menu->addSeparator();
  _tray_menu->addAction(QIcon(":/icons/help-about.svg"), tr("About"),
                        this, &Application::showAboutDialog);
  _tray_menu->addSeparator();
  _tray_menu->addAction(QIcon(":/icons/application-exit.svg"),
                        tr("Quit"), qApp, &QApplication::quit);

  _tray_icon.setToolTip(QApplication::applicationDisplayName());
}

void Application::createWindows()
{
  for (int i = 0; i < _cfg->global().getNumInstances(); i++) {
    auto wnd = std::make_unique<ClockWindow>();

    wnd->setOriginPoint(wnd->originPoint() + QPoint(i*15, i*15));

    wnd->setWindowFlag(Qt::NoDropShadowWindowHint);
    wnd->setWindowFlag(Qt::FramelessWindowHint);
    wnd->setAttribute(Qt::WA_TranslucentBackground);

#ifdef Q_OS_WINDOWS
    wnd->setWindowFlag(Qt::Tool);   // trick to hide app icon from taskbar (Windows only)
#endif

    wnd->setWindowTitle(QString("%1 %2").arg(QApplication::applicationDisplayName()).arg(i));

    connect(wnd.get(), &ClockWindow::saveStateRequested, this, &Application::saveWindowState);

    connect(wnd.get(), &ClockWindow::settingsDialogRequested, this, &Application::showSettingsDialog);
    connect(wnd.get(), &ClockWindow::aboutDialogRequested, this, &Application::showAboutDialog);
    connect(wnd.get(), &ClockWindow::appExitRequested, this, &QApplication::quit);

    connect(&_tick_timer, &QTimer::timeout, wnd->clock(), &GraphicsDateTimeWidget::updateSeparatorsState);
#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
    connect(&_mouse_tracker, &MouseTracker::mousePositionChanged, wnd.get(), &ClockWindow::handleMouseMove);
#endif
#if defined(Q_OS_WINDOWS)
    connect(&_time_timer, &QTimer::timeout, wnd.get(), &ClockWindow::runStayOnTopHacks);
#endif

    _windows.push_back(std::move(wnd));

    configureWindow(_windows.size() - 1);

    _settings_rcv.push_back(std::make_unique<SettingsChangeListener>(this, i));
    _settings_tr.push_back(std::make_unique<SettingsChangeTransmitter>());
  }

  std::ranges::for_each(_windows, [](auto& w) { w->show(); });
  // window flags/properties must be applied to visible window
  for (auto& w : _windows) setPlatformWindowFlags(w.get());
}

void Application::initUpdater()
{
  if (!_cfg->global().getCheckForUpdates())
    return;

  _update_checker = std::make_unique<UpdateChecker>();
  _update_checker->setCheckForBeta(_cfg->global().getCheckForBetaVersion());

  connect(_update_checker.get(), &UpdateChecker::newVersion, this, &Application::handleNewVersion);

  _update_timer = std::make_unique<QTimer>();
  _update_timer->setSingleShot(true);

  connect(_update_timer.get(), &QTimer::timeout, this, &Application::maybeCheckForUpdate);

  auto save_last_update = [this]() {
    StateImpl app_state(_cfg->state());
    app_state.setValue("LastUpdateCheck", QDateTime::currentDateTime());
  };

  connect(_update_checker.get(), &UpdateChecker::newVersion, this, save_last_update);
  connect(_update_checker.get(), &UpdateChecker::upToDate, this, save_last_update);

  _update_timer->start(std::chrono::seconds(40));
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

void Application::addPositionMenu()
{
  auto pos_menu = _tray_menu->addMenu(tr("Position"));
  auto t_menu = pos_menu->addMenu(tr("Top"));
  t_menu->addAction(tr("Left"),   this, &Application::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignTop | Qt::AlignLeft));
  t_menu->addAction(tr("Middle"), this, &Application::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignTop | Qt::AlignHCenter));
  t_menu->addAction(tr("Right"),  this, &Application::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignTop | Qt::AlignRight));
  auto m_menu = pos_menu->addMenu(tr("Middle"));
  m_menu->addAction(tr("Left"),   this, &Application::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignVCenter | Qt::AlignLeft));
  m_menu->addAction(tr("Middle"), this, &Application::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignVCenter | Qt::AlignHCenter));
  m_menu->addAction(tr("Right"),  this, &Application::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignVCenter | Qt::AlignRight));
  auto b_menu = pos_menu->addMenu(tr("Bottom"));
  b_menu->addAction(tr("Left"),   this, &Application::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignBottom | Qt::AlignLeft));
  b_menu->addAction(tr("Middle"), this, &Application::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignBottom | Qt::AlignHCenter));
  b_menu->addAction(tr("Right"),  this, &Application::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignBottom | Qt::AlignRight));
}

void Application::setPlatformWindowFlags(ClockWindow* wnd)
{
  // many platform-specific flags require visible window
  if (!wnd->isVisible()) return;

  wnd->setStayOnTop(_cfg->global().getStayOnTop());
  wnd->setTransparentForInput(_cfg->global().getTransparentForMouse());
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
  _windows[idx]->activateWindow();
  _windows[idx]->raise();
  _dm.maybeCreateAndShowDialog<SettingsDialog>(0xbd480f59, this, idx);
}

void Application::showAboutDialog()
{
  _dm.maybeCreateAndShowDialog<AboutDialog>(0x41697269);
}

void Application::moveWindowToPredefinedPos()
{
  auto act = qobject_cast<QAction*>(sender());
  Q_ASSERT(act);
  _windows.front()->moveToPredefinedPos(act->data().value<Qt::Alignment>());
}

void Application::maybeCheckForUpdate()
{
  const StateImpl app_state(_cfg->state());
  const auto& last_update = app_state.value("LastUpdateCheck",
                                            QDateTime(QDate(2013, 6, 18), QTime(11, 20, 5), QTimeZone::utc()))
                                .toDateTime();
  const auto update_period = _cfg->global().getUpdatePeriodDays();
  if (last_update.daysTo(QDateTime::currentDateTime()) >= update_period)
    _update_checker->checkForUpdates();
}

void Application::handleNewVersion(QVersionNumber v, QDate d, QUrl l)
{
  disconnect(&_tray_icon, &QSystemTrayIcon::messageClicked, nullptr, nullptr);
  _tray_icon.showMessage(
        tr("%1 Update").arg(qApp->applicationName()),
        tr("Update available: %1 (%2).\nClick this message to download.")
            .arg(v.toString(), QLocale().toString(d, QLocale::ShortFormat)),
        QSystemTrayIcon::Information, -1);
  connect(&_tray_icon, &QSystemTrayIcon::messageClicked, [=] () { QDesktopServices::openUrl(l); });
}
