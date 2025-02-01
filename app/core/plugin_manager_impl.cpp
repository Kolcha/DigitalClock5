// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "plugin_manager_impl.hpp"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QPluginLoader>

#include "config/app_config.hpp"
#include "core/translation.hpp"
#include "gui/clock_window.hpp"
#include "plugin/clock_plugin.hpp"
#include "plugin/plugin_settings.hpp"

#include "clock_application.hpp"

namespace {

class PluginProber
{
public:
  explicit PluginProber(const QString& filename)
    : _loader(filename)
  {
    _loader.load();
  }

  ~PluginProber()
  {
    _loader.unload();
  }

  operator bool()
  {
    return _loader.isLoaded() && qobject_cast<ClockPlugin*>(_loader.instance());
  }

private:
  QPluginLoader _loader;
};

} // namespace

class PluginHandleImpl
{
public:
  PluginHandleImpl(const QString& filename, ClockApplication* app, PluginManager* pm)
    : _loader(filename)
    , _app(app)
    , _pm(pm)
  {
    _loader.load();
    _metadata = _loader.metaData().value("MetaData").toObject().toVariantHash();
    _tr = loadTranslation(id(), app->activeLang());
  }

  virtual ~PluginHandleImpl()
  {
    if (_tr)
      QCoreApplication::removeTranslator(_tr.get());
    _loader.unload();
  }

  void setTranslator(std::unique_ptr<QTranslator> tr)
  {
    if (_tr) QCoreApplication::removeTranslator(_tr.get());
    _tr = std::move(tr);
    if (_tr) QCoreApplication::installTranslator(_tr.get());
  }

  inline bool isActive() const noexcept { return _active; }

  bool isConfigrable() const
  {
    return _metadata.value("configurable", false).toBool();
  }

  QString id() const
  {
    QFileInfo fi(_loader.fileName());
    return _metadata.value("name", fi.baseName()).toString();
  }

  QString fileName() const { return _loader.fileName(); }

  const QVariantHash& metadata() const noexcept { return _metadata; }

  ClockPlugin* plugin()
  {
    return qobject_cast<ClockPlugin*>(_loader.instance());
  }

  void handleDestroyed()
  {
    _pm->maybeUnload(id());
  }

  void setActive(bool act)
  {
    _active = act;
    if (_active && !_initialized)
      initialize();

    const auto act_insts = _app->config()->global()->getActiveInstancesList();
    for (const auto idx : act_insts)
      if (auto inst = plugin()->instance(idx))
        setInstanceActive(inst, idx, act);

    _pm->setPluginActive(id(), act);
  }

  void configure(QWidget* parent, size_t idx)
  {
    if (!_initialized)
      initialize();
    plugin()->configure(parent, idx);
  }

  void update(size_t idx, const QDateTime& dt)
  {
    if (auto inst = plugin()->instance(idx))
      inst->update(dt);
  }

  void handleOptionChange(size_t i, opt::InstanceOptions o, const QVariant& v)
  {
    onOptionChanged(plugin()->instance(i), o, v);
  }

protected:
  void initPluginAccess(ClockPluginInstance* inst, size_t idx) const;
  void setInstanceActive(ClockPluginInstance* inst, size_t idx, bool act) const;

  void onOptionChanged(ClockPluginInstance* inst, opt::InstanceOptions o, const QVariant& v);

private:
  void initSettingsPlugin(ClockPluginInstance* inst, size_t idx) const;
  void deinitSettingsPlugin(ClockPluginInstance* inst, size_t idx) const;

  auto createPrefixedStorage(QString p) const
  {
    auto st = std::make_unique<SettingsStorageImpl>(std::ref(*_app->settings()));
    return std::make_unique<PrefixedSettingsStorage>(std::move(st), std::move(p));
  }

  auto createPluginSettingsStorage() const
  {
    return createPrefixedStorage(QString("plugin_%1").arg(id()));
  }

  auto createPluginStateStorage() const
  {
    return createPrefixedStorage(QString("state/plugin_%1").arg(id()));
  }

