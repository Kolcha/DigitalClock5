// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "clock_application.hpp"

#include <QtDebug>

#include <QtWidgets/QLayout>

#include "config/app_config.hpp"
#include "core/plugin_manager_impl.hpp"
#include "core/update_checker.hpp"
#include "gui/clock_widget.hpp"
#include "gui/clock_window.hpp"
#include "renderer/font_skin.hpp"
#include "utilities/layout_config_parser.hpp"
#include "utilities/texture.hpp"

void ClockApplication::onGlobalOptionChanged(opt::GlobalOptions opt, const QVariant& val)
{
  qDebug() << "global:" << opt << val;
  const auto& gcfg = *_cfg->global();

  switch (opt) {
    case opt::Locale:
      if (val.toString() != _last_lang)
        retranslateUI();
      break;

    case opt::ActiveInstances:
      // runtime change is not yet supported
      break;
    case opt::RefInstance:
      // hidden option, not supposed to be changed
      break;
    case opt::OptionsSharing:
      // runtime change is not yet supported
      break;

    case opt::Plugins:
      // plugins list change is handled explicitly
      // as plugins depend on data which may change during settings loading
      break;

    case opt::StayOnTop:
    case opt::FullscreenDetect:
    case opt::TransparentForMouse:

    case opt::SnapToEdge:
    case opt::SnapThreshold:
    case opt::PreventOutOfScreen:

    case opt::ChangeCursorOnHover:

    case opt::TransparentOnHover:
    case opt::OpacityOnHover:
      for (const auto& [i, w] : _windows) applyWindowOption(i, opt, val);
      break;

    case opt::CheckForUpdates:
      if (val.toBool())
        initUpdater();
      else
        _update_checker.reset();
      break;
    case opt::CheckForBetaVersion:
      if (_update_checker)
        _update_checker->setCheckForBeta(val.toBool());
      break;
    case opt::UpdatePeriodDays:
      // does nothing, value is used directly from config
      break;

    case opt::SkinBaseSize:
      // hidden option, not supposed to be changed
      break;
  }
}

void ClockApplication::onInstanceOptionChanged(size_t i, opt::InstanceOptions opt, const QVariant& val)
{
  qDebug() << "instance:" << opt << val;
  if (_cfg->global()->getOptionsSharing() && _cfg->instance(i)->isSharedOption(opt)) {
    for (const auto& [w, _] : _windows)
      applyInstanceOption(w, opt, val);
  } else {
    applyInstanceOption(i, opt, val);
  }
}

