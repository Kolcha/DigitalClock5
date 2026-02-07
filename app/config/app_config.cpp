// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app_config.hpp"

#include <QtCore/QMargins>
#include <QtCore/QTimeZone>
#include <QtGui/QColor>
#include <QtGui/QFont>

#include "renderer/renderer.hpp"
#include "utilities/texture.hpp"

GlobalConfig::GlobalConfig(std::unique_ptr<SettingsStorage> st, QObject* parent)
  : QObject(parent)
  , ConfigBase(std::move(st))
{}

QVector<size_t> GlobalConfig::getActiveInstancesList() const
{
  QVector<size_t> l;
  // active instances are saved as bitmask: bit set - instance is active
  const auto act_insts = getActiveInstances();
  l.reserve(8u*sizeof(act_insts));
  Q_ASSERT(act_insts != 0);       // at least one instance must be active
  for (size_t i = 0; i < 8u*sizeof(act_insts); i++)
    if ((act_insts & (1 << i)) != 0)
      l.push_back(i);
  return l;
}

void GlobalConfig::setActiveInstancesList(const QVector<size_t>& l)
{
  quint8 act_insts = 0;
  for (size_t i = 0; i < l.size() && i < 8u*sizeof(act_insts); i++)
    act_insts |= (1 << i);
  Q_ASSERT(act_insts != 0);       // at least one instance must be active
  setActiveInstances(act_insts);
}

QString GlobalConfig::key(opt::GlobalOptions o)
{
  switch (o) {
    case opt::Locale:                   return "app_global/locale";

    case opt::ActiveInstances:          return "app_global/active_instances";
    case opt::RefInstance:              return "app_global/ref_instance";
    case opt::OptionsSharing:           return "app_global/options_sharing";

    case opt::Plugins:                  return "app_global/plugins";

    case opt::StayOnTop:                return "behavior/stay_on_top";
    case opt::FullscreenDetect:         return "behavior/fullscreen_detect";
    case opt::TransparentForMouse:      return "behavior/transparent_for_mouse";

    case opt::SnapToEdge:               return "behavior/snap_to_edge";
    case opt::SnapThreshold:            return "behavior/snap_threshold";
    case opt::PreventOutOfScreen:       return "behavior/prevent_out_of_screen";

    case opt::ChangeCursorOnHover:      return "behavior/change_cursor_on_hover";

    case opt::TransparentOnHover:       return "behavior/transparent_on_hover";
    case opt::OpacityOnHover:           return "behavior/opacity_on_hover";

    case opt::TrayIconSingleClickAction:  return "behavior/tray_icon_single_click_action";
    case opt::TrayIconDoubleClickAction:  return "behavior/tray_icon_double_click_action";

    case opt::CheckForUpdates:          return "updater/check_for_updates";
    case opt::CheckForBetaVersion:      return "updater/check_for_beta";
    case opt::UpdatePeriodDays:         return "updater/update_period_days";

    case opt::SkinBaseSize:             return "app_limits/skin_base_size";

    case opt::UseCustomTrayIconColor:   return "app_global/use_custom_tray_icon_color";
    case opt::CustomTrayIconColor:      return "app_global/custom_tray_icon_color";
  }
  Q_UNREACHABLE();
}

QVariant GlobalConfig::def_value(opt::GlobalOptions o)
{
  switch (o) {
    case opt::Locale:                   return QString("auto");

    case opt::ActiveInstances:          return QVariant::fromValue(static_cast<quint8>(1));
    case opt::RefInstance:              return QVariant::fromValue(static_cast<size_t>(0));
    case opt::OptionsSharing:           return true;

    case opt::Plugins:                  return QStringList();

    case opt::StayOnTop:                return true;
    case opt::FullscreenDetect:         return true;
    case opt::TransparentForMouse:      return false;

    case opt::SnapToEdge:               return true;
    case opt::SnapThreshold:            return 15;
    case opt::PreventOutOfScreen:       return true;

    case opt::ChangeCursorOnHover:      return true;

    case opt::TransparentOnHover:       return false;
    case opt::OpacityOnHover:           return 15;

    case opt::TrayIconSingleClickAction:  return QVariant::fromValue(opt::NoAction);
    case opt::TrayIconDoubleClickAction:  return QVariant::fromValue(opt::OpenSettings);

    case opt::CheckForUpdates:          return true;
    case opt::CheckForBetaVersion:      return true;
    case opt::UpdatePeriodDays:         return 7;

    case opt::SkinBaseSize:             return 96;

    case opt::UseCustomTrayIconColor:   return false;
    case opt::CustomTrayIconColor:      return QColor(112, 96, 240);
  }
  Q_UNREACHABLE();
}