  void initialize()
  {
    Q_ASSERT(!_initialized);
    ClockPlugin::Context ctx = {
      .settings = createPluginSettingsStorage(),
      .state = createPluginStateStorage(),
      .active_instances = _app->config()->global()->getActiveInstancesList(),
    };
    plugin()->init(std::move(ctx));
    _initialized = true;
  }

private:
  QPluginLoader _loader;
  ClockApplication* _app;
  PluginManager* _pm;

  std::unique_ptr<QTranslator> _tr;

  QVariantHash _metadata;

  bool _initialized = false;
  bool _active = false;
};


void PluginHandleImpl::initPluginAccess(ClockPluginInstance* inst, size_t idx) const
{
  if (auto wa = dynamic_cast<WidgetAccess*>(inst))
    wa->init(_app->window(idx));

  if (auto sa = dynamic_cast<SkinSystemAccess*>(inst))
    sa->init(_app->getSkinManager());

  if (auto ta = dynamic_cast<TrayIconAccess*>(inst))
    ta->init(_app->getTrayIconManager());
}

void PluginHandleImpl::setInstanceActive(ClockPluginInstance* inst, size_t idx, bool act) const
{
  if (act) {
    initSettingsPlugin(inst, idx);
    initPluginAccess(inst, idx);
    inst->startup();
  } else {
    inst->shutdown();
    deinitSettingsPlugin(inst, idx);
  }
}

void PluginHandleImpl::onOptionChanged(ClockPluginInstance* inst, opt::InstanceOptions o, const QVariant& v)
{
  if (auto sp = qobject_cast<SettingsPluginInstance*>(inst))
    sp->onOptionChanged(o, v);
}

void PluginHandleImpl::initSettingsPlugin(ClockPluginInstance* inst, size_t idx) const
{
  if (auto sp = qobject_cast<SettingsPluginInstance*>(inst)) {
    QObject::connect(sp, &SettingsPluginInstance::optionChanged,
                     [this, idx](auto&&... args) { _pm->changeOption(idx, args...); });
    sp->init(_pm->_curr_settings[idx]);
  }
}

void PluginHandleImpl::deinitSettingsPlugin(ClockPluginInstance* inst, size_t idx) const
{
  if (auto sp = qobject_cast<SettingsPluginInstance*>(inst))
    QObject::disconnect(sp, &SettingsPluginInstance::optionChanged, nullptr, nullptr);
}


PluginHandle::PluginHandle(std::shared_ptr<PluginHandleImpl> impl)
  : _impl(std::move(impl))
{
  Q_ASSERT(_impl);
}

PluginHandle::~PluginHandle()
{
  // notify implementation about handle destruction
  // implementation in turn may remove itself from the loaded list
  // (if it is the only last copy)
  // it asks manager to do this, as it doesn't know about shared pointer
  // and publicly visible handle should not know/share manager
  if (_impl) _impl->handleDestroyed();
}

bool PluginHandle::isActive() const
{
  return _impl->isActive();
}

bool PluginHandle::isConfigurable() const
{
  return _impl->isConfigrable();
}

QString PluginHandle::id() const
{
  return _impl->id();
}

QString PluginHandle::fileName() const
{
  return _impl->fileName();
}

QString PluginHandle::name() const
{
  return _impl->plugin()->name();
}

QString PluginHandle::description() const
{
  return _impl->plugin()->description();
}

