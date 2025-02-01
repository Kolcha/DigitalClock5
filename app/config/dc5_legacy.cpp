// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dc5_legacy.hpp"

#include <QDateTime>
#include <QRegularExpression>

#include "renderer/renderer.hpp"

namespace compat {

static constexpr const char* const CONFIG_VERSION_KEY = "state/config_version";
static constexpr const int CURRENT_CONFIG_VERSION = 1;

static quint8 map_active_instances(int inst_count)
{
  quint8 act_insts = 0;
  for (int i = 0; i < inst_count; i++)
    act_insts |= (1 << i);
  Q_ASSERT(act_insts > 0);
  return act_insts;
}

static int map_tx_per_char(bool per_char)
{
  return per_char ? LinesRenderer::PerChar : LinesRenderer::AllText;
}

static bool match_window_key(QRegularExpression re, QStringView full_key, int& idx, const char* k)
{
  auto match = re.matchView(full_key);
  if (!match.hasMatch())
    return false;
  idx = match.captured(1).toInt();
  return match.captured(2) == QLatin1StringView(k);
}

static bool is_window_option(QStringView full_key, int& idx, const char* k)
{
  QRegularExpression re(R"(^Window(\d+)/(.*)$)");
  return match_window_key(std::move(re), full_key, idx, k);
}

static bool is_window_state(QStringView full_key, int& idx, const char* k)
{
  QRegularExpression re(R"(^state/window/(\d+)/(.*)$)");
  return match_window_key(std::move(re), full_key, idx, k);
}

static QString instance_key(int idx, const char* key)
{
  return QString("instance_%1/%2").arg(idx).arg(key);
}

static QString instance_state(int idx, const char* key)
{
  return QString("state/instance_%1/%2").arg(idx).arg(key);
}

static void update_true_per_char(QVariantHash& s, int idx, bool en)
{
  auto& val = s[instance_key(idx, "appearance/true_per_char_rendering")];
  val = val.toBool() || en;
}

QVariantHash mapFromDC5Legacy(const QVariantHash& settings)
{
  QVariantHash dc5s;
  for (auto iter = settings.begin(); iter != settings.end(); ++iter) {
    const auto& k = iter.key();
    const auto& v = iter.value();

    // app_global settings
    if (k == "app_global/Locale")
      dc5s["app_global/locale"] = v;

    if (k == "app_global/NumInstances")
      dc5s["app_global/active_instances"] = map_active_instances(v.toInt());
    if (k == "app_global/AppearancePerInstance")
      dc5s["app_global/options_sharing"] = !v.toBool();

    if (k == "app_global/StayOnTop")
      dc5s["behavior/stay_on_top"] = v;
    if (k == "app_global/FullscreenDetect")
      dc5s["behavior/fullscreen_detect"] = v;
    if (k == "app_global/TransparentForMouse")
      dc5s["behavior/transparent_for_mouse"] = v;

    if (k == "app_global/SnapToEdge")
      dc5s["behavior/snap_to_edge"] = v;
    if (k == "app_global/SnapThreshold")
      dc5s["behavior/snap_threshold"] = v;
    if (k == "app_global/PreventOutOfScreen")
      dc5s["behavior/prevent_out_of_screen"] = v;

    if (k == "app_global/CheckForUpdates")
      dc5s["updater/check_for_updates"] = v;
    if (k == "app_global/CheckForBetaVersion")
      dc5s["updater/check_for_beta"] = v;
    if (k == "app_global/UpdatePeriodDays")
      dc5s["updater/update_period_days"] = v;

    if (k == "app_global/TransparentOnHover")
      dc5s["behavior/transparent_on_hover"] = v;
    if (k == "app_global/OpacityOnHover")
      dc5s["behavior/opacity_on_hover"] = v;

    if (k == "app_global/Plugins")
      dc5s["app_global/plugins"] = v;

    // limits settings
    if (k == "limits/MinScaling")
      ;   // unsupported, not implemented
    if (k == "limits/MaxScaling")
      ;   // unsupported, not implemented

    if (k == "limits/BaseSize")
      dc5s["app_limits/skin_base_size"] = v;

    if (k == "limits/MinSpacing")
      ;   // unsupported, not implemented
    if (k == "limits/MaxSpacing")
      ;   // unsupported, not implemented

    // app global state (state/app)
    if (k == "state/app/LastUpdateCheck")
      dc5s["state/updater/last_update_check"] = v.toDateTime().toSecsSinceEpoch();

    // window settings
    // appearance settings
    if (int idx = -1; is_window_option(k, idx, "appearance/ScalingH"))
      dc5s[instance_key(idx, "appearance/scaling")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/ScalingV"))
      ;   // unsupported, only one public scaling option

    if (int idx = -1; is_window_option(k, idx, "appearance/LayoutSpacing"))
      dc5s[instance_key(idx, "geometry/layout_spacing")] = v;

    if (int idx = -1; is_window_option(k, idx, "appearance/Opacity"))
      dc5s[instance_key(idx, "appearance/opacity")] = v;

    if (int idx = -1; is_window_option(k, idx, "appearance/ApplyColorization"))
      dc5s[instance_key(idx, "appearance/colorization_enabled")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/ColorizationStrength"))
      dc5s[instance_key(idx, "appearance/colorization_strength")] = static_cast<int>(v.toReal() * 100);
    if (int idx = -1; is_window_option(k, idx, "appearance/ColorizationColor"))
      dc5s[instance_key(idx, "appearance/colorization_color")] = v;

    if (int idx = -1; is_window_option(k, idx, "appearance/UseFontInsteadOfSkin"))
      dc5s[instance_key(idx, "appearance/use_skin")] = !v.toBool();
    if (int idx = -1; is_window_option(k, idx, "appearance/Font"))
      dc5s[instance_key(idx, "appearance/font")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/Skin"))
      dc5s[instance_key(idx, "appearance/skin")] = v;

    if (int idx = -1; is_window_option(k, idx, "appearance/SeparatorFlashes"))
      dc5s[instance_key(idx, "clock/flashing_separator")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/UseCustomSeparators"))
      dc5s[instance_key(idx, "clock/use_custom_separators")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/CustomSeparators"))
      dc5s[instance_key(idx, "clock/custom_separators")] = v;

    if (int idx = -1; is_window_option(k, idx, "appearance/TimeFormat"))
      dc5s[instance_key(idx, "clock/time_format")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/SecondsScaleFactor")) {
      dc5s[instance_key(idx, "clock/smaller_seconds")] = v.toInt() < 100;
      dc5s[instance_key(idx, "clock/seconds_size")] = std::clamp(v.toInt(), 10, 100);
    }

    if (int idx = -1; is_window_option(k, idx, "appearance/UseSystemForeground"))
      dc5s[instance_key(idx, "appearance/texture_color_use_theme")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/TextureType"))
      dc5s[instance_key(idx, "appearance/texture_type")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/TextureColor"))
      dc5s[instance_key(idx, "appearance/texture_color")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/TextureGradient"))
      dc5s[instance_key(idx, "appearance/texture_gradient")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/TexturePatternFile"))
      dc5s[instance_key(idx, "appearance/texture_pattern")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/TextureStretch"))
      dc5s[instance_key(idx, "appearance/texture_pattern_tile")] = !v.toBool();
    if (int idx = -1; is_window_option(k, idx, "appearance/TexturePerCharacter")) {
      dc5s[instance_key(idx, "appearance/texture_customization")] = map_tx_per_char(v.toBool());
      update_true_per_char(dc5s, idx, v.toBool());
    }

    if (int idx = -1; is_window_option(k, idx, "appearance/UseSystemBackground"))
      dc5s[instance_key(idx, "appearance/background_color_use_theme")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/BackgroundType"))
      dc5s[instance_key(idx, "appearance/background_type")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/BackgroundColor"))
      dc5s[instance_key(idx, "appearance/background_color")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/BackgroundGradient"))
      dc5s[instance_key(idx, "appearance/background_gradient")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/BackgroundPatternFile"))
      dc5s[instance_key(idx, "appearance/background_pattern")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/BackgroundStretch"))
      dc5s[instance_key(idx, "appearance/background_pattern_tile")] = !v.toBool();
    if (int idx = -1; is_window_option(k, idx, "appearance/BackgroundPerCharacter")) {
      dc5s[instance_key(idx, "appearance/background_customization")] = map_tx_per_char(v.toBool());
      update_true_per_char(dc5s, idx, v.toBool());
    }

    if (int idx = -1; is_window_option(k, idx, "appearance/SpacingH"))
      dc5s[instance_key(idx, "geometry/char_spacing")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/SpacingV"))
      dc5s[instance_key(idx, "geometry/line_spacing")] = v;

    if (int idx = -1; is_window_option(k, idx, "appearance/CharMargins"))
      dc5s[instance_key(idx, "geometry/char_margins")] = v;
    if (int idx = -1; is_window_option(k, idx, "appearance/TextMargins"))
      dc5s[instance_key(idx, "geometry/widget_margins")] = v;

    if (int idx = -1; is_window_option(k, idx, "appearance/IgnoreAX"))
      ;   // unsupported by design, replaced with another option
    if (int idx = -1; is_window_option(k, idx, "appearance/IgnoreAY"))
      dc5s[instance_key(idx, "geometry/respect_line_spacing")] = !v.toBool();

    if (int idx = -1; is_window_option(k, idx, "appearance/LayoutConfig"))
      dc5s[instance_key(idx, "clock/layout_config_string")] = v;

    // generic settings
    if (int idx = -1; is_window_option(k, idx, "generic/ShowLocalTime"))
      dc5s[instance_key(idx, "clock/show_local_time")] = v;
    if (int idx = -1; is_window_option(k, idx, "generic/TimeZone"))
      dc5s[instance_key(idx, "clock/time_zone")] = v;

    // window state (state/window/%1)
    if (int idx = -1; is_window_state(k, idx, "origin"))
      dc5s[instance_state(idx, "origin_pos")] = v;
    if (int idx = -1; is_window_state(k, idx, "anchor"))
      dc5s[instance_state(idx, "anchor_point")] = v;
  }
  return dc5s;
}

void migrateConfig(QSettings& settings)
{
  if (settings.value(CONFIG_VERSION_KEY).toInt() == CURRENT_CONFIG_VERSION)
    return;

  QVariantHash orig_settings;
  const auto keys = settings.allKeys();
  for (const auto& k : keys)
    orig_settings[k] = settings.value(k);

  const auto migr_settings = mapFromDC5Legacy(orig_settings);
  if (!orig_settings.isEmpty() && migr_settings.isEmpty())
    return;

  for (const auto& k : keys)
    settings.remove(k);

  for (auto iter = migr_settings.begin(); iter != migr_settings.end(); ++iter)
    settings.setValue(iter.key(), iter.value());

  settings.setValue(CONFIG_VERSION_KEY, CURRENT_CONFIG_VERSION);
}

} // namespace compat
