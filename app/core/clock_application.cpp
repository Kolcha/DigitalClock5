// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "clock_application.hpp"

#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtCore/QLibraryInfo>
#include <QtCore/QTimer>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>

#include "config/app_config.hpp"
#include "config/app_state.hpp"
#include "config/settings_backend_impl.hpp"
#include "core/plugin_manager_impl.hpp"
#include "core/skin_manager_impl.hpp"
#include "core/translation.hpp"
#include "core/tray_icon_manager_impl.hpp"
#include "core/update_checker.hpp"
#include "gui/about/about_dialog.hpp"
#include "gui/clock_tray_icon.hpp"
#include "gui/clock_widget.hpp"
#include "gui/clock_window.hpp"
#include "gui/settings/settings_dialog.hpp"
#include "platform/mouse_tracker.hpp"
#include "scale_factor.hpp"

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

static constexpr const char* const LAST_UPDATE_CHECK_KEY = "updater/last_update_check";

} // namespace

ClockApplication::ClockApplication(QObject* parent)
  : QObject(parent)
  , _tray_icon(std::make_unique<ClockTrayIcon>())
  , _tm(std::make_unique<TrayIconManagerImpl>(*_tray_icon))
  , _sm(std::make_unique<SkinManagerImpl>())
  , _tick_timer(std::make_unique<QTimer>())
  , _pm(std::make_unique<PluginManager>(this))
#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
  , _mouse_tracker(std::make_unique<MouseTracker>())
#endif
{
  connect(_pm.get(), &PluginManager::optionChanged, this, &ClockApplication::onInstanceOptionChanged);

  updatePaletteLinkColors();

  initConfig();
  loadTranslation();
  initTray();
  createWindows();
  reloadConfig();
  startTimers();
}

ClockApplication::~ClockApplication() = default;

ClockWindow* ClockApplication::window(size_t idx) const noexcept
{
  if (auto iter = _windows.find(idx); iter != _windows.end())
    return iter->second.get();
  return nullptr;
}