QIcon PluginHandle::icon() const
{
  const auto id = this->id();

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

QVariantHash PluginHandle::metadata() const
{
  return _impl->metadata();
}

void PluginHandle::setActive(bool act)
{
  _impl->setActive(act);
}

void PluginHandle::configure(QWidget* parent, size_t idx)
{
  _impl->configure(parent, idx);
}


PluginManager::PluginManager(ClockApplication* app, QObject* parent)
  : QObject(parent)
  , _app(app)
{
  Q_ASSERT(_app);
}

PluginManager::~PluginManager()
{
  unloadAll();
}

void PluginManager::retranslate(const QString& lang)
{
  for (auto iter = _active_plugins.begin(); iter != _active_plugins.end(); ++iter)
    iter.value()->setTranslator(::loadTranslation(iter.value()->id(), lang));
  for (auto iter = _loaded_plugins.begin(); iter != _loaded_plugins.end(); ++iter)
    iter.value()->setTranslator(::loadTranslation(iter.value()->id(), lang));
}

void PluginManager::load(const QStringList& plgs)
{
  const auto avail_plgs = enumerate();
  for (const auto& id : plgs) {
    for (const auto& p : avail_plgs) {
      if (p->id() == id) {
        // look into already loaded plugins first
        if (auto iter = _loaded_plugins.find(id); iter != _loaded_plugins.end()) {
          (*iter)->setActive(true);
          break;
        }
        auto& ph = _active_plugins[id];
        if (ph) break;  // plugin already loaded
        ph = p;
        ph->setActive(true);
      }
    }
  }
}

void PluginManager::unload(const QStringList& plgs)
{
  for (const auto& id : plgs) {
    if (auto p = _active_plugins.value(id); p)
      p->setActive(false);
  }
}

void PluginManager::unloadAll()
{
  unload(_active_plugins.keys());
}

QVector<PluginHandle> PluginManager::plugins()
{
  QVector<PluginHandle> res_plgs;

  const auto plgs = enumerate();
  for (const auto& p : plgs) {
    if (auto ap = _active_plugins.value(p->id())) {
      res_plgs.emplace_back(std::move(ap));
    } else {
      auto& lp = _loaded_plugins[p->id()];
      if (!lp) lp = p;
      res_plgs.emplace_back(lp);
    }
  }

  Q_ASSERT(res_plgs.size() == plgs.size());
  return res_plgs;
}

void PluginManager::tick(size_t idx, const QDateTime& dt)
{
  for (auto iter = _active_plugins.begin(); iter != _active_plugins.end(); ++iter)
    iter.value()->update(idx, dt);
}

void PluginManager::onOptionChanged(size_t i, opt::InstanceOptions o, const QVariant& v)
{
  for (auto iter = _active_plugins.begin(); iter != _active_plugins.end(); ++iter)
    iter.value()->handleOptionChange(i, o, v);
  // record current settings
  _curr_settings[i][o] = v;
}

QVector<std::shared_ptr<PluginHandleImpl> > PluginManager::enumerate()
{
  QVector<std::shared_ptr<PluginHandleImpl>> avail_plgs;

  QDir d(QCoreApplication::applicationDirPath());
#ifdef Q_OS_MACOS
  if (!d.cd("../PlugIns")) return avail_plgs;
#else
  if (!d.cd("plugins")) return avail_plgs;
#endif
  const auto files = d.entryList(QDir::Files);
  for (const auto& f : files) {
    const auto fn = d.absoluteFilePath(f);
    if (PluginProber prober(fn); prober) {
      avail_plgs.push_back(std::make_shared<PluginHandleImpl>(fn, _app, this));
    }
  }
  return avail_plgs;
}

void PluginManager::changeOption(size_t i, opt::InstanceOptions o, const QVariant& v)
{
  const auto gcfg = _app->config()->global();
  if (gcfg->getOptionsSharing() && i != gcfg->getRefInstance())
    return;
  emit optionChanged(i, o, v);
}

void PluginManager::setPluginActive(const QString& id, bool active)
{
  if (active && _active_plugins.contains(id)) {
    Q_ASSERT(!_loaded_plugins.contains(id));
    return;
  }

  if (active) {
    auto p = _loaded_plugins.take(id);
    Q_ASSERT(p);
    Q_ASSERT(!_active_plugins.contains(id));
    _active_plugins[id] = std::move(p);
  } else {
    auto p = _active_plugins.take(id);
    Q_ASSERT(p);
    Q_ASSERT(!_loaded_plugins.contains(id));
    _loaded_plugins[id] = std::move(p);
  }

  if (!active && _loaded_plugins.contains(id)) {
    Q_ASSERT(!_active_plugins.contains(id));
    maybeUnload(id);
  }
}

void PluginManager::maybeUnload(const QString& id)
{
  Q_ASSERT(_loaded_plugins.contains(id) || _active_plugins.contains(id));
  auto iter = _loaded_plugins.find(id);
  // at "the last iteration" 2 copies of shared pointer exist:
  // one in the container and another one in the handle initiated this call
  if (iter != _loaded_plugins.end() && iter->use_count() == 2)
    _loaded_plugins.erase(iter);
}