void ClockApplication::applyInstanceOption(size_t idx, opt::InstanceOptions opt, const QVariant& val)
{
  const auto& icfg = *_cfg->instance(idx);
  auto wnd = window(idx);

  switch (opt) {
    case opt::UseSkin:
      val.toBool() ?
         applyInstanceOption(idx, opt::Skin, icfg.getSkin()) :
         applyInstanceOption(idx, opt::Font, icfg.getFont());
      break;
    case opt::Skin:
      wnd->clock()->setSkin(_sm->load(val.toString()));
      break;
    case opt::Font:
      wnd->clock()->setSkin(std::make_unique<FontSkin>(val.value<QFont>()));
      break;

    case opt::Scaling: {
      bool ok = false;
      auto s = val.toInt(&ok) / 100.;
      if (ok)
        wnd->clock()->setScaling(s, s);
      else
        wnd->clock()->setScaling(val.value<ScaleFactor>());
      break;
    }

    case opt::Opacity:
      wnd->setWindowOpacity(val.toInt() / 100.);
      break;

    case opt::TextureType:
      switch (val.value<tx::TextureType>()) {
        case tx::None:
          wnd->clock()->setTextureCustomization(LinesRenderer::None);
          break;
        case tx::Color:
          applyInstanceOption(idx, opt::TextureColorUseTheme, icfg.getTextureColorUseTheme());
          break;
        case tx::Gradient:
          applyInstanceOption(idx, opt::TextureGradient, QVariant::fromValue(icfg.getTextureGradient()));
          break;
        case tx::Pattern:
          applyInstanceOption(idx, opt::TexturePattern, icfg.getTexturePattern());
          break;
      }
      if (val.value<tx::TextureType>() != tx::None)
        wnd->clock()->setTextureCustomization(icfg.getTextureCustomization());
      break;
    case opt::TextureCustomization:
      wnd->clock()->setTextureCustomization(val.value<LinesRenderer::CustomizationType>());
      break;
    case opt::TextureColor:
      wnd->clock()->setTexture(solid_color_texture(val.value<QColor>()));
      break;
    case opt::TextureColorUseTheme:
      applyInstanceOption(idx, opt::TextureColor, val.toBool() ?
                          wnd->palette().color(QPalette::WindowText) :
                          icfg.getTextureColor());
      break;
    case opt::TextureGradient:
      wnd->clock()->setTexture(gradient_texture(val.value<QGradient>()));
      break;
    case opt::TexturePattern:
      wnd->clock()->setTexture(pattern_from_file(val.toString()),
                               icfg.getTexturePatternTile() ? Qt::RepeatTile : Qt::StretchTile);
      break;
    case opt::TexturePatternTile:
      wnd->clock()->setTexture(pattern_from_file(icfg.getTexturePattern()),
                               val.toBool() ? Qt::RepeatTile : Qt::StretchTile);
      break;

    case opt::BackgroundType:
      switch (val.value<tx::TextureType>()) {
        case tx::None:
          wnd->clock()->setBackgroundCustomization(LinesRenderer::None);
          break;
        case tx::Color:
          applyInstanceOption(idx, opt::BackgroundColorUseTheme, icfg.getBackgroundColorUseTheme());
          break;
        case tx::Gradient:
          applyInstanceOption(idx, opt::BackgroundGradient, QVariant::fromValue(icfg.getBackgroundGradient()));
          break;
        case tx::Pattern:
          applyInstanceOption(idx, opt::BackgroundPattern, icfg.getBackgroundPattern());
          break;
      }
      if (val.value<tx::TextureType>() != tx::None)
        wnd->clock()->setBackgroundCustomization(icfg.getBackgroundCustomization());
      break;
    case opt::BackgroundCustomization:
      wnd->clock()->setBackgroundCustomization(val.value<LinesRenderer::CustomizationType>());
      break;
    case opt::BackgroundColor:
      wnd->clock()->setBackground(solid_color_texture(val.value<QColor>()));
      break;
    case opt::BackgroundColorUseTheme:
      applyInstanceOption(idx, opt::BackgroundColor, val.toBool() ?
                          wnd->palette().color(QPalette::Window) :
                          icfg.getBackgroundColor());
      break;
    case opt::BackgroundGradient:
      wnd->clock()->setBackground(gradient_texture(val.value<QGradient>()));
      break;
    case opt::BackgroundPattern:
      wnd->clock()->setBackground(pattern_from_file(val.toString()),
                                  icfg.getBackgroundPatternTile() ? Qt::RepeatTile : Qt::StretchTile);
      break;
    case opt::BackgroundPatternTile:
      wnd->clock()->setBackground(pattern_from_file(icfg.getBackgroundPattern()),
                                  val.toBool() ? Qt::RepeatTile : Qt::StretchTile);
      break;

    case opt::ApplyBackgroundToWindow:
      // not implemented
      break;

    case opt::TruePerCharRendering:
      wnd->clock()->setTruePerCharRendering(val.toBool());
      break;

    case opt::ColorizationEnabled:
      wnd->setColorizationEnabled(val.toBool());
      break;
    case opt::ColorizationColor:
      wnd->setColorizationColor(val.value<QColor>());
      break;
    case opt::ColorizationStrength:
      wnd->setColorizationStrength(val.toInt() / 100.);
      break;

    case opt::LayoutSpacing:
      wnd->layout()->setSpacing(val.toInt());
      break;
    case opt::CharSpacing:
      wnd->clock()->setCharSpacing(val.toInt());
      break;
    case opt::LineSpacing:
      wnd->clock()->setLineSpacing(val.toInt());
      break;
    case opt::RespectLineSpacing:
      wnd->clock()->setRespectLineSpacing(val.toBool());
      break;

    case opt::CharMargins:
      wnd->clock()->setCharMargins(val.value<QMarginsF>());
      break;
    case opt::WidgetMargins:
      wnd->clock()->setTextMargins(val.value<QMargins>());
      break;
    case opt::LayoutMargins:
      wnd->layout()->setContentsMargins(val.value<QMargins>());
      break;

    case opt::TimeFormat:
      wnd->clock()->setFormat(val.toString());
      break;
    case opt::LayoutConfig:
      wnd->clock()->setOptions(parse_layout_options(val.toString()));
      break;

    case opt::SmallerSeconds:
      applyInstanceOption(idx, opt::SecondsSize, val.toBool() ? icfg.getSecondsSize() : 100);
      break;
    case opt::SecondsSize: {
      auto k = val.toInt() / 100.;
      wnd->clock()->setTokenTransform("ss", QTransform::fromScale(k, k));
      break;
    }

    case opt::ShowLocalTime:
    case opt::TimeZone:
      // nothing to do here, handled automatically in main timer event handler
      // TODO: call handler to update the clock and avoid delay
      break;

    case opt::FlashingSeparator:
      wnd->clock()->setFlashingSeparator(val.toBool());
      break;
    case opt::UseCustomSeparators:
      applyInstanceOption(idx, opt::CustomSeparators, val.toBool() ? icfg.getCustomSeparators() : QString());
      break;
    case opt::CustomSeparators:
      wnd->clock()->setCustomSeparators(val.toString());
      break;
  }

  _pm->onOptionChanged(idx, opt, val);
}

void ClockApplication::applyWindowOption(size_t idx, opt::GlobalOptions opt, const QVariant& val)
{
  auto wnd = window(idx);

  switch (opt) {
    case opt::StayOnTop:
      wnd->setStayOnTop(val.toBool());
      break;
    case opt::FullscreenDetect:
      wnd->setFullscreenDetectEnabled(val.toBool());
      break;
    case opt::TransparentForMouse:
      wnd->setTransparentForMouse(val.toBool());
      break;

    case opt::SnapToEdge:
      wnd->setSnapToEdgeEnabled(val.toBool());
      break;
    case opt::SnapThreshold:
      wnd->setSnapThreshold(val.toInt());
      break;
    case opt::PreventOutOfScreen:
      wnd->setPreventOutOfScreenEnabled(val.toBool());
      break;

    case opt::ChangeCursorOnHover:
      wnd->setChangeCursorOnHoverEnabled(val.toBool());
      break;

    case opt::TransparentOnHover:
      wnd->setTransparentOnHoverEnabled(val.toBool());
      break;
    case opt::OpacityOnHover:
      wnd->setOpacityOnHover(val.toInt() / 100.);
      break;

    default:
      break;
  }
}
