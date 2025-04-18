// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "plugin_config.hpp"

#include "utilities/texture.hpp"

namespace plugin::text {

QString PluginInstanceConfig::key(wpo::InstanceOptions o)
{
  switch (o) {
    case wpo::WidgetPosition:           return "widget/position";

    case wpo::AutoscaleEnabled:         return "widget/autoscale_enabled";
    case wpo::PercentOfClockSize:       return "widget/percent_of_clock_size";
    case wpo::WidgetAlignment:          return "widget/alignment";

    case wpo::FollowClockAppearance:    return "widget/follow_clock_appearance";
    case wpo::UseClockSkin:             return "widget/use_clock_skin";
  }
  Q_ASSERT(false);
  return {};
}

QString PluginInstanceConfig::key(opt::InstanceOptions o)
{
  switch (o) {
    case opt::UseSkin:                  return "appearance/use_skin";
    case opt::Skin:                     return "appearance/skin";
    case opt::Font:                     return "appearance/font";

    case opt::Scaling:                  return "appearance/scaling";

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

    case opt::TruePerCharRendering:     return "appearance/true_per_char_rendering";

    case opt::CharSpacing:              return "geometry/char_spacing";
    case opt::LineSpacing:              return "geometry/line_spacing";
    case opt::RespectLineSpacing:       return "geometry/respect_line_spacing";

    case opt::CharMargins:              return "geometry/char_margins";
    case opt::WidgetMargins:            return "geometry/widget_margins";

    case opt::LayoutConfig:             return "geometry/layout_config_string";

    default:                            break;  // just to make clang happy...
  }
  Q_ASSERT(false);
  return {};
}

QVariant PluginInstanceConfig::def_value(wpo::InstanceOptions o)
{
  switch (o) {
    case wpo::WidgetPosition:           return QVariant::fromValue(Bottom);

    case wpo::AutoscaleEnabled:         return true;
    case wpo::PercentOfClockSize:       return 100;
    case wpo::WidgetAlignment:          return QVariant::fromValue(Qt::AlignCenter);

    case wpo::FollowClockAppearance:    return true;
    case wpo::UseClockSkin:             return false;
  }
  Q_ASSERT(false);
  return {};
}

QVariant PluginInstanceConfig::def_value(opt::InstanceOptions o)
{
  switch (o) {
    case opt::UseSkin:                  return false;
    case opt::Skin:                     return QString("electronic");
    case opt::Font:                     return QFont("Comic Sans MS", 24);

    case opt::Scaling:                  return 100;

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

    case opt::TruePerCharRendering:     return false;

    case opt::CharSpacing:              return 0;
    case opt::LineSpacing:              return 2;
    case opt::RespectLineSpacing:       return false;

    case opt::CharMargins:              return QVariant::fromValue(QMarginsF());
    case opt::WidgetMargins:            return QVariant::fromValue(QMarginsF());

    case opt::LayoutConfig:             return QString();

    default:                            break;  // just to make clang happy...
  }
  Q_ASSERT(false);
  return {};
}

bool PluginInstanceConfig::isSkinOption(opt::InstanceOptions o)
{
  static const QSet<opt::InstanceOptions> skin_opts = {
    opt::UseSkin,
    opt::Skin,
    opt::Font,
  };
  return skin_opts.contains(o);
}


PluginConfig::PluginConfig(std::unique_ptr<PluginSettingsBackend> st, QObject* parent)
  : QObject(parent)
  , _st(std::move(st))
{}

PluginInstanceConfig* PluginConfig::instance(size_t i)
{
  auto& inst = _instances[i];
  if (!inst)
    inst = createInstance(i);
  return inst.get();
}

void PluginConfig::commit()
{
  for (const auto& [_, i] : _instances)
    i->commit();
}

void PluginConfig::discard()
{
  for (const auto& [_, i] : _instances)
    i->discard();
}

std::unique_ptr<PluginInstanceConfig> PluginConfig::createInstanceImpl(std::unique_ptr<SettingsStorage> st) const
{
  return std::make_unique<PluginInstanceConfig>(std::move(st));
}

std::unique_ptr<PluginInstanceConfig> PluginConfig::createInstance(size_t i)
{
  auto st = std::make_unique<PluginSettingsStorage>(std::ref(*_st));
  auto pst = std::make_unique<PrefixedSettingsStorage>(std::move(st), instance_prefix(i));
  return createInstanceImpl(std::move(pst));
}

} // namespace plugin::text
