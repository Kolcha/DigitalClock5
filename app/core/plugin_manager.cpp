/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "plugin_manager.hpp"

#include <QDir>
#include <QFileInfo>

#include <QApplication>
#include <QPluginLoader>

#include "clock_plugin.hpp"

#include "application.hpp"
#include "translation.hpp"
#include "gui/plugin_settings_dialog.hpp"

class PluginHandle {
public:
  PluginHandle(const QString& filename, const QString& lang);
  ~PluginHandle();

  PluginHandle(const PluginHandle&) = delete;
  PluginHandle(PluginHandle&&) = default;

  PluginHandle& operator=(const PluginHandle&) = delete;
  PluginHandle& operator=(PluginHandle&&) = default;

  operator bool() const { return _loader && plugin(); }

  ClockPluginFactory* plugin() const;

  QString file() const;
  // result is not cached for the both of functions
  // everything is recalculated each time it called
  QString id() const;
  QVariantHash metadata() const;

  ClockPluginBase* addInstance();
  ClockPluginBase* instance(size_t i) const;
  void createInstances(int max_inst);
  auto& instances() const { return _instances; }

private:
  std::unique_ptr<QPluginLoader> _loader;
  std::unique_ptr<QTranslator> _translator;
  std::vector<std::unique_ptr<ClockPluginBase>> _instances;
};

PluginHandle::PluginHandle(const QString& filename, const QString& lang)
    : _loader(std::make_unique<QPluginLoader>(filename))
{
  _loader->load();

  _translator = loadTranslation(id(), lang);
  if (_translator)
    QApplication::installTranslator(_translator.get());
}

PluginHandle::~PluginHandle()
{
  if (_loader)
    _loader->unload();
  if (_translator)
    QApplication::removeTranslator(_translator.get());
}

ClockPluginFactory* PluginHandle::plugin() const
{
  if (!_loader)
    return nullptr;
  return qobject_cast<ClockPluginFactory*>(_loader->instance());
}

ClockPluginBase* PluginHandle::addInstance()
{
  if (!plugin())
    return nullptr;

  if (auto inst = plugin()->create()) {
    _instances.push_back(std::move(inst));
    return _instances.back().get();
  }

  return nullptr;
}

void PluginHandle::createInstances(int max_inst)
{
  if (auto p = plugin()) {
    if (p->perClockInstance()) {
      for (int i = 0; i < max_inst; i++)
        addInstance();
    } else {
      addInstance();
    }
  }
}

ClockPluginBase* PluginHandle::instance(size_t i) const
{
  if (i >= _instances.size())
    return nullptr;
  return _instances[i].get();
}

QString PluginHandle::file() const
{
  if (!_loader)
    return {};
  return _loader->fileName();
}

QString PluginHandle::id() const
{
  if (!_loader)
    return {};

  auto fallback = QFileInfo(file()).baseName();
  return metadata().value("name", fallback).toString();
}

QVariantHash PluginHandle::metadata() const
{
  if (!_loader)
    return {};
  return _loader->metaData().value("MetaData").toObject().toVariantHash();
}


class ChangeRetransmitter : public QObject
{
  Q_OBJECT

public:
  explicit ChangeRetransmitter(QObject* parent = nullptr)
      : QObject(parent)
  {}

signals:
  void optionChanged(cs::SharedConfigKeys, const QVariant&);
};


class ConfigurePluginHandle : public QObject
{
  Q_OBJECT

public:
  // plugin is already loaded
  explicit ConfigurePluginHandle(Application* app, PluginHandle* h, QObject* parent = nullptr);
  // plugin is not loaded
  explicit ConfigurePluginHandle(Application* app, const QString& p, QObject* parent = nullptr);

  ~ConfigurePluginHandle();

  void openDialog();

private slots:
  void onInstanceChanged(int idx);

  void onAccepted();
  void onRejected();

private:
  Application* _app = nullptr;
  PluginSettingsDialog* _dlg = nullptr;
  // shared_ptr allows to set deleter on runtime
  std::shared_ptr<PluginHandle> _handle;
  const bool _was_loaded = false;
  // window highlight tracking
  int _init_hl_wnd = 0;
  int _curr_hl_wnd = 0;
};

