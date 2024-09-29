/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dc4_to_dc5.hpp"

#include <QDataStream>
#include <QFile>
#include <QVariantHash>
#include <QVariantMap>

#include "config/appearance_base.hpp"
#include "gui/clock_window.hpp"

namespace dc4 {

void importFromFile(QVariantMap& settings, const QString& filename)
{
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) return;

  QDataStream fin(&file);
  fin.setVersion(QDataStream::Qt_5_12);

  fin >> settings;
}

} // namespace dc4

namespace compat {

static int map_alignment(int dc4a)
{
  switch (dc4a) {
    case 1: return ClockWindow::AnchorLeft;
    case 2: return ClockWindow::AnchorRight;
    case 3: return ClockWindow::AnchorCenter;
  }
  return ClockWindow::AnchorLeft;
}

static int map_background_type(bool bg_enable)
{
  return bg_enable ? AppearanceSectionBase::SolidColor : AppearanceSectionBase::None;
}

static int map_texture_type(int dc4tt)
{
  switch (dc4tt) {
    case 0: return AppearanceSectionBase::None;
    case 1: return AppearanceSectionBase::SolidColor;
    case 2: return AppearanceSectionBase::Pattern;
  }
  return AppearanceSectionBase::None;
}

QVariantHash mapFromDC4(const QVariantMap& settings, int idx)
{
  QVariantHash dc5s;
  for (auto iter = settings.begin(); iter != settings.end(); ++iter) {
    const auto& k = iter.key();
    const auto& v = iter.value();

    // clock settings
    if (k == "clock/opacity")
      dc5s[QString("Window%1/appearance/Opacity").arg(idx)] = static_cast<int>(v.toReal() * 100);
    if (k == "clock/stay_on_top")
      dc5s["app_global/StayOnTop"] = v.toBool();
    if (k == "clock/transp_for_input")
      dc5s["app_global/TransparentForMouse"] = v.toBool();
    if (k == "clock/separator_flash")
      dc5s[QString("Window%1/appearance/SeparatorFlashes").arg(idx)] = v.toBool();
    if (k == "clock/plugins")
      dc5s["app_global/Plugins"] = v.toStringList();
    if (k == "clock/time_format") {
      auto fmt = v.toString();
      if (fmt.isEmpty())
        fmt = QLocale().timeFormat(QLocale::ShortFormat);
      dc5s[QString("Window%1/appearance/TimeFormat").arg(idx)] = fmt;
    }
    if (k == "clock/alignment") {
      dc5s[QString("Window%1/generic/AnchorPoint").arg(idx)] = map_alignment(v.toInt());
      dc5s[QString("state/window/%1/anchor").arg(idx)] = map_alignment(v.toInt());
    }
    if (k == "clock/background_enabled")
      dc5s[QString("Window%1/appearance/BackgroundType").arg(idx)] = map_background_type(v.toBool());
    if (k == "clock/background_color")
      dc5s[QString("Window%1/appearance/BackgroundColor").arg(idx)] = v.value<QColor>();
    if (k == "clock/fullscreen_detect")
      dc5s["app_global/FullscreenDetect"] = v.toBool();
    if (k == "clock/show_on_all_desktops")
      ;   // unsupported, not implemented
    if (k == "clock/local_time")
      dc5s[QString("Window%1/generic/ShowLocalTime").arg(idx)] = v.toBool();
    if (k == "clock/time_zone")
      dc5s[QString("Window%1/generic/TimeZone").arg(idx)] = v.toString();

    // skin settings
    if (k == "skin/name") {     // TODO: map name to folder, handle "no skin"
      if (v.toString() == "Text Skin") {
        dc5s[QString("Window%1/appearance/UseFontInsteadOfSkin").arg(idx)] = true;
      } else {
        dc5s[QString("Window%1/appearance/UseFontInsteadOfSkin").arg(idx)] = false;
        dc5s[QString("Window%1/appearance/Skin").arg(idx)] = v.toString();
      }
    }
    if (k == "skin/font")
      dc5s[QString("Window%1/appearance/Font").arg(idx)] = v.value<QFont>();
    if (k == "skin/zoom") {
      dc5s[QString("Window%1/appearance/ScalingH").arg(idx)] = static_cast<int>(v.toReal() * 100);
      dc5s[QString("Window%1/appearance/ScalingV").arg(idx)] = static_cast<int>(v.toReal() * 100);
    }
    if (k == "skin/color")
      dc5s[QString("Window%1/appearance/TextureColor").arg(idx)] = v.value<QColor>();
    if (k == "skin/texture")
      dc5s[QString("Window%1/appearance/TexturePatternFile").arg(idx)] = v.toString();
    if (k == "skin/texture_type")
      dc5s[QString("Window%1/appearance/TextureType").arg(idx)] = map_texture_type(v.toInt());
    if (k == "skin/texture_per_element")
      dc5s[QString("Window%1/appearance/TexturePerCharacter").arg(idx)] = v.toBool();
    if (k == "skin/texture_draw_mode")
      dc5s[QString("Window%1/appearance/TextureStretch").arg(idx)] = v.toInt() == 0;
    if (k == "skin/customization") {
      switch (v.toInt()) {
        case 0:
          dc5s[QString("Window%1/appearance/TextureType").arg(idx)] = (int)AppearanceSectionBase::None;
          dc5s[QString("Window%1/appearance/ApplyColorization").arg(idx)] = false;
          break;
        case 1:
          dc5s[QString("Window%1/appearance/TextureType").arg(idx)] = map_texture_type(settings["skin/texture_type"].toInt());
          dc5s[QString("Window%1/appearance/ApplyColorization").arg(idx)] = false;
          break;
        case 2:
          dc5s[QString("Window%1/appearance/TextureType").arg(idx)] = (int)AppearanceSectionBase::None;
          dc5s[QString("Window%1/appearance/ApplyColorization").arg(idx)] = true;
          break;
      }
    }
    if (k == "skin/spacing") {
      dc5s[QString("Window%1/appearance/SpacingH").arg(idx)] = v.toInt();
      dc5s[QString("Window%1/appearance/SpacingV").arg(idx)] = v.toInt();
    }
    if (k == "skin/colorize_color")
      dc5s[QString("Window%1/appearance/ColorizationColor").arg(idx)] = v.value<QColor>();
    if (k == "skin/colorize_level")
      dc5s[QString("Window%1/appearance/ColorizationStrength").arg(idx)] = v.toReal();

    // updater settings
    if (k == "updater/autoupdate")
      dc5s["app_global/CheckForUpdates"] = v.toBool();
    if (k == "updater/update_period")
      dc5s["app_global/UpdatePeriodDays"] = v.toInt();
    if (k == "updater/check_for_beta")
      dc5s["app_global/CheckForBetaVersion"] = v.toBool();

    // misc settings
    if (k == "misc/clock_url_enabled")
      ;   // unsupported, not implemented
    if (k == "misc/clock_url_string")
      ;   // unsupported, not implemented
    if (k == "misc/fullscreen_ignore_list")
      ;   // unsupported, not relevant
    if (k == "misc/better_stay_on_top")
      ;   // unsupported, not relevant
    if (k == "misc/show_hide_enabled")
      ;   // unsupported, not implemented
    if (k == "misc/export_state")
      ;   // unsupported, not implemented
    if (k == "window/always_visible")
      dc5s["app_global/PreventOutOfScreen"] = v.toBool();
    if (k == "app/single_instance")
      ;   // unsupported, not implemented

    // window settings
    if (k == "window/show_border")
      ;   // unsupported, hardcoded
    if (k == "window/snap_to_edges")
      dc5s["app_global/SnapToEdge"] = v.toBool();
    if (k == "window/snap_threshold")
      dc5s["app_global/SnapThreshold"] = v.toInt();
    if (k == "window/refresh_interval")
      ;   // unsupported, not implemented
    if (k == "window/show_on_all_monitors")
      ;   // unsupported, not relevant
    if (k == "window/transparent_on_hover")
      dc5s["app_global/TransparentOnHover"] = v.toBool();
    if (k == "window/opacity_on_hover")
      dc5s["app_global/OpacityOnHover"] = static_cast<int>(v.toReal() * 100);
    if (k == "window/hover_buttons")
      ;   // unsupported, not implemented
    if (k == "window/move_step")
      ;   // unsupported, not implemented
    if (k == "window/show_taskbar_icon")
      ;   // unsupported, not implemented
  }

  if (!dc5s.isEmpty()) {
    dc5s[QString("Window%1/appearance/UseSystemForeground").arg(idx)] = false;
    dc5s[QString("Window%1/appearance/UseSystemBackground").arg(idx)] = false;
  }

  return dc5s;
}

void importFromFile(QVariantHash& settings, const QString& filename, int idx)
{
  QVariantMap dc4s;
  dc4::importFromFile(dc4s, filename);
  settings = mapFromDC4(dc4s, idx);
}

} // namespace compat
