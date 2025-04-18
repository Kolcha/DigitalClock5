// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "plugin_core_global.hpp"

#include <QtCore/QObject>

#include <QtCore/QHash>
#include <QtCore/QVariant>
#include <QtGui/QFont>
#include <QtGui/QGradient>

#include "common/instance_options.hpp"
#include "config/config_base.hpp"
#include "config/variant_converters.hpp"
#include "plugin/plugin_settings.hpp"
#include "renderer/renderer.hpp"

namespace plugin::text {
namespace impl {
class SharedOptionRouter;
} // namespace impl

namespace wpo {

Q_NAMESPACE_EXPORT(PLUGIN_CORE_EXPORT)

enum InstanceOptions {
  WidgetPosition,

  AutoscaleEnabled,
  PercentOfClockSize,
  WidgetAlignment,

  FollowClockAppearance,
  UseClockSkin,
};

Q_ENUM_NS(InstanceOptions)

} // namespace wpo

#define PLUGIN_CONFIG_OPTION_IMPL(type, ns, name)                                       \
  virtual type get##name##Default() const { return def_value(ns::name).value<type>(); } \
  void set##name(const type& v) { setValue(ns::name, v); }                              \
  type get##name() const { return value(ns::name, get##name##Default()); }

#define PLUGIN_CONFIG_OPTION(type, name)  PLUGIN_CONFIG_OPTION_IMPL(type, wpo, name)
#define SHARED_CONFIG_OPTION(type, name)  PLUGIN_CONFIG_OPTION_IMPL(type, opt, name)

enum WidgetPosition { Bottom, Right };

class PLUGIN_CORE_EXPORT PluginInstanceConfig : public QObject, public ConfigBase
{
  Q_OBJECT

public:
  explicit PluginInstanceConfig(std::unique_ptr<SettingsStorage> st, QObject* parent = nullptr)
    : QObject(parent)
    , ConfigBase(std::move(st))
  {}

  PLUGIN_CONFIG_OPTION(WidgetPosition, WidgetPosition)

  PLUGIN_CONFIG_OPTION(bool, AutoscaleEnabled)
  PLUGIN_CONFIG_OPTION(int, PercentOfClockSize)
  PLUGIN_CONFIG_OPTION(Qt::Alignment, WidgetAlignment)

  PLUGIN_CONFIG_OPTION(bool, FollowClockAppearance)
  PLUGIN_CONFIG_OPTION(bool, UseClockSkin)

  SHARED_CONFIG_OPTION(bool, UseSkin)
  SHARED_CONFIG_OPTION(QString, Skin)
  SHARED_CONFIG_OPTION(QFont, Font)

  SHARED_CONFIG_OPTION(tx::TextureType, TextureType)
  SHARED_CONFIG_OPTION(LinesRenderer::CustomizationType, TextureCustomization)
  SHARED_CONFIG_OPTION(QColor, TextureColor)
  SHARED_CONFIG_OPTION(bool, TextureColorUseTheme)
  SHARED_CONFIG_OPTION(QGradient, TextureGradient)
  SHARED_CONFIG_OPTION(QString, TexturePattern)
  SHARED_CONFIG_OPTION(bool, TexturePatternTile)

  SHARED_CONFIG_OPTION(tx::TextureType, BackgroundType)
  SHARED_CONFIG_OPTION(LinesRenderer::CustomizationType, BackgroundCustomization)
  SHARED_CONFIG_OPTION(QColor, BackgroundColor)
  SHARED_CONFIG_OPTION(bool, BackgroundColorUseTheme)
  SHARED_CONFIG_OPTION(QGradient, BackgroundGradient)
  SHARED_CONFIG_OPTION(QString, BackgroundPattern)
  SHARED_CONFIG_OPTION(bool, BackgroundPatternTile)

  SHARED_CONFIG_OPTION(bool, TruePerCharRendering)

  SHARED_CONFIG_OPTION(int, CharSpacing)
  SHARED_CONFIG_OPTION(int, LineSpacing)
  SHARED_CONFIG_OPTION(bool, RespectLineSpacing)

  SHARED_CONFIG_OPTION(QMarginsF, CharMargins)
  SHARED_CONFIG_OPTION(QMarginsF, WidgetMargins)

  SHARED_CONFIG_OPTION(QString, LayoutConfig)

public slots:
  void commit() { storage()->commit(); }
  void discard() { storage()->discard(); }

private:
  static QString key(wpo::InstanceOptions o);
  static QString key(opt::InstanceOptions o);
  static QVariant def_value(wpo::InstanceOptions o);
  static QVariant def_value(opt::InstanceOptions o);

  static bool isSkinOption(opt::InstanceOptions o);

  template<typename Key, typename T>
  void setValue(Key opt, const T& val)
  {
    ConfigBase::setValue(key(opt), val);
  }

  template<typename Key, typename T>
  T value(Key opt, const T& def) const
  {
    return ConfigBase::value(key(opt), def);
  }

  friend class impl::SharedOptionRouter;
};

#undef SHARED_CONFIG_OPTION
#undef PLUGIN_CONFIG_OPTION
#undef PLUGIN_CONFIG_OPTION_IMPL


class PLUGIN_CORE_EXPORT PluginConfig : public QObject
{
  Q_OBJECT

public:
  explicit PluginConfig(std::unique_ptr<PluginSettingsBackend> st, QObject* parent = nullptr);

  PluginInstanceConfig* instance(size_t i);

public slots:
  // all or nothing!
  void commit();
  void discard();

protected:
  // plugin implementation should override this factory method creating
  // the specific plugin config inherited from PluginInstanceConfig
  // user should cast instance() to that specific type when needed
  // default implementation creates "generic" config, see PluginInstanceConfig
  virtual std::unique_ptr<PluginInstanceConfig> createInstanceImpl(std::unique_ptr<SettingsStorage> st) const;

private:
  std::unique_ptr<PluginInstanceConfig> createInstance(size_t i);

private:
  std::unique_ptr<PluginSettingsBackend> _st;
  std::map<size_t, std::unique_ptr<PluginInstanceConfig>> _instances;
};

} // namespace plugin::text