void ClockApplication::reconfigureWindow(size_t i)
{
  auto inst_cfg = _cfg->instance(i);
  auto glob_cfg = _cfg->global();

  Q_ASSERT(inst_cfg);
  Q_ASSERT(glob_cfg);

  const auto s = ScaleFactor(inst_cfg->getScaling() / 100.);
  applyInstanceOption(i, opt::Scaling, QVariant::fromValue(s));
  applyInstanceOption(i, opt::Opacity, inst_cfg->getOpacity());
  applyInstanceOption(i, opt::UseSkin, inst_cfg->getUseSkin());

  applyInstanceOption(i, opt::TextureType, inst_cfg->getTextureType());
  applyInstanceOption(i, opt::BackgroundType, inst_cfg->getBackgroundType());

  applyInstanceOption(i, opt::TruePerCharRendering, inst_cfg->getTruePerCharRendering());

  applyInstanceOption(i, opt::ColorizationColor, inst_cfg->getColorizationColor());
  applyInstanceOption(i, opt::ColorizationStrength, inst_cfg->getColorizationStrength());
  applyInstanceOption(i, opt::ColorizationEnabled, inst_cfg->getColorizationEnabled());

  applyInstanceOption(i, opt::LayoutSpacing, inst_cfg->getLayoutSpacing());
  applyInstanceOption(i, opt::CharSpacing, inst_cfg->getCharSpacing());
  applyInstanceOption(i, opt::LineSpacing, inst_cfg->getLineSpacing());
  applyInstanceOption(i, opt::RespectLineSpacing, inst_cfg->getRespectLineSpacing());

  applyInstanceOption(i, opt::CharMargins, QVariant::fromValue(inst_cfg->getCharMargins()));
  applyInstanceOption(i, opt::WidgetMargins, QVariant::fromValue(inst_cfg->getWidgetMargins()));
  applyInstanceOption(i, opt::LayoutMargins, QVariant::fromValue(inst_cfg->getLayoutMargins()));

  applyInstanceOption(i, opt::TimeFormat, inst_cfg->getTimeFormat());
  applyInstanceOption(i, opt::LayoutConfig, inst_cfg->getLayoutConfig());

  applyInstanceOption(i, opt::SmallerSeconds, inst_cfg->getSmallerSeconds());

  applyInstanceOption(i, opt::ShowLocalTime, inst_cfg->getShowLocalTime());
  applyInstanceOption(i, opt::TimeZone, QVariant::fromValue(inst_cfg->getTimeZone()));

  applyInstanceOption(i, opt::FlashingSeparator, inst_cfg->getFlashingSeparator());
  applyInstanceOption(i, opt::UseCustomSeparators, inst_cfg->getUseCustomSeparators());

  applyWindowOption(i, opt::StayOnTop, glob_cfg->getStayOnTop());
  applyWindowOption(i, opt::FullscreenDetect, glob_cfg->getFullscreenDetect());
  applyWindowOption(i, opt::TransparentForMouse, glob_cfg->getTransparentForMouse());

  applyWindowOption(i, opt::SnapToEdge, glob_cfg->getSnapToEdge());
  applyWindowOption(i, opt::SnapThreshold, glob_cfg->getSnapThreshold());
  applyWindowOption(i, opt::PreventOutOfScreen, glob_cfg->getPreventOutOfScreen());

  applyWindowOption(i, opt::ChangeCursorOnHover, glob_cfg->getChangeCursorOnHover());

  applyWindowOption(i, opt::TransparentOnHover, glob_cfg->getTransparentOnHover());
  applyWindowOption(i, opt::OpacityOnHover, glob_cfg->getOpacityOnHover());

  auto wnd = window(i);
  wnd->loadState(StateStorage(*_state->instance(i)));
  wnd->show();

  // due to applyInstanceOption() implementation quirks, not all options are tracked
  // hence some necessary ones must be provided to the plugin system explicitly
  _pm->onOptionChanged(i, opt::TextureCustomization, QVariant::fromValue(inst_cfg->getTextureCustomization()));
  _pm->onOptionChanged(i, opt::TextureColor, inst_cfg->getTextureColor());
  _pm->onOptionChanged(i, opt::TextureColorUseTheme, inst_cfg->getTextureColorUseTheme());
  _pm->onOptionChanged(i, opt::TextureGradient, QVariant::fromValue(inst_cfg->getTextureGradient()));
  _pm->onOptionChanged(i, opt::TexturePattern, inst_cfg->getTexturePattern());
  _pm->onOptionChanged(i, opt::TexturePatternTile, inst_cfg->getTexturePatternTile());
  _pm->onOptionChanged(i, opt::BackgroundCustomization, QVariant::fromValue(inst_cfg->getBackgroundCustomization()));
  _pm->onOptionChanged(i, opt::BackgroundColor, inst_cfg->getBackgroundColor());
  _pm->onOptionChanged(i, opt::BackgroundColorUseTheme, inst_cfg->getBackgroundColorUseTheme());
  _pm->onOptionChanged(i, opt::BackgroundGradient, QVariant::fromValue(inst_cfg->getBackgroundGradient()));
  _pm->onOptionChanged(i, opt::BackgroundPattern, inst_cfg->getBackgroundPattern());
  _pm->onOptionChanged(i, opt::BackgroundPatternTile, inst_cfg->getBackgroundPatternTile());
}

void ClockApplication::reloadConfig()
{
  _pm->unloadAll();

  onGlobalOptionChanged(opt::Locale, _cfg->global()->getLocale());
  onGlobalOptionChanged(opt::ActiveInstances, QVariant::fromValue(_cfg->global()->getActiveInstancesList()));
  onGlobalOptionChanged(opt::OptionsSharing, _cfg->global()->getOptionsSharing());

  updateInstances();                // add/remove windows

  for (const auto& [i, w] : _windows) reconfigureWindow(i);

  onGlobalOptionChanged(opt::CheckForUpdates, _cfg->global()->getCheckForUpdates());

  _pm->load(_cfg->global()->getPlugins());
}

bool ClockApplication::eventFilter(QObject* obj, QEvent* e)
{
  if (e->type() == QEvent::ApplicationPaletteChange) {
    if (updatePaletteLinkColors())
      return true;
  }

  if (e->type() == QEvent::PaletteChange) {
    if (auto w = qobject_cast<ClockWindow*>(obj)) {
      auto idx = findWindow(w);
      Q_ASSERT(idx != -1);
      auto inst_cfg = _cfg->instance(idx);
      if (inst_cfg->getTextureType() == tx::Color && inst_cfg->getTextureColorUseTheme())
        applyInstanceOption(idx, opt::TextureColorUseTheme, true);
      if (inst_cfg->getBackgroundType() == tx::Color && inst_cfg->getBackgroundColorUseTheme())
        applyInstanceOption(idx, opt::BackgroundColorUseTheme, true);
    }
  }

  return QObject::eventFilter(obj, e);
}

