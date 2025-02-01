/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dc4_to_dc5.hpp"

#include <QDataStream>
#include <QFile>
#include <QLocale>
#include <QVariantHash>
#include <QVariantMap>

#include "common/instance_options.hpp"
#include "gui/clock_window.hpp"
#include "renderer/renderer.hpp"

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
  return bg_enable ? tx::Color : tx::None;
}

static int map_bg_customization(bool bg_enable)
{
  return bg_enable ? LinesRenderer::AllText : LinesRenderer::None;
}

static int map_texture_type(int dc4tt)
{
  switch (dc4tt) {
    case 0: return tx::None;
    case 1: return tx::Color;
    case 2: return tx::Pattern;
  }
  return tx::None;
}

static int map_tx_per_char(bool per_char)
{
  return per_char ? LinesRenderer::PerChar : LinesRenderer::AllText;
}

static QString instance_key(int idx, const char* key)
{
  return QString("instance_%1/%2").arg(idx).arg(key);
}

QVariantHash mapFromDC4(const QVariantMap& settings, int idx)
{
  QVariantHash dc5s;
  for (auto iter = settings.begin(); iter != settings.end(); ++iter) {
    const auto& k = iter.key();
    const auto& v = iter.value();

    // clock settings
    if (k == "clock/opacity")
      dc5s[instance_key(idx, "appearance/opacity")] = static_cast<int>(v.toReal() * 100);
    if (k == "clock/stay_on_top")
      dc5s["behavior/stay_on_top"] = v.toBool();
    if (k == "clock/transp_for_input")
      dc5s["behavior/transparent_for_mouse"] = v.toBool();
    if (k == "clock/separator_flash")
      dc5s[instance_key(idx, "clock/flashing_separator")] = v.toBool();
    if (k == "clock/plugins")
      dc5s["app_global/plugins"] = v.toStringList();
    if (k == "clock/time_format") {
      auto fmt = v.toString();
      if (fmt.isEmpty())
        fmt = QLocale().timeFormat(QLocale::ShortFormat);
      dc5s[instance_key(idx, "clock/time_format")] = fmt;
    }
    if (k == "clock/alignment")
      dc5s[QString("state/instance_%1/anchor_point").arg(idx)] = map_alignment(v.toInt());
    if (k == "clock/background_enabled") {
      // TODO: map to "appearance/apply_background_to_window" once it be implemented
      dc5s[instance_key(idx, "appearance/background_customization")] = map_bg_customization(v.toBool());
      dc5s[instance_key(idx, "appearance/background_type")] = map_background_type(v.toBool());
    }
    if (k == "clock/background_color")
      dc5s[instance_key(idx, "appearance/background_color")] = v.value<QColor>();
    if (k == "clock/fullscreen_detect")
      dc5s["behavior/fullscreen_detect"] = v.toBool();
    if (k == "clock/show_on_all_desktops")
      ;   // unsupported, not implemented
    if (k == "clock/local_time")
      dc5s[instance_key(idx, "clock/show_local_time")] = v.toBool();
    if (k == "clock/time_zone")
      dc5s[instance_key(idx, "clock/time_zone")] = v.toString();

    // skin settings
    if (k == "skin/name") {     // TODO: map name to folder, handle "no skin"
      if (v.toString() == "Text Skin") {
        dc5s[instance_key(idx, "appearance/use_skin")] = false;
      } else {
        dc5s[instance_key(idx, "appearance/use_skin")] = true;
        dc5s[instance_key(idx, "appearance/skin")] = v.toString();
      }
    }
    if (k == "skin/font")
      dc5s[instance_key(idx, "appearance/font")] = v.value<QFont>();
    if (k == "skin/zoom")
      dc5s[instance_key(idx, "appearance/scaling")] = static_cast<int>(v.toReal() * 100);
    if (k == "skin/color")
      dc5s[instance_key(idx, "appearance/texture_color")] = v.value<QColor>();
    if (k == "skin/texture")
      dc5s[instance_key(idx, "appearance/texture_pattern")] = v.toString();
    if (k == "skin/texture_type")
      dc5s[instance_key(idx, "appearance/texture_type")] = map_texture_type(v.toInt());
    if (k == "skin/texture_per_element") {
      dc5s[instance_key(idx, "appearance/texture_customization")] = map_tx_per_char(v.toBool());
      dc5s[instance_key(idx, "appearance/true_per_char_rendering")] = settings["skin/name"].toString() != "Text Skin";
    }
    if (k == "skin/texture_draw_mode")
      dc5s[instance_key(idx, "appearance/texture_pattern_tile")] = v.toInt() == 1;
    if (k == "skin/customization") {
      switch (v.toInt()) {
        case 0:
          dc5s[instance_key(idx, "appearance/texture_type")] = (int)tx::None;
          dc5s[instance_key(idx, "appearance/texture_customization")] = (int)LinesRenderer::None;
          dc5s[instance_key(idx, "appearance/colorization_enabled")] = false;
          break;
        case 1:
          dc5s[instance_key(idx, "appearance/texture_type")] = map_texture_type(settings["skin/texture_type"].toInt());
          dc5s[instance_key(idx, "appearance/texture_customization")] = map_tx_per_char(settings["skin/texture_per_element"].toBool());
          dc5s[instance_key(idx, "appearance/colorization_enabled")] = false;
          break;
        case 2:
          dc5s[instance_key(idx, "appearance/texture_type")] = (int)tx::None;
          dc5s[instance_key(idx, "appearance/texture_customization")] = (int)LinesRenderer::None;
          dc5s[instance_key(idx, "appearance/colorization_enabled")] = true;
          break;
      }
    }
    if (k == "skin/spacing") {
      dc5s[instance_key(idx, "geometry/char_spacing")] = v.toInt();
      dc5s[instance_key(idx, "geometry/line_spacing")] = v.toInt();
      dc5s[instance_key(idx, "geometry/respect_line_spacing")] = false;
    }
    if (k == "skin/colorize_color")
      dc5s[instance_key(idx, "appearance/colorization_color")] = v.value<QColor>();
    if (k == "skin/colorize_level")
      dc5s[instance_key(idx, "appearance/colorization_strength")] = static_cast<int>(v.toReal() * 100);

    // updater settings
    if (k == "updater/autoupdate")
      dc5s["updater/check_for_updates"] = v.toBool();
    if (k == "updater/update_period")
      dc5s["updater/update_period_days"] = v.toInt();
    if (k == "updater/check_for_beta")
      dc5s["updater/check_for_beta"] = v.toBool();

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
      dc5s["behavior/prevent_out_of_screen"] = v.toBool();
    if (k == "app/single_instance")
      ;   // unsupported, not implemented

    // window settings
    if (k == "window/show_border")
      ;   // unsupported, hardcoded
    if (k == "window/snap_to_edges")
      dc5s["behavior/snap_to_edge"] = v.toBool();
    if (k == "window/snap_threshold")
      dc5s["behavior/snap_threshold"] = v.toInt();
    if (k == "window/refresh_interval")
      ;   // unsupported, not implemented
    if (k == "window/show_on_all_monitors")
      ;   // unsupported, not relevant
    if (k == "window/transparent_on_hover")
      dc5s["behavior/transparent_on_hover"] = v.toBool();
    if (k == "window/opacity_on_hover")
      dc5s["behavior/opacity_on_hover"] = static_cast<int>(v.toReal() * 100);
    if (k == "window/hover_buttons")
      ;   // unsupported, not implemented
    if (k == "window/move_step")
      ;   // unsupported, not implemented
    if (k == "window/show_taskbar_icon")
      ;   // unsupported, not implemented
  }

  if (!dc5s.isEmpty()) {
    dc5s[instance_key(idx, "appearance/texture_color_use_theme")] = false;
    dc5s[instance_key(idx, "appearance/background_color_use_theme")] = false;
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
