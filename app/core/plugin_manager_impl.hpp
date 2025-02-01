// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtCore/QObject>

#include <memory>

#include <QtCore/QString>
#include <QtCore/QVariantHash>
#include <QtCore/QVector>
#include <QtGui/QIcon>

#include "common/instance_options.hpp"
#include "common/instance_options_hash.hpp"

class ClockApplication;
class PluginHandleImpl;

class QWidget;

// plugin is loaded (as library) while this object is held
// so, whatever resources (like icon and translations) are available
// object can be freely copied
// plugin should not be initialized until it is required
// no methods should be called on invalid handle
class PluginHandle
{
public:
  PluginHandle() = default;   // constructs invalid handle
  explicit PluginHandle(std::shared_ptr<PluginHandleImpl> impl);
  ~PluginHandle();

  PluginHandle(const PluginHandle& ) = default;
  PluginHandle(PluginHandle&& ) = default;

  PluginHandle& operator =(const PluginHandle& ) = default;
  PluginHandle& operator =(PluginHandle&& ) = default;

  operator bool() const noexcept { return !!_impl; }

  bool isActive() const;
  bool isConfigurable() const;

  QString id() const;
  QString fileName() const;

  QString name() const;
  QString description() const;

  QIcon icon() const;

  QVariantHash metadata() const;

  void setActive(bool act);
  inline void activate() { setActive(true); }
  inline void deactivate() { setActive(false); }

  void configure(QWidget* parent, size_t idx);

private:
  std::shared_ptr<PluginHandleImpl> _impl;
};


class PluginManager : public QObject
{
  Q_OBJECT

public:
  explicit PluginManager(ClockApplication* app, QObject* parent = nullptr);
  ~PluginManager();

  void retranslate(const QString& lang);

  void load(const QStringList& plgs);
  void unload(const QStringList& plgs);
  void unloadAll();

  // returns all available plugins
  // can do enumeration or other heavy operation
  QVector<PluginHandle> plugins();

  // periodic updates, time is in instance's time zone
  void tick(size_t idx, const QDateTime& dt);

public slots:
  void onOptionChanged(size_t i, opt::InstanceOptions o, const QVariant& v);

signals:
  void optionChanged(size_t i, opt::InstanceOptions o, const QVariant& v);

private:
  QVector<std::shared_ptr<PluginHandleImpl>> enumerate();

  friend class PluginHandleImpl;    // to avoid QObject inheritance
  // private interface for handle implementation
  // handles option change requests from plugins
  void changeOption(size_t i, opt::InstanceOptions o, const QVariant& v);
  // moves plugin from loaded to active and vice versa
  void setPluginActive(const QString& id, bool active);
  // may remove given plugin from loaded plugins list
  void maybeUnload(const QString& id);

private:
  ClockApplication* _app;
  QHash<QString, std::shared_ptr<PluginHandleImpl>> _active_plugins;
  // keeps only temporarily loaded plugins, excluding active ones
  QHash<QString, std::shared_ptr<PluginHandleImpl>> _loaded_plugins;
  QHash<size_t, InstanceOptionsHash> _curr_settings;
};