ConfigurePluginHandle::ConfigurePluginHandle(Application* app, PluginHandle* h, QObject* parent)
    : QObject(parent)
    , _app(app)
    , _handle(h, [](auto) { /* not owned pointer */ })
    , _was_loaded(true)
{
}

ConfigurePluginHandle::ConfigurePluginHandle(Application* app, const QString& p, QObject* parent)
    : QObject(parent)
    , _app(app)
    , _handle(std::make_shared<PluginHandle>(p, app->activeLang()))
    , _was_loaded(false)
{
  _handle->createInstances(app->config().global().getNumInstances());
}

ConfigurePluginHandle::~ConfigurePluginHandle()
{
  _app->window(_curr_hl_wnd)->disableFrame();
  _app->window(_init_hl_wnd)->enableFrame();
}

void ConfigurePluginHandle::openDialog()
{
  for (int i = 0; i < _app->config().global().getNumInstances(); i++) {
    if (_app->window(i)->frameVisible()) {
      _init_hl_wnd = i;
      _curr_hl_wnd = i;
      break;
    }
  }

  _dlg = new PluginSettingsDialog();
  _dlg->setWindowTitle(_handle->plugin()->title());

  connect(_dlg, &QDialog::accepted, this, &ConfigurePluginHandle::onAccepted);
  connect(_dlg, &QDialog::rejected, this, &ConfigurePluginHandle::onRejected);

  connect(_dlg, &PluginSettingsDialog::instanceChanged, this, &ConfigurePluginHandle::onInstanceChanged);

  connect(_dlg, &QDialog::finished, _dlg, &QObject::deleteLater);
  connect(_dlg, &QObject::destroyed, this, &ConfigurePluginHandle::deleteLater);

  _dlg->setNumInstances(_handle->plugin()->perClockInstance() ? _handle->instances().size() : 1);

  onInstanceChanged(0);

  _dlg->open();
}

void ConfigurePluginHandle::onInstanceChanged(int idx)
{
  auto sp = dynamic_cast<ConfigurablePlugin*>(_handle->instance(idx));
  Q_ASSERT(sp);

  auto& plg_cfg = _app->config().plugin(_handle->id());
  _dlg->setWidget(sp->configure(plg_cfg.config(idx), plg_cfg.state(idx)));

  _app->window(_curr_hl_wnd)->disableFrame();
  _curr_hl_wnd = idx;
  _app->window(_curr_hl_wnd)->enableFrame();
}

void ConfigurePluginHandle::onAccepted()
{
  for (size_t i = 0; i < _handle->instances().size(); i++) {
    _app->config().plugin(_handle->id()).config(i).commit();
    dynamic_cast<ConfigurablePlugin*>(_handle->instance(i))->accepted();
  }
}

void ConfigurePluginHandle::onRejected()
{
  for (size_t i = 0; i < _handle->instances().size(); i++) {
    _app->config().plugin(_handle->id()).config(i).discard();
    dynamic_cast<ConfigurablePlugin*>(_handle->instance(i))->rejected();
  }

  if (_was_loaded) {
    for (size_t i = 0; i < _handle->instances().size(); i++) {
      auto cp = dynamic_cast<ConfigurablePlugin*>(_handle->instance(i));
      Q_ASSERT(cp);
      cp->initSettings(_app->config().plugin(_handle->id()).config(i));
    }
  }
}


struct PluginManager::Impl {
  Application* app = nullptr;

  std::unordered_map<QString, PluginInfo> available;
  std::unordered_map<QString, PluginHandle> loaded;

  std::vector<std::unique_ptr<ChangeRetransmitter>> cfg_sinks_clk;  // from clock to plugins
  std::vector<std::unique_ptr<ChangeRetransmitter>> cfg_sinks_plg;  // to clock/plugins

  QTimer timer;   // independent timer for periodic tick() calls

  void enumerate();

  void load(const QString& id);
  void unload(const QString& id);