InstanceConfig::InstanceConfig(std::unique_ptr<SettingsStorage> st, AppConfig& cfg, QObject* parent)
  : QObject(parent)
  , ConfigBase(std::move(st))
  , _cfg(cfg)
{
  _unique_opts.insert(opt::ShowLocalTime);
  _unique_opts.insert(opt::TimeZone);
  _unique_opts.insert(opt::HideClockWidget);
}

void InstanceConfig::commit()
{
  if (targetInstance() != this)
    targetInstance()->commit();
  storage()->commit();
}

void InstanceConfig::discard()
{
  if (targetInstance() != this)
    targetInstance()->discard();
  storage()->discard();
}

QString InstanceConfig::key(opt::InstanceOptions o)
{
  switch (o) {
    case opt::UseSkin:                  return "appearance/use_skin";
    case opt::Skin:                     return "appearance/skin";
    case opt::Font:                     return "appearance/font";

    case opt::Scaling:                  return "appearance/scaling";

    case opt::Opacity:                  return "appearance/opacity";

    case opt::TextureType:              return "appearance/texture_type";
    case opt::TextureCustomization:     return "appearance/texture_customization";
    case opt::TextureColor:             return "appearance/texture_color";
    case opt::TextureColorUseTheme:     return "appearance/texture_color_use_theme";
    case opt::TextureGradient:          return "appearance/texture_gradient";
    case opt::TexturePattern:           return "appearance/texture_pattern";
    case opt::TexturePatternTile:       return "appearance/texture_pattern_tile";

    case opt::BackgroundType:           return "appearance/background_type";
    case opt::BackgroundCustomization:  return "appearance/background_customization";
    case opt::BackgroundColor:          return "appearance/background_color";
    case opt::BackgroundColorUseTheme:  return "appearance/background_color_use_theme";
    case opt::BackgroundGradient:       return "appearance/background_gradient";
    case opt::BackgroundPattern:        return "appearance/background_pattern";
    case opt::BackgroundPatternTile:    return "appearance/background_pattern_tile";

    case opt::ApplyBackgroundToWindow:  return "appearance/apply_background_to_window";

    case opt::TruePerCharRendering:     return "appearance/true_per_char_rendering";

    case opt::ColorizationEnabled:      return "appearance/colorization_enabled";
    case opt::ColorizationColor:        return "appearance/colorization_color";
    case opt::ColorizationStrength:     return "appearance/colorization_strength";

    case opt::LayoutSpacing:            return "geometry/layout_spacing";
    case opt::CharSpacing:              return "geometry/char_spacing";
    case opt::LineSpacing:              return "geometry/line_spacing";
    case opt::RespectLineSpacing:       return "geometry/respect_line_spacing";

    case opt::CharMargins:              return "geometry/char_margins";
    case opt::WidgetMargins:            return "geometry/widget_margins";
    case opt::LayoutMargins:            return "geometry/layout_margins";

    case opt::TimeFormat:               return "clock/time_format";
    case opt::LayoutConfig:             return "clock/layout_config_string";

    case opt::SmallerSeconds:           return "clock/smaller_seconds";
    case opt::SecondsSize:              return "clock/seconds_size";

    case opt::ShowLocalTime:            return "clock/show_local_time";
    case opt::TimeZone:                 return "clock/time_zone";

    case opt::FlashingSeparator:        return "clock/flashing_separator";
    case opt::UseCustomSeparators:      return "clock/use_custom_separators";
    case opt::CustomSeparators:         return "clock/custom_separators";

    case opt::HideClockWidget:          return "misc/hide_clock_widget";
  }
  Q_UNREACHABLE();
}

