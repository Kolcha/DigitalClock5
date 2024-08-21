/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QObject>

#include "plugin_info.hpp"

// forward-declare Application here to avoid circular dependencies
class Application;

// PluginManager is an "itegral part" of application
// rather than a "standalone reusable component"
// it requires access to different application components
// that may be need for plugin initialization or installation/use
// so, it is not a big deal that it depends on the Application class
class PluginManager : public QObject
{
  Q_OBJECT

public:
  explicit PluginManager(Application* app, QObject* parent = nullptr);
  ~PluginManager();

  // should be called when app initialization is almost done
  void init();

  // returns list of ids that can be used in other functions
  QStringList availablePlugins() const;

  PluginInfo pluginInfo(const QString& id) const;

public slots:
  void loadPlugin(const QString& id);
  void unloadPlugin(const QString& id);

  // opens plugin's configuration dialog
  void configurePlugin(const QString& id);

private:
  // implementation may be very heavy and have many dependencies
  // so, hide it, just to avoid exposure of these dependencies
  // to any other "integral parts" of the application
  struct Impl;
  std::unique_ptr<Impl> _impl;
};