  void configure(const QString& id);

private:
  void initAllInterfaces(const PluginHandle& h);
  void connectEverything(const PluginHandle& h);
};

void PluginManager::Impl::enumerate()
{
  QDir d(QApplication::applicationDirPath());
#ifdef Q_OS_MACOS
  if (!d.cd("../PlugIns")) return;
#else
  if (!d.cd("plugins")) return;
#endif
  const auto files = d.entryList(QDir::Files);
  for (const auto& f : files) {
    PluginHandle handle(d.absoluteFilePath(f), app->activeLang());
    if (auto iface = handle.plugin()) {
      auto metadata = handle.metadata();
      auto id = handle.id();
      auto& info = available[id];
      info.path = handle.file();
      info.id = id;
      info.configurable = metadata.value("configurable", false).toBool();
      info.multiinstance = iface->perClockInstance();
    }
  }
}

void PluginManager::Impl::load(const QString& id)
{
  auto iter = available.find(id);
  if (iter == available.end())
    return;

  if (loaded.contains(id))
    return;

  auto& handle = loaded.emplace(id, PluginHandle{iter->second.path, app->activeLang()}).first->second;
  if (!handle) {
    loaded.erase(id);   // not super-efficient, but easy and still fine
    return;
  }

  handle.createInstances(app->config().global().getNumInstances());

  initAllInterfaces(handle);
  connectEverything(handle);

  std::ranges::for_each(handle.instances(), [](auto& p) { p->init(); });
}

void PluginManager::Impl::unload(const QString& id)
{
  auto iter = loaded.find(id);
  if (iter == loaded.end())
    return;

  std::ranges::for_each(iter->second.instances(), [](auto& p) { p->shutdown(); });

  loaded.erase(iter);
}

void PluginManager::Impl::configure(const QString& id)
{
  auto aiter = available.find(id);
  if (aiter == available.end())
    return;

  // this object is "self-destructive" thanks to QObject
  ConfigurePluginHandle* cfg_state;
  if (auto iter = loaded.find(id); iter != loaded.end())
    cfg_state = new ConfigurePluginHandle(app, &iter->second);
  else
    cfg_state = new ConfigurePluginHandle(app, aiter->second.path);

  cfg_state->openDialog();
}

void PluginManager::Impl::initAllInterfaces(const PluginHandle& h)
{
  for (size_t i = 0; i < h.instances().size(); i++) {
    // load state and config
    if (auto cp = dynamic_cast<ConfigurablePlugin*>(h.instance(i))) {
      auto& plg_cfg = app->config().plugin(h.id());
      cp->initState(&plg_cfg.state(i));
      cp->initSettings(plg_cfg.config(i));
    }
    // provide shared settings
    if (auto sp = dynamic_cast<SettingsPlugin*>(h.instance(i))) {
      auto& wcfg = app->config().window(i).appearance();
      SettingsPlugin::SharedSettings ss;
      ss[cs::Opacity] = wcfg.getOpacity();
      ss[cs::ScalingH] = wcfg.getScalingH();
      ss[cs::ScalingV] = wcfg.getScalingV();
      ss[cs::SpacingH] = wcfg.getSpacingH();
      ss[cs::SpacingV] = wcfg.getSpacingV();
      ss[cs::ApplyColorization] = wcfg.getApplyColorization();
      ss[cs::ColorizationStrength] = wcfg.getColorizationStrength();
      ss[cs::ColorizationColor] = wcfg.getColorizationColor();
      ss[cs::UseFontInsteadOfSkin] = wcfg.getUseFontInsteadOfSkin();
      ss[cs::Font] = wcfg.getFont();
      ss[cs::Skin] = wcfg.getSkin();
      ss[cs::SeparatorFlashes] = wcfg.getSeparatorFlashes();
      ss[cs::UseCustomSeparators] = wcfg.getUseCustomSeparators();
      ss[cs::CustomSeparators] = wcfg.getCustomSeparators();
      ss[cs::TimeFormat] = wcfg.getTimeFormat();
      ss[cs::SecondsScaleFactor] = wcfg.getSecondsScaleFactor();
      ss[cs::UseSystemForeground] = wcfg.getUseSystemForeground();
      ss[cs::Texture] = wcfg.getTexture();
      ss[cs::TextureStretch] = wcfg.getTextureStretch();
      ss[cs::TexturePerCharacter] = wcfg.getTexturePerCharacter();
      ss[cs::UseSystemBackground] = wcfg.getUseSystemBackground();
      ss[cs::Background] = wcfg.getBackground();
      ss[cs::BackgroundStretch] = wcfg.getBackgroundStretch();
      ss[cs::BackgroundPerCharacter] = wcfg.getBackgroundPerCharacter();
      sp->initSharedSettings(ss);
    }
    // provide skin settings
    if (auto sa = dynamic_cast<SkinAccessExtension*>(h.instance(i))) {
      auto wnd = app->window(i)->clock();
      sa->initSkin(wnd->skin());
      SkinAccessExtension::AppearanceSettings as;
      as[cs::UseSystemForeground] = wnd->useSystemForeground();
      as[cs::Texture] = wnd->texture();
      as[cs::TextureStretch] = wnd->textureStretch();
      as[cs::TexturePerCharacter] = wnd->texturePerChar();
      as[cs::UseSystemBackground] = wnd->useSystemBackground();
      as[cs::Background] = wnd->background();
      as[cs::BackgroundStretch] = wnd->backgroundStretch();
      as[cs::BackgroundPerCharacter] = wnd->backgroundPerChar();
      sa->initAppearance(as);
    }
    // provide clock window
    if (auto wa = dynamic_cast<WindowAccessExtension*>(h.instance(i))) {
      wa->initWindow(app->window(i));
    }
  }
}