QVariant InstanceConfig::def_value(opt::InstanceOptions o)
{
  switch (o) {
    case opt::UseSkin:                  return true;
    case opt::Skin:                     return QString("electronic");
    case opt::Font:                     return QFont("Comic Sans MS", 72);

    case opt::Scaling:                  return 100;

    case opt::Opacity:                  return 75;

    case opt::TextureType:              return QVariant::fromValue(tx::Color);
    case opt::TextureCustomization:     return QVariant::fromValue(LinesRenderer::PerChar);
    case opt::TextureColor:             return QColor(112, 96, 240);
    case opt::TextureColorUseTheme:     return false;
    case opt::TextureGradient:          return QVariant::fromValue(sample_conical_gradient());
    case opt::TexturePattern:           return QString();
    case opt::TexturePatternTile:       return true;

    case opt::BackgroundType:           return QVariant::fromValue(tx::None);
    case opt::BackgroundCustomization:  return QVariant::fromValue(LinesRenderer::PerLine);
    case opt::BackgroundColor:          return QColor(0, 0, 0, 160);
    case opt::BackgroundColorUseTheme:  return false;
    case opt::BackgroundGradient:       return QVariant::fromValue(sample_linear_gradient());
    case opt::BackgroundPattern:        return QString();
    case opt::BackgroundPatternTile:    return true;

    case opt::ApplyBackgroundToWindow:  return false;

    case opt::TruePerCharRendering:     return false;

    case opt::ColorizationEnabled:      return false;
    case opt::ColorizationColor:        return QColor(85, 0, 255);
    case opt::ColorizationStrength:     return 80;

    case opt::LayoutSpacing:            return 0;
    case opt::CharSpacing:              return 4;
    case opt::LineSpacing:              return 8;
    case opt::RespectLineSpacing:       return false;

    case opt::CharMargins:              return QVariant::fromValue(QMarginsF());
    case opt::WidgetMargins:            return QVariant::fromValue(QMarginsF());
    case opt::LayoutMargins:            return QVariant::fromValue(QMargins());

    case opt::TimeFormat:               return QString("HH:mm");
    case opt::LayoutConfig:             return QString();

    case opt::SmallerSeconds:           return false;
    case opt::SecondsSize:              return 60;

    case opt::ShowLocalTime:            return true;
    case opt::TimeZone:                 return QVariant::fromValue(QTimeZone::systemTimeZone());

    case opt::FlashingSeparator:        return true;
    case opt::UseCustomSeparators:      return false;
    case opt::CustomSeparators:         return QString();

    case opt::HideClockWidget:          return false;
  }
  Q_ASSERT(false);
  return {};
}

bool InstanceConfig::shouldRedirect(opt::InstanceOptions o) const
{
  return _cfg.global()->getOptionsSharing() &&
         !_unique_opts.contains(o) &&
         targetInstance() != this;
}

InstanceConfig* InstanceConfig::targetInstance() const
{
  return _cfg.instance(_cfg.global()->getRefInstance());
}


AppConfig::AppConfig(SettingsBackend& st, QObject* parent)
  : QObject(parent)
  , _st(st)
{
  auto gst = std::make_unique<SettingsStorageImpl>(std::ref(_st));
  _global = std::make_unique<GlobalConfig>(std::move(gst));
  const auto act_insts = _global->getActiveInstancesList();
  for (auto i : act_insts)
    _instances[i] = createInstance(i);
}

InstanceConfig* AppConfig::instance(size_t i) const noexcept
{
  if (auto iter = _instances.find(i); iter != _instances.end())
    return iter->second.get();
  return nullptr;
}

void AppConfig::addInstance(size_t i)
{
  _instances[i] = createInstance(i);
}

void AppConfig::removeInstance(size_t i)
{
  _instances.erase(i);
}

void AppConfig::commit()
{
  _global->commit();
  for (auto& instance : _instances | std::views::values)
    instance->commit();
}

void AppConfig::discard()
{
  _global->discard();
  for (auto& instance : _instances | std::views::values)
    instance->discard();
}

std::unique_ptr<InstanceConfig> AppConfig::createInstance(size_t i)
{
  auto prefix = QString("instance_%1").arg(i);
  auto st = std::make_unique<SettingsStorageImpl>(std::ref(_st));
  auto pst = std::make_unique<PrefixedSettingsStorage>(std::move(st), std::move(prefix));
  return std::make_unique<InstanceConfig>(std::move(pst), *this);
}
