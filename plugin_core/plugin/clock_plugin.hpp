// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "plugin_core_global.hpp"

#include <QtCore/QObject>

#include <memory>

#include "common/instance_options.hpp"
#include "common/instance_options_hash.hpp"
#include "common/settings.hpp"

// --------------------------------------
// plugin instance interfaces
// --------------------------------------

// generic clock plugin instance interface
class PLUGIN_CORE_EXPORT ClockPluginInstance : public QObject
{
  Q_OBJECT

public:
  explicit ClockPluginInstance(QObject* parent = nullptr) : QObject(parent) {}

public slots:
  virtual void startup() = 0;
  virtual void shutdown() = 0;

  // called every 0.5 sec
  // connected to the main clock's timer
  // for multi-instance plugins given time is in clock widget timezone,
  // for single-instance plugins timezone is not defined and may vary
  virtual void update(const QDateTime& dt) = 0;
};


class PLUGIN_CORE_EXPORT SettingsPluginInstance : public ClockPluginInstance
{
  Q_OBJECT

public:
  using ClockPluginInstance::ClockPluginInstance;

  // initializes plugin with current instace settings
  virtual void init(const InstanceOptionsHash& settings) = 0;

public slots:
  virtual void onOptionChanged(opt::InstanceOptions o, const QVariant& v) = 0;

signals:
  void optionChanged(opt::InstanceOptions o, const QVariant& v);
};


// --------------------------------------
// "access" interfaces
// provide access to the different clock components
// all these interfaces have only one init() method, just with different parameters
// plugin instance may inherit multiple interfaces to get everything required
// the order of init() calls in unspecified and plugin should not rely on it
// all init() calls are called before PluginInstance::startup()
// --------------------------------------

// grants plugin access to the clock widget (top-level)
// plugin may add widget(s) to it or change its position
// it is guaranteed that this widget has QGridLayout
class PLUGIN_CORE_EXPORT WidgetAccess
{
public:
  virtual ~WidgetAccess() = default;

  // it is guaranteed that pointer is never NULL,
  // and it outlives PluginInstance::shutdown()
  // plugin should not delete this pointer
  virtual void init(QWidget* w) = 0;
};


class SkinManager;

// grants plugin access to the skin system
class PLUGIN_CORE_EXPORT SkinSystemAccess
{
public:
  virtual ~SkinSystemAccess() = default;

  // it is guaranteed that pointer is never NULL,
  // and it outlives PluginInstance::shutdown()
  // plugin should not delete this pointer
  virtual void init(const SkinManager* sm) = 0;
};


class TrayIconManager;

// grants plugin access to the tray icon
class PLUGIN_CORE_EXPORT TrayIconAccess
{
public:
  virtual ~TrayIconAccess() = default;

  // it is guaranteed that pointer is never NULL,
  // and it outlives PluginInstance::shutdown()
  // plugin should not delete this pointer
  virtual void init(TrayIconManager* sm) = 0;
};


// --------------------------------------
// Plugin interface
// --------------------------------------

// base interface for all clock plugins
// this plugin interface acts as factory for plugin instances,
// implementation should not contain any logic except required
// for the plugin instance initialization and maintenance,
// plugin-specific logic should be in plugin instance instead
class PLUGIN_CORE_EXPORT ClockPlugin : public QObject
{
  Q_OBJECT

public:
  explicit ClockPlugin(QObject* parent = nullptr) : QObject(parent) {}

  struct Context {
    // "root" storage for plugin settings,
    // plugin is allowed to write here directly without any prefixes,
    // or create whatever its own structure instead
    std::unique_ptr<SettingsStorage> settings;
    // the same as for settings above, but dedicated for state variables
    std::unique_ptr<SettingsStorage> state;
    // list of currently active clock instances (windows)
    // the same identifiers as in this list later passed to instance()
    QVector<size_t> active_instances;
  };

  // generic initialization function
  // called before anything else (the very first call to the plugin)
  // app may or may not set fields in the Context depending on plugin properties
  // plugin should avoid any heavy initialization in constructor,
  // and do it in this function instead
  // it is also called before configure()
  virtual void init(Context&& ctx) = 0;

  // provides human-readable plugin name, possibly translatable
  virtual QString name() const = 0;
  // provides plugin short description, possibly translatable
  virtual QString description() const = 0;

  // creates (if required) and returns the only plugin instance
  // returned pointer should not be deleted by the user,
  // plugin should manage its lifetime
  // should do only creation and basic instance initialization
  // (if required) using the data passed to init() previously
  // subsequent calls with the same argument must return the same pointer
  // raw pointer is returned for convenient usage with qobject_cast()
  // idx - instance id, plugin should not try to interpret it,
  //       just use as an opaque unique identifier,
  //       only the caller side knows its meaning
  // plugins that don't have "per-instance" meaning should return
  // the pointer only for one identifier, and NULL for the rest
  virtual ClockPluginInstance* instance(size_t idx) = 0;

public slots:
  // virtual void startup() = 0;
  // on this call, plugin should destroy all created instances and
  // whatever else data, to be prepared to the new startup() call
  // (with or without preceding init() call)
  // cleanup should not rely on destructor, as plugin object is kept in memory
  // while library is loaded
  // virtual void shutdown() = 0;

  // open plugin's configuration dialog, optionally for the specific instance
  // it's up to the plugin implementation how to handle idx paramenter
  // it is guaranteed that 'parent' parameter is never NULL
  // and 'idx' is set to the current "active" instance
  // idx should not be interpreted, just used as opaque identifier
  // plugin should support configuration with and without active instances
  virtual void configure(QWidget* parent, size_t idx) = 0;
};

#define ClockPlugin_IId "com.github.Kolcha.DigitalClock5.ClockPlugin/1.0"
Q_DECLARE_INTERFACE(ClockPlugin, ClockPlugin_IId)