void PluginManager::Impl::connectEverything(const PluginHandle& h)
{
  const auto& mdata = h.metadata();
  for (size_t i = 0; i < h.instances().size(); i++) {
    // generic plugin interface
    if (mdata.value("time_listener", false).toBool()) {
      QObject::connect(app->window(i)->clock(), &GraphicsDateTimeWidget::dateTimeChanged,
                       h.instance(i), &ClockPluginBase::onTimeChanged);
      // plugin may need date/time (or time zone) for initialization, so call handler now
      h.instance(i)->onTimeChanged(app->window(i)->clock()->currentDateTime());
    }
    // always connect tick()
    QObject::connect(&timer, &QTimer::timeout, h.instance(i), &ClockPluginBase::tick);
    // settings plugin interface
    if (auto sp = dynamic_cast<SettingsPlugin*>(h.instance(i))) {
      if (mdata.value("settings_listener", false).toBool()) {
        auto idx = h.plugin()->perClockInstance() ? i : 0;
        QObject::connect(cfg_sinks_clk[idx].get(), &ChangeRetransmitter::optionChanged,
                         sp, &SettingsPlugin::onOptionChanged);
        QObject::connect(cfg_sinks_plg[idx].get(), &ChangeRetransmitter::optionChanged,
                         sp, &SettingsPlugin::onOptionChanged);
      }
      if (mdata.value("settings_notifier", false).toBool()) {
        auto idx = h.plugin()->perClockInstance() ? i : 0;
        QObject::connect(sp, &SettingsPlugin::optionChanged,
                         cfg_sinks_clk[idx].get(), &ChangeRetransmitter::optionChanged);
        QObject::connect(sp, &SettingsPlugin::optionChanged,
                         cfg_sinks_plg[idx].get(), &ChangeRetransmitter::optionChanged);
      }
    }
    // skin access extension
    if (auto sp = dynamic_cast<SkinAccessExtension*>(h.instance(i))) {
      QObject::connect(app->window(i)->clock(), &GraphicsWidgetBase::skinChanged,
                       h.instance(i), [sp](auto skin) { sp->initSkin(skin); });
    }
  }
}