void ClockApplication::tick()
{
  auto now = QDateTime::currentDateTime();
  for (const auto& [i, w] : _windows) {
    auto inst_cfg = _cfg->instance(i);
    if (!inst_cfg->getShowLocalTime()) {  // is it expensive (QVariant)?
      auto tz_dt = now.toTimeZone(inst_cfg->getTimeZone());
      w->clock()->setDateTime(tz_dt);
      _pm->tick(i, tz_dt);
    } else {
      w->clock()->setDateTime(now);
      _pm->tick(i, now);
    }
    w->clock()->animateSeparator();
  }
}

void ClockApplication::saveWindowState()
{
  auto wnd = qobject_cast<ClockWindow*>(sender());
  Q_ASSERT(wnd);
  auto idx = findWindow(wnd);

  StateStorage wst(*_state->instance(idx));
  wnd->saveState(wst);
}

void ClockApplication::showSettingsDialog()
{
  std::ranges::for_each(_windows, [](const auto& p) { auto& [_, w] = p; if (!w->isVisible()) w->show(); });
  auto wnd = qobject_cast<ClockWindow*>(sender());
  size_t idx = wnd ? findWindow(wnd) : _windows.begin()->first;
  _windows[idx]->activateWindow();
  _windows[idx]->raise();
  auto dlg = _dm.maybeCreateAndShowDialog<SettingsDialog>(0xbd480f59, this, idx);
  connect(dlg, &SettingsDialog::globalOptionChanged, this, &ClockApplication::onGlobalOptionChanged);
  connect(dlg, &SettingsDialog::instanceOptionChanged, this, &ClockApplication::onInstanceOptionChanged);
  connect(dlg, &SettingsDialog::accepted, _cfg.get(), &AppConfig::commit);
  connect(dlg, &SettingsDialog::rejected, _cfg.get(), &AppConfig::discard);
  connect(dlg, &SettingsDialog::rejected, this, &ClockApplication::reloadConfig);
}

void ClockApplication::showAboutDialog()
{
  _dm.maybeCreateAndShowDialog<AboutDialog>(0x41697269);
}

void ClockApplication::moveWindowToPredefinedPos()
{
  auto act = qobject_cast<QAction*>(sender());
  Q_ASSERT(act);
  Q_ASSERT(!_windows.empty());
  _windows.begin()->second->moveToPredefinedPos(act->data().value<Qt::Alignment>());
}

void ClockApplication::maybeCheckForUpdate()
{
  const StateStorage app_state(*_state->global());
  constexpr const qint64 def_update = 1371554405;
  const auto last_update_ts = app_state.value(LAST_UPDATE_CHECK_KEY, def_update).value<qint64>();
  const auto last_update = QDateTime::fromSecsSinceEpoch(last_update_ts, QTimeZone::utc());

  const auto update_period = _cfg->global()->getUpdatePeriodDays();
  if (last_update.daysTo(QDateTime::currentDateTime()) >= update_period)
    _update_checker->checkForUpdates();
}

void ClockApplication::handleNewVersion(const QVersionNumber& v, const QDate& d, const QUrl& l)
{
  disconnect(_tray_icon.get(), &QSystemTrayIcon::messageClicked, nullptr, nullptr);
  // *INDENT-OFF*
  _tray_icon->showMessage(
        tr("%1 Update").arg(qApp->applicationName()),
        tr("Update available: %1 (%2).\nClick this message to download.")
            .arg(v.toString(), QLocale().toString(d, QLocale::ShortFormat)),
        QSystemTrayIcon::Information, -1);
  // *INDENT-ON*
  connect(_tray_icon.get(), &QSystemTrayIcon::messageClicked, [=] () { QDesktopServices::openUrl(l); });
}

void ClockApplication::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
  switch (reason) {
    case QSystemTrayIcon::DoubleClick:
      applyTrayIconDoubleClickAction();
      break;
    case QSystemTrayIcon::Trigger:
      applyTrayIconSingleClickAction();
      break;
    default:
      break;
  }
}

