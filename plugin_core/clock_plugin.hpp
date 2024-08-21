/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QObject>

#include "config/settings_storage.hpp"

#include "plugin_core_global.hpp"
#include "shared_config_keys.hpp"

// generic plugin interface
// represents loaded and running plugin instance
// several instances may exist for the same plugin
class PLUGIN_CORE_EXPORT ClockPluginBase : public QObject
{
  Q_OBJECT

public:
  virtual void init() = 0;
  virtual void shutdown() = 0;

  virtual void tick() = 0;
};


// single instance factory object for plugin instances
// represents "plugin itself", provides some basic properties
class PLUGIN_CORE_EXPORT ClockPluginFactory : public QObject
{
  Q_OBJECT

public:
  virtual std::unique_ptr<ClockPluginBase> create() const = 0;

  virtual QString title() const = 0;
  virtual QString description() const = 0;

  virtual bool perClockInstance() const = 0;
};

#define ClockPluginFactory_iid "com.github.Kolcha.DigitalClock5.Clock/1.0"
Q_DECLARE_INTERFACE(ClockPluginFactory, ClockPluginFactory_iid)


// allows plugin to listen to and update shared clock settings
// inherits and extends ClockPluginBase
// should be primary base class for the plugins where such functionality is desired
// additional non QObject-based interfaces can be inherited as required
class PLUGIN_CORE_EXPORT SettingsPlugin : public ClockPluginBase
{
  Q_OBJECT

public slots:
  // called each time one of shared options changed
  // (doesn't matter by the clock or another plugin)
  // plugin must have 'settings_listener' metadata field set to 'true'
  // to subscribe to this updates, disabled by default
  // TODO: connect this slot after init() call
  virtual void onOptionChanged(cs::SharedConfigKeys opt, const QVariant& val) = 0;

signals:
  // should be called when plugin wants to change something
  // plugin must have 'settings_notifier' metadata field set to 'true'
  // to be able to use this, otherwise any notifications are ignored
  // signal can be emitted from init()
  void optionChanged(cs::SharedConfigKeys opt, const QVariant& val);
};


// extension interface for configurable plugins
class PLUGIN_CORE_EXPORT ConfigurablePlugin {
public:
  virtual ~ConfigurablePlugin() = default;

  // allows to save the state, called before shutdown()
  virtual void saveState(StateClient& st) const = 0;
  // allows to load the state, called before init()
  virtual void loadState(const StateClient& st) = 0;

  // allows plugin to init its structures from saved settings
  // plugin should not save this reference for the future use
  // called before init() but after loadState()
  // but also called when settings are discarded
  virtual void initSettings(const SettingsStorageClient& st) = 0;

  // creates confuration widget
  // should use provided settings and state objects for controls initialization
  // lifetime of the returned widget is managed by the calling side,
  // lifetime of the passed references at least the same as widget's,
  // so provided references can (and should) be used inside widget implementation
  // implementation should be "stateless", i.e. rely only on this input data and
  // be able to reconstruct whatever state based only on it
  // caller side takes care about committing or discarding the changes,
  // if changes are discarded, initSettings() is called
  // widget should be considered as a "page" rather than "dialog", i.e. shouldn't
  // contain any "OK"/"Cancel" (or whatever else similar) buttons
  // this function can be called for not started (i.e. no init() calls) plugin
  // implementation should be prepared for the such case
  virtual QWidget* configure(SettingsStorageClient& s, StateClient& t) = 0;
};