namespace {

QIcon plugin_icon(QStringView id)
{
  const auto known_names = {
      QString(":/icons/%1").arg(id),
      QString(":/%1/icon").arg(id),
      QString(":/%1/logo").arg(id),
  };

  const auto known_exts = {"svg", "png"};

  for (const auto& p : known_names) {
    for (const auto& e : known_exts) {
      auto fn = QString("%1.%2").arg(p, e);
      if (QFile::exists(fn)) {
        return QIcon(fn);
      }
    }
  }

  return QIcon(":/icons/plugins.svg");
}


void fill_plugin_info(const PluginHandle& handle, PluginInfo& info)
{
  if (auto iface = handle.plugin()) {
    info.title = iface->title();
    info.description = iface->description();
    info.metadata = handle.metadata();
    // QIcon doesn't load file on creation, just save the path
    // so, when plugin unloads, icon file becomes unavailable
    info.icon = plugin_icon(handle.id()).pixmap(96);
  }
}

} // namespace


PluginManager::PluginManager(Application* app, QObject* parent)
    : QObject(parent)
    , _impl(std::make_unique<Impl>())
{
  _impl->app = app;
}

PluginManager::~PluginManager()
{
  // shutdown all plugins on destruction
  // iterate over 'available' rather than 'loaded' because
  // 'unload()' removes items, so iterators are invalidated
  // it is safe to "unload" not loaded plugin
  for (const auto& [id, _] : _impl->available)
    _impl->unload(id);
}

void PluginManager::init()
{
  _impl->enumerate();

  for (int i = 0; i < _impl->app->config().global().getNumInstances(); i++) {
    _impl->cfg_sinks_clk.push_back(std::make_unique<ChangeRetransmitter>());
    _impl->cfg_sinks_plg.push_back(std::make_unique<ChangeRetransmitter>());
  }

  // settings sync with the clock
  bool same_appearance = !_impl->app->config().global().getAppearancePerInstance();
  if (same_appearance) {
    // clock 0 -> all sinks
    for (const auto& s : _impl->cfg_sinks_clk) {
      connect(_impl->app->settingsTransmitter(0), &SettingsChangeTransmitter::optionChanged,
              s.get(), &ChangeRetransmitter::optionChanged);
    }
    // sink 0 -> all clocks
    for (const auto& l : _impl->app->settingsListeners()) {
      connect(_impl->cfg_sinks_plg[0].get(), &ChangeRetransmitter::optionChanged,
              l.get(), &SettingsChangeListener::onOptionChanged);
    }
  } else {
    // one-to-one
    Q_ASSERT(_impl->cfg_sinks_clk.size() == _impl->cfg_sinks_plg.size());
    for (size_t i = 0; i < _impl->cfg_sinks_clk.size(); i++) {
      auto cl = _impl->app->settingsListener(i);
      auto ct = _impl->app->settingsTransmitter(i);
      auto ci = _impl->cfg_sinks_clk[i].get();
      auto po = _impl->cfg_sinks_plg[i].get();
      connect(po, &ChangeRetransmitter::optionChanged, cl, &SettingsChangeListener::onOptionChanged);
      connect(ct, &SettingsChangeTransmitter::optionChanged, ci, &ChangeRetransmitter::optionChanged);
    }
  }

  _impl->timer.start(1000);
}

QStringList PluginManager::availablePlugins() const
{
  QStringList ids;
  for (const auto& [id, _] : _impl->available)
    ids.push_back(id);
  return ids;
}

PluginInfo PluginManager::pluginInfo(const QString& id) const
{
  PluginInfo info;
  if (auto iter = _impl->available.find(id); iter != _impl->available.end()) {
    info = iter->second;
    fill_plugin_info({info.path, _impl->app->activeLang()}, info);
  }
  info.enabled = _impl->loaded.contains(id);
  return info;
}

void PluginManager::enumeratePlugins()
{
  _impl->available.clear();
  _impl->enumerate();
}

void PluginManager::loadPlugin(const QString& id)
{
  _impl->load(id);
}

void PluginManager::unloadPlugin(const QString& id)
{
  _impl->unload(id);
}

void PluginManager::configurePlugin(const QString& id)
{
  _impl->configure(id);
}

#include "plugin_manager.moc"