void ClockApplication::initConfig()
{
  using namespace Qt::Literals::StringLiterals;
  QDir app_dir(QCoreApplication::applicationDirPath());
  if (app_dir.exists(u"portable.txt"_s) || app_dir.exists(u".portable"_s))
    _settings = std::make_unique<SettingsBackendImpl>(app_dir.absoluteFilePath(u"settings.ini"_s));
  else
    _settings = std::make_unique<SettingsBackendImpl>();
  _cfg = std::make_unique<AppConfig>(*_settings);
  _state = std::make_unique<AppState>(*_settings);

  // this is not nice, but safe enough: here we know what actually was created
  static_cast<SkinManagerImpl*>(_sm.get())->setSkinBaseSize(_cfg->global()->getSkinBaseSize());
}

void ClockApplication::loadTranslation()
{
  auto locale = _cfg->global()->getLocale();
  const bool detect = locale == "auto";

  if (locale == _last_lang && !detect)
    return;

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
    _active_lang.replace('-', '_');
    _translators.push_back(std::move(app_translator));
  }

  _last_lang = locale;
}

void ClockApplication::initTray()
{
#ifdef Q_OS_WINDOWS
  connect(_tray_icon.get(), &QSystemTrayIcon::activated, this, &ClockApplication::onTrayIconActivated);
#endif
  _tray_icon->show();

  _tray_menu = std::make_unique<QMenu>();
  _tray_icon->setContextMenu(_tray_menu.get());

  _tray_menu->addAction(QIcon(":/icons/configure.svg"), tr("Settings"),
                        this, &ClockApplication::showSettingsDialog);

  if (const auto insts = _cfg->global()->getActiveInstancesList(); insts.size() == 1)
    addPositionMenu();

  _tray_menu->addSeparator();
  _tray_menu->addAction(QIcon(":/icons/help-about.svg"), tr("About"),
                        this, &ClockApplication::showAboutDialog);
  _tray_menu->addSeparator();
  _tray_menu->addAction(QIcon(":/icons/application-exit.svg"),
                        tr("Quit"), qApp, &QCoreApplication::quit);

  // _tray_icon.setToolTip(QApplication::applicationDisplayName());
}

void ClockApplication::createWindows()
{
  const auto act_insts = _cfg->global()->getActiveInstancesList();
  for (auto i : act_insts)
    _windows[i] = createWindow(i);
}

void ClockApplication::initUpdater()
{
  if (!_cfg->global()->getCheckForUpdates())
    return;

  _update_checker = std::make_unique<UpdateChecker>();
  _update_checker->setCheckForBeta(_cfg->global()->getCheckForBetaVersion());

  connect(_update_checker.get(), &UpdateChecker::newVersion, this, &ClockApplication::handleNewVersion);

  auto update_timer = new QTimer(this);
  update_timer->setSingleShot(true);

  connect(update_timer, &QTimer::timeout, this, &ClockApplication::maybeCheckForUpdate);
  connect(update_timer, &QTimer::timeout, update_timer, &QTimer::deleteLater);

  auto save_last_update = [this]() {
    StateStorage app_state(*_state->global());
    app_state.setValue(LAST_UPDATE_CHECK_KEY, QDateTime::currentSecsSinceEpoch());
  };

  connect(_update_checker.get(), &UpdateChecker::newVersion, this, save_last_update);
  connect(_update_checker.get(), &UpdateChecker::upToDate, this, save_last_update);

  update_timer->start(std::chrono::seconds(40));
}

void ClockApplication::startTimers()
{
  connect(_tick_timer.get(), &QTimer::timeout, this, &ClockApplication::tick);
  _tick_timer->start(500);
}

void ClockApplication::retranslateUI()
{
  std::ranges::for_each(_translators, [](auto& t) { QApplication::removeTranslator(t.get()); });
  _translators.clear();
  _active_lang = "en_US";

  loadTranslation();
  _pm->retranslate(_active_lang);
}

void ClockApplication::updateInstances()
{
  const auto active_insts = _cfg->global()->getActiveInstancesList();
  QSet<size_t> desired_insts(active_insts.begin(), active_insts.end());
  Q_ASSERT(!desired_insts.empty());

  QSet<size_t> existing_insts;
  for (const auto& [i, _] : _windows)
    existing_insts.insert(i);

  const auto to_remove = existing_insts - desired_insts;
  for (const auto i : to_remove) {
    _windows.erase(i);
    _cfg->removeInstance(i);
  }

  const auto to_add = desired_insts - existing_insts;
  for (const auto i : to_add) {
    _windows[i] = createWindow(i);
    _cfg->addInstance(i);
  }
}

