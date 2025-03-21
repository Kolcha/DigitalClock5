// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtGui/QFont>

#include "common/instance_options.hpp"
#include "config/config_base.hpp"
#include "config/variant_converters.hpp"
#include "renderer/renderer.hpp"

#include "global_options.hpp"

#define GLOBAL_OPTION(type, name)                             \
  void set##name(const type& v) { setValue(opt::name, v); }   \
  type get##name() const { return value<type>(opt::name); }

class GlobalConfig final : public QObject, public ConfigBase
{
  Q_OBJECT

public:
  explicit GlobalConfig(std::unique_ptr<SettingsStorage> st, QObject* parent = nullptr);

  QVector<size_t> getActiveInstancesList() const;
  void setActiveInstancesList(QVector<size_t> l);

  GLOBAL_OPTION(QString, Locale)

  GLOBAL_OPTION(quint8, ActiveInstances)
  GLOBAL_OPTION(size_t, RefInstance)
  GLOBAL_OPTION(bool, OptionsSharing)

  GLOBAL_OPTION(QStringList, Plugins)

  GLOBAL_OPTION(bool, StayOnTop)
  GLOBAL_OPTION(bool, FullscreenDetect)
  GLOBAL_OPTION(bool, TransparentForMouse)

  GLOBAL_OPTION(bool, SnapToEdge)
  GLOBAL_OPTION(int, SnapThreshold)
  GLOBAL_OPTION(bool, PreventOutOfScreen)

  GLOBAL_OPTION(bool, ChangeCursorOnHover)

  GLOBAL_OPTION(bool, TransparentOnHover)
  GLOBAL_OPTION(int, OpacityOnHover)

  GLOBAL_OPTION(opt::TrayIconAction, TrayIconSingleClickAction)
  GLOBAL_OPTION(opt::TrayIconAction, TrayIconDoubleClickAction)

  GLOBAL_OPTION(bool, CheckForUpdates)
  GLOBAL_OPTION(bool, CheckForBetaVersion)
  GLOBAL_OPTION(int, UpdatePeriodDays)

  GLOBAL_OPTION(int, SkinBaseSize)

public slots:
  inline void commit() { storage()->commit(); }
  inline void discard() { storage()->discard(); }

private:
  static QString key(opt::GlobalOptions o);
  static QVariant def_value(opt::GlobalOptions o);

  template<typename T>
  void setValue(opt::GlobalOptions opt, const T& val)
  {
    ConfigBase::setValue(key(opt), val);
  }

  template<typename T>
  T value(opt::GlobalOptions opt) const
  {
    const auto def = def_value(opt);
    return ConfigBase::value(key(opt), def.value<T>());
  }
};

#undef GLOBAL_OPTION

#define INSTANCE_OPTION(type, name)                           \
  void set##name(const type& v) { setValue(opt::name, v); }   \
  type get##name() const { return value<type>(opt::name); }

class AppConfig;

class InstanceConfig final : public QObject, public ConfigBase
{
  Q_OBJECT

public:
  InstanceConfig(std::unique_ptr<SettingsStorage> st, AppConfig& cfg, QObject* parent = nullptr);

  bool isSharedOption(opt::InstanceOptions opt) const { return !_unique_opts.contains(opt); }

  INSTANCE_OPTION(bool, UseSkin)
  INSTANCE_OPTION(QString, Skin)
  INSTANCE_OPTION(QFont, Font)

  INSTANCE_OPTION(int, Scaling)

  INSTANCE_OPTION(int, Opacity)

  INSTANCE_OPTION(tx::TextureType, TextureType)
  INSTANCE_OPTION(LinesRenderer::CustomizationType, TextureCustomization)
  INSTANCE_OPTION(QColor, TextureColor)
  INSTANCE_OPTION(bool, TextureColorUseTheme)
  INSTANCE_OPTION(QGradient, TextureGradient)
  INSTANCE_OPTION(QString, TexturePattern)
  INSTANCE_OPTION(bool, TexturePatternTile)

  INSTANCE_OPTION(tx::TextureType, BackgroundType)
  INSTANCE_OPTION(LinesRenderer::CustomizationType, BackgroundCustomization)
  INSTANCE_OPTION(QColor, BackgroundColor)
  INSTANCE_OPTION(bool, BackgroundColorUseTheme)
  INSTANCE_OPTION(QGradient, BackgroundGradient)
  INSTANCE_OPTION(QString, BackgroundPattern)
  INSTANCE_OPTION(bool, BackgroundPatternTile)

  INSTANCE_OPTION(bool, ApplyBackgroundToWindow)

  INSTANCE_OPTION(bool, TruePerCharRendering)

  INSTANCE_OPTION(bool, ColorizationEnabled)
  INSTANCE_OPTION(QColor, ColorizationColor)
  INSTANCE_OPTION(int, ColorizationStrength)

  INSTANCE_OPTION(int, LayoutSpacing)
  INSTANCE_OPTION(int, CharSpacing)
  INSTANCE_OPTION(int, LineSpacing)
  INSTANCE_OPTION(bool, RespectLineSpacing)

  INSTANCE_OPTION(QMarginsF, CharMargins)
  INSTANCE_OPTION(QMargins, WidgetMargins)
  INSTANCE_OPTION(QMargins, LayoutMargins)

  INSTANCE_OPTION(QString, TimeFormat)
  INSTANCE_OPTION(QString, LayoutConfig)

  INSTANCE_OPTION(bool, SmallerSeconds)
  INSTANCE_OPTION(int, SecondsSize)

  INSTANCE_OPTION(bool, ShowLocalTime)
  INSTANCE_OPTION(QTimeZone, TimeZone)

  INSTANCE_OPTION(bool, FlashingSeparator)
  INSTANCE_OPTION(bool, UseCustomSeparators)
  INSTANCE_OPTION(QString, CustomSeparators)

  INSTANCE_OPTION(bool, HideClockWidget)

public slots:
  void commit();
  void discard();

private:
  static QString key(opt::InstanceOptions o);
  static QVariant def_value(opt::InstanceOptions o);

  template<typename T>
  void setValue(opt::InstanceOptions opt, const T& val)
  {
    if (shouldRedirect(opt))
      targetInstance()->setValue(opt, val);
    else
      ConfigBase::setValue(key(opt), val);
  }

  template<typename T>
  T value(opt::InstanceOptions opt) const
  {
    if (shouldRedirect(opt))
      return targetInstance()->value<T>(opt);
    const auto def = def_value(opt);
    return ConfigBase::value(key(opt), def.value<T>());
  }

  bool shouldRedirect(opt::InstanceOptions o) const;
  InstanceConfig* targetInstance() const;

private:
  AppConfig& _cfg;
  QSet<opt::InstanceOptions> _unique_opts;
};

#undef INSTANCE_OPTION


class AppConfig final : public QObject
{
  Q_OBJECT

public:
  explicit AppConfig(SettingsBackend& st, QObject* parent = nullptr);

  // Qt style: const function returns non-const pointer
  GlobalConfig* global() const noexcept { return _global.get(); }
  InstanceConfig* instance(size_t i) const noexcept;

  void addInstance(size_t i);
  void removeInstance(size_t i);

  // no plugin configs here!
  // they (and this config too) just
  // share the same settings storage

public slots:
  // all or nothing!
  void commit();
  void discard();

private:
  std::unique_ptr<InstanceConfig> createInstance(size_t i);

private:
  SettingsBackend& _st;
  std::unique_ptr<GlobalConfig> _global;
  std::map<size_t, std::unique_ptr<InstanceConfig>> _instances;
};
