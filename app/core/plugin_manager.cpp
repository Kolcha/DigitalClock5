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
#include "gui/plugin_settings_dialog.hpp"

class PluginHandle {
public:
  explicit PluginHandle(const QString& filename);
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
  std::vector<std::unique_ptr<ClockPluginBase>> _instances;
};

PluginHandle::PluginHandle(const QString& filename)
    : _loader(std::make_unique<QPluginLoader>(filename))
{
  _loader->load();
}

PluginHandle::~PluginHandle()
{
  if (_loader)
    _loader->unload();
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
    , _handle(std::make_shared<PluginHandle>(p))
    , _was_loaded(false)
{
  _handle->createInstances(app->config().global().getNumInstances());
}

void ConfigurePluginHandle::openDialog()
{
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
}

void ConfigurePluginHandle::onAccepted()
{
  for (size_t i = 0; i < _handle->instances().size(); i++)
    _app->config().plugin(_handle->id()).config(i).commit();
}

void ConfigurePluginHandle::onRejected()
{
  for (size_t i = 0; i < _handle->instances().size(); i++)
    _app->config().plugin(_handle->id()).config(i).discard();

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

  std::vector<std::unique_ptr<ChangeRetransmitter>> cfg_sinks;

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
    PluginHandle handle(d.absoluteFilePath(f));
    if (auto iface = handle.plugin()) {
      auto metadata = handle.metadata();
      auto id = handle.id();
      auto& info = available[id];
      info.path = handle.file();
      info.id = id;
      info.title = iface->title();
      info.description = iface->description();
      info.configurable = metadata.value("configurable", false).toBool();
      info.multiinstance = iface->perClockInstance();
      info.metadata = std::move(metadata);
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

  auto& handle = loaded.emplace(id, iter->second.path).first->second;
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

  // save state
  if (available[id].configurable) {
    for (size_t i = 0; i < iter->second.instances().size(); i++) {
      if (auto cp = dynamic_cast<ConfigurablePlugin*>(iter->second.instance(i)))
        cp->saveState(app->config().plugin(id).state(i));
    }
  }

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
      cp->loadState(plg_cfg.state(i));
      cp->initSettings(plg_cfg.config(i));
    }
    // provide skin settings
    if (auto sa = dynamic_cast<SkinAccessExtension*>(h.instance(i))) {
      auto wnd = app->window(i)->clock();
      sa->initSkin(wnd->skin());    // TODO: track skin change
      SkinAccessExtension::AppearanceSettings as;
      as[cs::Texture] = wnd->texture();
      as[cs::TextureStretch] = wnd->textureStretch();
      as[cs::TexturePerCharacter] = wnd->texturePerChar();
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
    }
    // always connect tick()
    QObject::connect(&timer, &QTimer::timeout, h.instance(i), &ClockPluginBase::tick);
    // settings plugin interface
    if (auto sp = dynamic_cast<SettingsPlugin*>(h.instance(i))) {
      if (mdata.value("settings_listener", false).toBool()) {
        auto idx = h.plugin()->perClockInstance() ? i : 0;
        QObject::connect(cfg_sinks[idx].get(), &ChangeRetransmitter::optionChanged,
                         sp, &SettingsPlugin::onOptionChanged);
      }
      if (mdata.value("settings_notifier", false).toBool()) {
        auto idx = h.plugin()->perClockInstance() ? i : 0;
        QObject::connect(sp, &SettingsPlugin::optionChanged,
                         cfg_sinks[idx].get(), &ChangeRetransmitter::optionChanged);
      }
    }
  }
}


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

  for (int i = 0; i < _impl->app->config().global().getNumInstances(); i++)
    _impl->cfg_sinks.push_back(std::make_unique<ChangeRetransmitter>());

  // settings sync with the clock
  bool same_appearance = !_impl->app->config().global().getAppearancePerInstance();
  if (same_appearance) {
    // clock 0 -> all sinks
    for (const auto& s : _impl->cfg_sinks) {
      connect(_impl->app->settingsTransmitter(0), &SettingsChangeTransmitter::optionChanged,
              s.get(), &ChangeRetransmitter::optionChanged);
    }
    // sink 0 -> all clocks
    for (const auto& l : _impl->app->settingsListeners()) {
      connect(_impl->cfg_sinks[0].get(), &ChangeRetransmitter::optionChanged,
              l.get(), &SettingsChangeListener::onOptionChanged);
    }
  } else {
    // one-to-one
    for (size_t i = 0; i < _impl->cfg_sinks.size(); i++) {
      auto cl = _impl->app->settingsListener(i);
      auto ct = _impl->app->settingsTransmitter(i);
      auto ss = _impl->cfg_sinks[i].get();
      connect(ss, &ChangeRetransmitter::optionChanged, cl, &SettingsChangeListener::onOptionChanged);
      connect(ct, &SettingsChangeTransmitter::optionChanged, ss, &ChangeRetransmitter::optionChanged);
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
  if (auto iter = _impl->available.find(id); iter != _impl->available.end())
    info = iter->second;
  info.enabled = _impl->loaded.contains(id);
  return info;
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