std::unique_ptr<ClockWindow> ClockApplication::createWindow(size_t i)
{
  auto wnd = std::make_unique<ClockWindow>();
  wnd->setWindowTitle(QString("%1-%2").arg(QCoreApplication::applicationName()).arg(i));
  connect(wnd.get(), &ClockWindow::saveStateRequested, this, &ClockApplication::saveWindowState);
  connect(wnd.get(), &ClockWindow::settingsDialogRequested, this, &ClockApplication::showSettingsDialog);
  connect(wnd.get(), &ClockWindow::aboutDialogRequested, this, &ClockApplication::showAboutDialog);
  connect(wnd.get(), &ClockWindow::appExitRequested, qApp, &QCoreApplication::quit);
#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
  connect(_mouse_tracker.get(), &MouseTracker::mousePositionChanged, wnd.get(), &ClockWindow::handleMouseMove);
#endif
  connect(_tick_timer.get(), &QTimer::timeout, wnd.get(), &ClockWindow::tick);
  wnd->installEventFilter(this);
  return wnd;
}

size_t ClockApplication::findWindow(ClockWindow* wnd) const
{
  for (auto& [i, w] : _windows)
    if (w.get() == wnd)
      return i;
  return -1;
}

void ClockApplication::addPositionMenu()
{
  auto pos_menu = _tray_menu->addMenu(tr("Position"));
  auto t_menu = pos_menu->addMenu(QIcon(":/icons/align/top.svg"), tr("Top"));
  // *INDENT-OFF*
  t_menu->addAction(QIcon(":/icons/align/top-left.svg"), tr("Left"),
                    this, &ClockApplication::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignTop | Qt::AlignLeft));
  t_menu->addAction(QIcon(":/icons/align/top.svg"), tr("Middle"),
                    this, &ClockApplication::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignTop | Qt::AlignHCenter));
  t_menu->addAction(QIcon(":/icons/align/top-right.svg"), tr("Right"),
                    this, &ClockApplication::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignTop | Qt::AlignRight));
  auto m_menu = pos_menu->addMenu(QIcon(":/icons/align/v-center.svg"), tr("Middle"));
  m_menu->addAction(QIcon(":/icons/align/left.svg"), tr("Left"),
                    this, &ClockApplication::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignVCenter | Qt::AlignLeft));
  m_menu->addAction(QIcon(":/icons/align/h-center.svg"), tr("Middle"),
                    this, &ClockApplication::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignVCenter | Qt::AlignHCenter));
  m_menu->addAction(QIcon(":/icons/align/right.svg"), tr("Right"),
                    this, &ClockApplication::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignVCenter | Qt::AlignRight));
  auto b_menu = pos_menu->addMenu(QIcon(":/icons/align/bottom.svg"), tr("Bottom"));
  b_menu->addAction(QIcon(":/icons/align/bottom-left.svg"), tr("Left"),
                    this, &ClockApplication::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignBottom | Qt::AlignLeft));
  b_menu->addAction(QIcon(":/icons/align/bottom.svg"), tr("Middle"),
                    this, &ClockApplication::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignBottom | Qt::AlignHCenter));
  b_menu->addAction(QIcon(":/icons/align/bottom-right.svg"), tr("Right"),
                    this, &ClockApplication::moveWindowToPredefinedPos)
      ->setData(QVariant::fromValue(Qt::AlignBottom | Qt::AlignRight));
  // *INDENT-ON*
}

void ClockApplication::applyTrayIconAction(opt::TrayIconAction act)
{
  switch (act) {
    case opt::NoAction:
      break;
    case opt::OpenSettings:
      showSettingsDialog();
      break;
    case opt::ShowHideClock:
      std::ranges::for_each(_windows, [](const auto& p) { auto& [_, w] = p; w->setVisible(!w->isVisible()); });
      break;
    case opt::ToggleStayOnTop:
      _cfg->global()->setStayOnTop(!_cfg->global()->getStayOnTop());
      onGlobalOptionChanged(opt::StayOnTop, _cfg->global()->getStayOnTop());
      break;
  }
}

void ClockApplication::applyTrayIconSingleClickAction()
{
  applyTrayIconAction(_cfg->global()->getTrayIconSingleClickAction());
}

void ClockApplication::applyTrayIconDoubleClickAction()
{
  applyTrayIconAction(_cfg->global()->getTrayIconDoubleClickAction());
}
