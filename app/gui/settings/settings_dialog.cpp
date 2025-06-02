/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"

#include <QDesktopServices>
#include <QSettings>

#include <QColorDialog>
#include <QFileDialog>
#include <QFontDialog>

#include <gradient_dialog.h>

#include "config/app_config.hpp"
#include "config/dc4_to_dc5.hpp"
#include "config/serialization.hpp"
#include "core/clock_application.hpp"
#include "core/plugin_manager_impl.hpp"
#include "gui/clock_window.hpp"
#include "platform/autostart.h"
#include "plugin_info_dialog.hpp"
#include "plugin_list_item_widget.hpp"
#include "scale_factor.hpp"

namespace {

int setIndexByValue(QComboBox* box, const QVariant& value)
{
  auto idx = box->findData(value);
  if (idx >= 0)
    box->setCurrentIndex(idx);
  return idx;
}

using namespace Qt::Literals::StringLiterals;

const QSet<QString> standard_formats = {
  u"h:mm"_s, u"H:mm"_s, u"hh:mm"_s, u"HH:mm"_s,
  u"h:mm:ss"_s, u"H:mm:ss"_s, u"hh:mm:ss"_s, u"HH:mm:ss"_s,
  u"h:mmA"_s, u"h:mma"_s, u"h:mm A"_s, u"h:mm a"_s,
  u"hh:mmA"_s, u"hh:mma"_s, u"hh:mm A"_s, u"hh:mm a"_s,
  u"hh:mm:ssA"_s, u"hh:mm:ssa"_s, u"hh:mm:ss A"_s, u"hh:mm:ss a"_s,
};

QString tz_name(const QTimeZone& tz)
{
  return QString::fromLatin1(tz.id());
}

} // namespace

SettingsDialog::SettingsDialog(ClockApplication* app, size_t idx, QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
  , app(app)
  , _curr_idx(idx)
{
  ui->setupUi(this);

  app->window(_curr_idx)->enableFrame();

  initWindowsList();

  fillLanguagesList();
  fillUpdatePeriodsList();
  fillTrayIconActions(ui->tray_icon_s_action_box);
  fillTrayIconActions(ui->tray_icon_d_action_box);
  fillTimeZonesList();
  fillSkinsList();

  fillTextureTypes(ui->tx_options_box);
  fillTextureTypes(ui->bg_options_box);

  initControlsState();
}

SettingsDialog::~SettingsDialog()
{
  app->window(_curr_idx)->disableFrame();
  delete ui;
}

void SettingsDialog::accept()
{
#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
  // autostart case is unique
  if (ui->enable_autostart->isChecked() != IsAutoStartEnabled()) {
    SetAutoStart(ui->enable_autostart->isChecked());
  }
#endif
  if (_settings_imported)
    app->settings()->commitAll();
  QDialog::accept();
}

void SettingsDialog::reject()
{
  if (_settings_imported)
    app->settings()->discardAll();
  QDialog::reject();
}

void SettingsDialog::on_import_btn_clicked()
{
  // *INDENT-OFF*
  auto filename = QFileDialog::getOpenFileName(this, tr("Import settings from ..."),
                                               QDir::homePath(),
                                               tr("Digital Clock Settings (*.dc5 *.dcs)"));
  // *INDENT-ON*
  if (filename.isEmpty()) return;

  auto ext = QFileInfo(filename).suffix().toLower();

  QVariantHash settings;

  if (ext == "dc5")
    dc5::importFromFile(settings, filename);
  if (ext == "dcs")
    compat::importFromFile(settings, filename, _curr_idx);

  if (settings.isEmpty()) return;

  app->settings()->importSettings(settings);
  _settings_imported = true;
  app->reloadConfig();
  initControlsState();
}

void SettingsDialog::on_export_btn_clicked()
{
  // *INDENT-OFF*
  auto filename = QFileDialog::getSaveFileName(this, tr("Export settings to ..."),
                                               QDir::home().filePath("clock_settings.dc5"),
                                               tr("Digital Clock 5 Settings (*.dc5)"));
  // *INDENT-ON*
  if (filename.isEmpty()) return;
  QVariantHash settings;
  app->settings()->exportSettings(settings);
  dc5::exportToFile(settings, filename);
}

void SettingsDialog::on_windows_box_currentIndexChanged(int index)
{
  if (index < 0) return;
  app->window(_curr_idx)->disableFrame();
  _curr_idx = index;
  app->window(_curr_idx)->enableFrame();
  initGeneralTab(index);
  initAppearanceTab(index);
}

void SettingsDialog::on_lang_tr_btn_clicked()
{
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClock5/wiki/Translate"));
}

void SettingsDialog::on_lang_list_activated(int index)
{
  auto loc = ui->lang_list->itemData(index).toString();
  app->config()->global()->setLocale(loc);
  emit globalOptionChanged(opt::Locale, loc);
}

void SettingsDialog::on_enable_autostart_clicked(bool checked)
{
  Q_UNUSED(checked);
  // applied on changes commit
}

void SettingsDialog::on_enable_stay_on_top_clicked(bool checked)
{
  app->config()->global()->setStayOnTop(checked);
  emit globalOptionChanged(opt::StayOnTop, checked);
}

void SettingsDialog::on_fullscreen_detect_clicked(bool checked)
{
  app->config()->global()->setFullscreenDetect(!checked);
  emit globalOptionChanged(opt::FullscreenDetect, !checked);
}

void SettingsDialog::on_enable_transp_for_input_clicked(bool checked)
{
  app->config()->global()->setTransparentForMouse(checked);
  emit globalOptionChanged(opt::TransparentForMouse, checked);
}

void SettingsDialog::on_should_change_cursor_clicked(bool checked)
{
  app->config()->global()->setChangeCursorOnHover(checked);
  emit globalOptionChanged(opt::ChangeCursorOnHover, checked);
}

void SettingsDialog::on_snap_to_edges_clicked(bool checked)
{
  app->config()->global()->setSnapToEdge(checked);
  emit globalOptionChanged(opt::SnapToEdge, checked);
}

void SettingsDialog::on_snap_threshold_valueChanged(int arg1)
{
  app->config()->global()->setSnapThreshold(arg1);
  emit globalOptionChanged(opt::SnapThreshold, arg1);
}

void SettingsDialog::on_keep_always_visible_clicked(bool checked)
{
  app->config()->global()->setPreventOutOfScreen(checked);
  emit globalOptionChanged(opt::PreventOutOfScreen, checked);
}

void SettingsDialog::on_enable_multiwindow_clicked(bool checked)
{
  QVector<size_t> l;
  if (checked) {
    for (int i = 0; i < ui->wnd_count_edit->value(); i++)
      l.push_back(i);
  } else {
    l.push_back(0);
  }
  app->config()->global()->setActiveInstancesList(l);
  emit globalOptionChanged(opt::ActiveInstances, QVariant::fromValue(l));
}

void SettingsDialog::on_wnd_count_edit_valueChanged(int arg1)
{
  QVector<size_t> l;
  for (int i = 0; i < arg1; i++)
    l.push_back(i);
  app->config()->global()->setActiveInstancesList(l);
  emit globalOptionChanged(opt::ActiveInstances, QVariant::fromValue(l));
}

void SettingsDialog::on_use_same_appearance_clicked(bool checked)
{
  app->config()->global()->setOptionsSharing(checked);
  emit globalOptionChanged(opt::OptionsSharing, checked);
}

void SettingsDialog::on_transparent_on_hover_clicked(bool checked)
{
  app->config()->global()->setTransparentOnHover(checked);
  emit globalOptionChanged(opt::TransparentOnHover, checked);
}

void SettingsDialog::on_hide_on_mouse_hover_clicked(bool checked)
{
  int opacity = checked ? 0 : 15;
  app->config()->global()->setOpacityOnHover(opacity);
  emit globalOptionChanged(opt::OpacityOnHover, opacity);
}

void SettingsDialog::on_tray_icon_s_action_enabled_clicked(bool checked)
{
  auto act = checked ? ui->tray_icon_s_action_box->currentData().value<opt::TrayIconAction>() : opt::NoAction;
  app->config()->global()->setTrayIconSingleClickAction(act);
  emit globalOptionChanged(opt::TrayIconSingleClickAction, QVariant::fromValue(act));
}

void SettingsDialog::on_tray_icon_s_action_box_activated(int index)
{
  auto act = ui->tray_icon_s_action_box->currentData().value<opt::TrayIconAction>();
  app->config()->global()->setTrayIconSingleClickAction(act);
  emit globalOptionChanged(opt::TrayIconSingleClickAction, QVariant::fromValue(act));
}

void SettingsDialog::on_tray_icon_d_action_enabled_clicked(bool checked)
{
  auto act = checked ? ui->tray_icon_d_action_box->currentData().value<opt::TrayIconAction>() : opt::NoAction;
  app->config()->global()->setTrayIconDoubleClickAction(act);
  emit globalOptionChanged(opt::TrayIconDoubleClickAction, QVariant::fromValue(act));
}

void SettingsDialog::on_tray_icon_d_action_box_activated(int index)
{
  auto act = ui->tray_icon_d_action_box->currentData().value<opt::TrayIconAction>();
  app->config()->global()->setTrayIconDoubleClickAction(act);
  emit globalOptionChanged(opt::TrayIconDoubleClickAction, QVariant::fromValue(act));
}

void SettingsDialog::on_enable_autoupdate_clicked(bool checked)
{
  app->config()->global()->setCheckForUpdates(checked);
  emit globalOptionChanged(opt::CheckForUpdates, checked);
}

void SettingsDialog::on_update_period_edit_activated(int index)
{
  int period = ui->update_period_edit->itemData(index).toInt();
  app->config()->global()->setUpdatePeriodDays(period);
  emit globalOptionChanged(opt::UpdatePeriodDays, period);
}

void SettingsDialog::on_check_for_beta_clicked(bool checked)
{
  app->config()->global()->setCheckForBetaVersion(checked);
  emit globalOptionChanged(opt::CheckForBetaVersion, checked);
}

void SettingsDialog::on_smaller_seconds_clicked(bool checked)
{
  app->config()->instance(_curr_idx)->setSmallerSeconds(checked);
  emit instanceOptionChanged(_curr_idx, opt::SmallerSeconds, checked);
}

void SettingsDialog::on_seconds_scale_factor_edit_valueChanged(int arg1)
{
  app->config()->instance(_curr_idx)->setSecondsSize(arg1);
  emit instanceOptionChanged(_curr_idx, opt::SmallerSeconds, arg1);
}

void SettingsDialog::on_use_custom_format_toggled(bool checked)
{
  ui->format_group->setDisabled(checked);

  if (checked)
    ui->am_pm_group->setDisabled(true);
  else
    ui->am_pm_group->setEnabled(ui->rb_12h->isChecked());
}

void SettingsDialog::on_use_custom_format_clicked(bool checked)
{
  if (!checked) updateTimeFormat();
}

void SettingsDialog::on_format_edit_currentTextChanged(const QString& arg1)
{
  bool is_multiline = arg1.contains("\\n");
  ui->layout_cfg_label->setEnabled(is_multiline);
  ui->layout_cfg_edit->setEnabled(is_multiline);
  ui->layout_cfg_help_btn->setEnabled(is_multiline);
}

void SettingsDialog::on_format_edit_editTextChanged(const QString& arg1)
{
  ui->format_apply_btn->setEnabled(!arg1.isEmpty());
}

void SettingsDialog::on_format_help_btn_clicked()
{
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClock5/wiki/Supported-date-time-formats"));
}

void SettingsDialog::on_format_apply_btn_clicked()
{
  auto time_format = ui->format_edit->currentText();
  app->config()->instance(_curr_idx)->setTimeFormat(time_format);
  emit instanceOptionChanged(_curr_idx, opt::TimeFormat, time_format);
}

void SettingsDialog::on_layout_cfg_edit_textEdited(const QString& arg1)
{
  app->config()->instance(_curr_idx)->setLayoutConfig(arg1);
  emit instanceOptionChanged(_curr_idx, opt::LayoutConfig, arg1);
}

void SettingsDialog::on_layout_cfg_help_btn_clicked()
{
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClock5/wiki/Multiple-lines"));
}

void SettingsDialog::on_use_custom_seps_clicked(bool checked)
{
  app->config()->instance(_curr_idx)->setUseCustomSeparators(checked);
  emit instanceOptionChanged(_curr_idx, opt::UseCustomSeparators, checked);
}

void SettingsDialog::on_custom_seps_edit_textEdited(const QString& arg1)
{
  app->config()->instance(_curr_idx)->setCustomSeparators(arg1);
  emit instanceOptionChanged(_curr_idx, opt::CustomSeparators, arg1);
}

void SettingsDialog::on_custom_seps_help_btn_clicked()
{
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClock5/wiki/Custom-separator-characters"));
}

void SettingsDialog::updateTimeFormat()
{
  QChar hc = ui->rb_24h->isChecked() ? 'H' : 'h';
  qsizetype hs = ui->leading_zero->isChecked() ? 2 : 1;

  QString time_format(hs, hc);
  time_format += QLatin1String(":mm");

  if (ui->show_seconds->isChecked())
    time_format += QLatin1String(":ss");

  if (ui->rb_12h->isChecked() && ui->show_apm->isChecked()) {
    if (ui->add_space_before_apm->isChecked())
      time_format += ' ';

    time_format += ui->rb_ucase_apm->isChecked() ? 'A' : 'a';
  }

  ui->format_edit->setCurrentText(time_format);
  on_format_apply_btn_clicked();
}

void SettingsDialog::on_use_time_zone_clicked(bool checked)
{
  app->config()->instance(_curr_idx)->setShowLocalTime(!checked);
  emit instanceOptionChanged(_curr_idx, opt::ShowLocalTime, !checked);
}

void SettingsDialog::on_time_zone_edit_activated(int index)
{
  auto tz = ui->time_zone_edit->itemData(index).value<QTimeZone>();
  app->config()->instance(_curr_idx)->setTimeZone(tz);
  emit instanceOptionChanged(_curr_idx, opt::TimeZone, QVariant::fromValue(tz));
}

void SettingsDialog::on_multi_timezone_help_btn_clicked()
{
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClock5/wiki/Multiple-timezones"));
}

void SettingsDialog::on_font_rbtn_clicked()
{
  app->config()->instance(_curr_idx)->setUseSkin(false);
  emit instanceOptionChanged(_curr_idx, opt::UseSkin, false);
}

void SettingsDialog::on_select_font_btn_clicked()
{
  InstanceConfig& acfg = *app->config()->instance(_curr_idx);
  bool ok = false;
  QFont fnt = QFontDialog::getFont(&ok, acfg.getFont(), this);
  if (!ok) return;
  acfg.setFont(fnt);
  emit instanceOptionChanged(_curr_idx, opt::Font, fnt);
}

void SettingsDialog::on_skin_rbtn_clicked()
{
  app->config()->instance(_curr_idx)->setUseSkin(true);
  emit instanceOptionChanged(_curr_idx, opt::UseSkin, true);
}

void SettingsDialog::on_skin_cbox_activated(int index)
{
  QString skin_name = ui->skin_cbox->itemData(index).toString();
  app->config()->instance(_curr_idx)->setSkin(skin_name);
  emit instanceOptionChanged(_curr_idx, opt::Skin, skin_name);
}

void SettingsDialog::on_is_separator_flashes_clicked(bool checked)
{
  app->config()->instance(_curr_idx)->setFlashingSeparator(checked);
  emit instanceOptionChanged(_curr_idx, opt::FlashingSeparator, checked);
}

void SettingsDialog::on_scaling_edit_valueChanged(int arg1)
{
  app->config()->instance(_curr_idx)->setScaling(arg1);
  const auto s = ScaleFactor(arg1 / 100.);
  emit instanceOptionChanged(_curr_idx, opt::Scaling, QVariant::fromValue(s));
}

void SettingsDialog::on_opacity_edit_valueChanged(int arg1)
{
  app->config()->instance(_curr_idx)->setOpacity(arg1);
  emit instanceOptionChanged(_curr_idx, opt::Opacity, arg1);
}

void SettingsDialog::on_colorize_group_clicked(bool checked)
{
  app->config()->instance(_curr_idx)->setColorizationEnabled(checked);
  emit instanceOptionChanged(_curr_idx, opt::ColorizationEnabled, checked);
}

void SettingsDialog::on_select_colorization_color_clicked()
{
  InstanceConfig& acfg = *app->config()->instance(_curr_idx);
  auto color = QColorDialog::getColor(acfg.getColorizationColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;
  acfg.setColorizationColor(color);
  emit instanceOptionChanged(_curr_idx, opt::ColorizationColor, color);
}

void SettingsDialog::on_colorization_strength_edit_valueChanged(int arg1)
{
  app->config()->instance(_curr_idx)->setColorizationStrength(arg1);
  emit instanceOptionChanged(_curr_idx, opt::ColorizationStrength, arg1);
}

void SettingsDialog::on_texture_group_clicked(bool checked)
{
  auto tx_tt = checked ? ui->tx_options_box->currentData().value<tx::TextureType>() : tx::None;
  app->config()->instance(_curr_idx)->setTextureType(tx_tt);
  emit instanceOptionChanged(_curr_idx, opt::TextureType, tx_tt);
}

void SettingsDialog::on_tx_options_box_activated(int index)
{
  auto tx_tt = ui->tx_options_box->itemData(index).value<tx::TextureType>();
  app->config()->instance(_curr_idx)->setTextureType(tx_tt);
  emit instanceOptionChanged(_curr_idx, opt::TextureType, tx_tt);
}

void SettingsDialog::on_tx_options_box_currentIndexChanged(int index)
{
  disconnect(ui->tx_btn, &QToolButton::clicked, nullptr, nullptr);
  disconnect(ui->tx_option, &QCheckBox::clicked, nullptr, nullptr);
  ui->tx_option->hide();

  const InstanceConfig& acfg = *app->config()->instance(_curr_idx);

  switch (ui->tx_options_box->itemData(index).value<tx::TextureType>()) {
    case tx::None:
      // do nothing, just to make clang happy...
      break;
    case tx::Color:
      connect(ui->tx_btn, &QToolButton::clicked, this, &SettingsDialog::tx_select_color);
      ui->tx_option->setText(tr("follow system theme"));
      ui->tx_option->show();
      ui->tx_option->setChecked(acfg.getTextureColorUseTheme());
      ui->tx_btn->setDisabled(acfg.getTextureColorUseTheme());
      connect(ui->tx_option, &QCheckBox::toggled, ui->tx_btn, &QWidget::setDisabled);
      connect(ui->tx_option, &QCheckBox::clicked, this, &SettingsDialog::tx_use_system_color);
      break;
    case tx::Gradient:
      connect(ui->tx_btn, &QToolButton::clicked, this, &SettingsDialog::tx_select_gradient);
      ui->tx_option->hide();
      ui->tx_btn->setEnabled(true);
      break;
    case tx::Pattern:
      connect(ui->tx_btn, &QToolButton::clicked, this, &SettingsDialog::tx_select_pattern);
      ui->tx_option->setText(tr("stretch instead of tile"));
      ui->tx_option->show();
      ui->tx_option->setChecked(!acfg.getTexturePatternTile());
      ui->tx_btn->setEnabled(true);
      connect(ui->tx_option, &QCheckBox::clicked, this, &SettingsDialog::tx_pattern_stretch);
      break;
  }
}

void SettingsDialog::tx_use_system_color(bool use)
{
  app->config()->instance(_curr_idx)->setTextureColorUseTheme(use);
  emit instanceOptionChanged(_curr_idx, opt::TextureColorUseTheme, use);
}

void SettingsDialog::tx_select_color()
{
  auto& acfg = *app->config()->instance(_curr_idx);
  auto color = QColorDialog::getColor(acfg.getTextureColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;
  acfg.setTextureColor(color);
  emit instanceOptionChanged(_curr_idx, opt::TextureColor, color);
}

void SettingsDialog::tx_select_gradient()
{
  auto& acfg = *app->config()->instance(_curr_idx);
  bool ok = false;
  // *INDENT-OFF*
  auto gradient = GradientDialog::getGradient(&ok,
                                              acfg.getTextureGradient(),
                                              this);
  // *INDENT-ON*
  if (!ok) return;
  gradient.setCoordinateMode(QGradient::ObjectMode);
  acfg.setTextureGradient(gradient);
  emit instanceOptionChanged(_curr_idx, opt::TextureGradient, QVariant::fromValue(gradient));
}

void SettingsDialog::tx_select_pattern()
{
  auto& acfg = *app->config()->instance(_curr_idx);
  // *INDENT-OFF*
  auto file = QFileDialog::getOpenFileName(this,
                                           QString(),
                                           acfg.getTexturePattern(),
                                           tr("Images (*.png *.bmp *.jpg)"));
  // *INDENT-ON*
  if (file.isEmpty()) return;
  acfg.setTexturePattern(file);
  emit instanceOptionChanged(_curr_idx, opt::TexturePattern, file);
}

void SettingsDialog::tx_pattern_stretch(bool checked)
{
  app->config()->instance(_curr_idx)->setTexturePatternTile(!checked);
  emit instanceOptionChanged(_curr_idx, opt::TexturePatternTile, !checked);
}

void SettingsDialog::on_tx_per_element_cb_clicked(bool checked)
{
  auto tx_ct = checked ? LinesRenderer::PerChar : LinesRenderer::AllText;
  app->config()->instance(_curr_idx)->setTextureCustomization(tx_ct);
  emit instanceOptionChanged(_curr_idx, opt::TextureCustomization, tx_ct);
}

void SettingsDialog::on_background_group_clicked(bool checked)
{
  auto bg_tt = checked ? ui->bg_options_box->currentData().value<tx::TextureType>() : tx::None;
  app->config()->instance(_curr_idx)->setBackgroundType(bg_tt);
  emit instanceOptionChanged(_curr_idx, opt::BackgroundType, bg_tt);
}

void SettingsDialog::on_bg_options_box_activated(int index)
{
  auto bg_tt = ui->bg_options_box->itemData(index).value<tx::TextureType>();
  app->config()->instance(_curr_idx)->setBackgroundType(bg_tt);
  emit instanceOptionChanged(_curr_idx, opt::BackgroundType, bg_tt);
}

void SettingsDialog::on_bg_options_box_currentIndexChanged(int index)
{
  disconnect(ui->bg_btn, &QToolButton::clicked, nullptr, nullptr);
  disconnect(ui->bg_option, &QCheckBox::clicked, nullptr, nullptr);
  ui->bg_option->hide();

  const InstanceConfig& acfg = *app->config()->instance(_curr_idx);

  switch (ui->bg_options_box->itemData(index).value<tx::TextureType>()) {
    case tx::None:
      // do nothing, just to make clang happy...
      break;
    case tx::Color:
      connect(ui->bg_btn, &QToolButton::clicked, this, &SettingsDialog::bg_select_color);
      ui->bg_option->setText(tr("follow system theme"));
      ui->bg_option->show();
      ui->bg_option->setChecked(acfg.getBackgroundColorUseTheme());
      ui->bg_btn->setDisabled(acfg.getBackgroundColorUseTheme());
      connect(ui->bg_option, &QCheckBox::toggled, ui->bg_btn, &QWidget::setDisabled);
      connect(ui->bg_option, &QCheckBox::clicked, this, &SettingsDialog::bg_use_system_color);
      break;
    case tx::Gradient:
      connect(ui->bg_btn, &QToolButton::clicked, this, &SettingsDialog::bg_select_gradient);
      ui->bg_option->hide();
      ui->bg_btn->setEnabled(true);
      break;
    case tx::Pattern:
      connect(ui->bg_btn, &QToolButton::clicked, this, &SettingsDialog::bg_select_pattern);
      ui->bg_option->setText(tr("stretch instead of tile"));
      ui->bg_option->show();
      ui->bg_option->setChecked(!acfg.getBackgroundPatternTile());
      ui->bg_btn->setEnabled(true);
      connect(ui->bg_option, &QCheckBox::clicked, this, &SettingsDialog::bg_pattern_stretch);
      break;
  }
}

void SettingsDialog::bg_use_system_color(bool use)
{
  app->config()->instance(_curr_idx)->setBackgroundColorUseTheme(use);
  emit instanceOptionChanged(_curr_idx, opt::BackgroundColorUseTheme, use);
}

void SettingsDialog::bg_select_color()
{
  auto& acfg = *app->config()->instance(_curr_idx);
  auto color = QColorDialog::getColor(acfg.getBackgroundColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;
  acfg.setBackgroundColor(color);
  emit instanceOptionChanged(_curr_idx, opt::BackgroundColor, color);
}

void SettingsDialog::bg_select_gradient()
{
  auto& acfg = *app->config()->instance(_curr_idx);
  bool ok = false;
  // *INDENT-OFF*
  auto gradient = GradientDialog::getGradient(&ok,
                                              acfg.getBackgroundGradient(),
                                              this);
  // *INDENT-ON*
  if (!ok) return;
  gradient.setCoordinateMode(QGradient::ObjectMode);
  acfg.setBackgroundGradient(gradient);
  emit instanceOptionChanged(_curr_idx, opt::BackgroundGradient, QVariant::fromValue(gradient));
}

void SettingsDialog::bg_select_pattern()
{
  auto& acfg = *app->config()->instance(_curr_idx);
  // *INDENT-OFF*
  auto file = QFileDialog::getOpenFileName(this,
                                           QString(),
                                           acfg.getBackgroundPattern(),
                                           tr("Images (*.png *.bmp *.jpg)"));
  // *INDENT-ON*
  if (file.isEmpty()) return;
  acfg.setBackgroundPattern(file);
  emit instanceOptionChanged(_curr_idx, opt::BackgroundPattern, file);
}

void SettingsDialog::bg_pattern_stretch(bool checked)
{
  app->config()->instance(_curr_idx)->setBackgroundPatternTile(!checked);
  emit instanceOptionChanged(_curr_idx, opt::BackgroundPatternTile, !checked);
}

void SettingsDialog::on_bg_per_element_cb_clicked(bool checked)
{
  auto bg_ct = checked ? LinesRenderer::PerChar : LinesRenderer::AllText;
  app->config()->instance(_curr_idx)->setBackgroundCustomization(bg_ct);
  emit instanceOptionChanged(_curr_idx, opt::BackgroundCustomization, bg_ct);
}

void SettingsDialog::on_anchor_left_clicked()
{
  app->window(_curr_idx)->setAnchorPoint(ClockWindow::AnchorLeft);
}

void SettingsDialog::on_anchor_center_clicked()
{
  app->window(_curr_idx)->setAnchorPoint(ClockWindow::AnchorCenter);
}

void SettingsDialog::on_anchor_right_clicked()
{
  app->window(_curr_idx)->setAnchorPoint(ClockWindow::AnchorRight);
}

void SettingsDialog::on_hs_edit_valueChanged(int arg1)
{
  app->config()->instance(_curr_idx)->setCharSpacing(arg1);
  emit instanceOptionChanged(_curr_idx, opt::CharSpacing, arg1);
}

void SettingsDialog::on_vs_edit_valueChanged(int arg1)
{
  app->config()->instance(_curr_idx)->setLineSpacing(arg1);
  emit instanceOptionChanged(_curr_idx, opt::LineSpacing, arg1);
}

void SettingsDialog::onCharMarginsChanged()
{
  QMarginsF m(ui->m_char_l->value(), ui->m_char_t->value(), ui->m_char_r->value(), ui->m_char_b->value());
  app->config()->instance(_curr_idx)->setCharMargins(m);
  emit instanceOptionChanged(_curr_idx, opt::CharMargins, QVariant::fromValue(m));
}

void SettingsDialog::onTextMarginsChanged()
{
  QMarginsF m(ui->m_text_l->value(), ui->m_text_t->value(), ui->m_text_r->value(), ui->m_text_b->value());
  app->config()->instance(_curr_idx)->setWidgetMargins(m);
  emit instanceOptionChanged(_curr_idx, opt::WidgetMargins, QVariant::fromValue(m));
}

void SettingsDialog::on_true_per_char_clicked(bool checked)
{
  app->config()->instance(_curr_idx)->setTruePerCharRendering(checked);
  emit instanceOptionChanged(_curr_idx, opt::TruePerCharRendering, checked);
}

void SettingsDialog::on_respect_line_spacing_clicked(bool checked)
{
  app->config()->instance(_curr_idx)->setRespectLineSpacing(checked);
  emit instanceOptionChanged(_curr_idx, opt::RespectLineSpacing, checked);
}

void SettingsDialog::on_layout_spacing_edit_valueChanged(int arg1)
{
  app->config()->instance(_curr_idx)->setLayoutSpacing(arg1);
  emit instanceOptionChanged(_curr_idx, opt::LayoutSpacing, arg1);
}

void SettingsDialog::on_hide_clock_widget_clicked(bool checked)
{
  app->config()->instance(_curr_idx)->setHideClockWidget(checked);
  emit instanceOptionChanged(_curr_idx, opt::HideClockWidget, checked);
}

void SettingsDialog::initAppGlobalTab()
{
  ui->lang_tr_btn->setIcon(QIcon(":/icons/set-language.svg"));
  // app global
  const GlobalConfig& gs = *app->config()->global();
  setIndexByValue(ui->lang_list, gs.getLocale());
#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
  ui->enable_autostart->setChecked(IsAutoStartEnabled());
#else
  ui->enable_autostart->setVisible(false);  // not implemented
#endif
  ui->enable_stay_on_top->setChecked(gs.getStayOnTop());
#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
  ui->fullscreen_detect->setChecked(!gs.getFullscreenDetect());
#else
  ui->fullscreen_detect->setVisible(false); // not implemented
#endif
  ui->enable_transp_for_input->setChecked(gs.getTransparentForMouse());
  ui->should_change_cursor->setChecked(gs.getChangeCursorOnHover());

  ui->snap_to_edges->setChecked(gs.getSnapToEdge());
  ui->snap_threshold->setValue(gs.getSnapThreshold());
  ui->keep_always_visible->setChecked(gs.getPreventOutOfScreen());

  const auto act_insts = gs.getActiveInstancesList();
  ui->enable_multiwindow->setChecked(act_insts.size() > 1);
  ui->wnd_count_edit->setValue(act_insts.size());
  ui->use_same_appearance->setChecked(gs.getOptionsSharing());

  ui->transparent_on_hover->setChecked(gs.getTransparentOnHover());
  ui->hide_on_mouse_hover->setChecked(gs.getOpacityOnHover() == 0);

  ui->enable_autoupdate->setChecked(gs.getCheckForUpdates());
  ui->check_for_beta->setChecked(gs.getCheckForBetaVersion());

  setIndexByValue(ui->update_period_edit, gs.getUpdatePeriodDays());

#if defined(Q_OS_WINDOWS)
  auto tray_icon_s_action = gs.getTrayIconSingleClickAction();
  auto tray_icon_d_action = gs.getTrayIconDoubleClickAction();
  ui->tray_icon_s_action_enabled->setChecked(tray_icon_s_action != opt::NoAction);
  ui->tray_icon_d_action_enabled->setChecked(tray_icon_d_action != opt::NoAction);
  if (tray_icon_s_action == opt::NoAction) tray_icon_s_action = opt::ShowHideClock;
  if (tray_icon_d_action == opt::NoAction) tray_icon_d_action = opt::OpenSettings;
  setIndexByValue(ui->tray_icon_s_action_box, QVariant::fromValue(tray_icon_s_action));
  setIndexByValue(ui->tray_icon_d_action_box, QVariant::fromValue(tray_icon_d_action));
#else
  ui->tray_icon_s_action_enabled->setVisible(false);
  ui->tray_icon_s_action_box->setVisible(false);
  ui->tray_icon_d_action_enabled->setVisible(false);
  ui->tray_icon_d_action_box->setVisible(false);
#endif
}

void SettingsDialog::initGeneralTab(size_t idx)
{
  ui->format_help_btn->setIcon(QIcon(":/icons/help-hint.svg"));
  ui->custom_seps_help_btn->setIcon(QIcon(":/icons/help-hint.svg"));
  ui->layout_cfg_help_btn->setIcon(QIcon(":/icons/help-hint.svg"));
  ui->multi_timezone_help_btn->setIcon(QIcon(":/icons/help-hint.svg"));

  const InstanceConfig& acfg = *app->config()->instance(idx);

  QString time_format = acfg.getTimeFormat();

  ui->smaller_seconds->setChecked(acfg.getSmallerSeconds());
  if (ui->smaller_seconds->isChecked())
    ui->seconds_scale_factor_edit->setValue(acfg.getSecondsSize());

  ui->use_custom_format->setChecked(!standard_formats.contains(time_format));
  ui->format_edit->setCurrentText(time_format);
  // in case of the same format as default in the UI design file signal is not called
  on_format_edit_currentTextChanged(time_format);

  if (!ui->use_custom_format->isChecked()) {
    ui->rb_12h->setChecked(time_format.contains('h'));
    ui->rb_24h->setChecked(time_format.contains('H'));
    ui->leading_zero->setChecked(time_format.contains("hh", Qt::CaseInsensitive));
    ui->show_seconds->setChecked(time_format.contains(":ss"));

    ui->show_apm->setChecked(time_format.contains('a', Qt::CaseInsensitive));
    ui->add_space_before_apm->setChecked(time_format.contains(' '));
    ui->rb_ucase_apm->setChecked(time_format.contains('A'));
    ui->rb_lcase_apm->setChecked(time_format.contains('a'));

    updateTimeFormat();
  }
  // slot is not called if initial control state matches new state,
  // but the same logic should be triggered on initialization (regardless of the state)
  on_use_custom_format_toggled(ui->use_custom_format->isChecked());

  ui->use_custom_seps->setChecked(acfg.getUseCustomSeparators());
  // ui->use_custom_seps->setEnabled(app->window(idx)->clock()->skin()->supportsCustomSeparator());
  // ui->custom_seps_edit->setEnabled(app->window(idx)->clock()->skin()->supportsCustomSeparator());
  ui->custom_seps_edit->setText(acfg.getCustomSeparators());

  ui->layout_cfg_edit->setText(acfg.getLayoutConfig());

  connect(ui->rb_12h, &QRadioButton::clicked, this, &SettingsDialog::updateTimeFormat);
  connect(ui->rb_24h, &QRadioButton::clicked, this, &SettingsDialog::updateTimeFormat);
  connect(ui->leading_zero, &QCheckBox::clicked, this, &SettingsDialog::updateTimeFormat);
  connect(ui->show_seconds, &QCheckBox::clicked, this, &SettingsDialog::updateTimeFormat);

  connect(ui->show_apm, &QCheckBox::clicked, this, &SettingsDialog::updateTimeFormat);
  connect(ui->add_space_before_apm, &QCheckBox::clicked, this, &SettingsDialog::updateTimeFormat);
  connect(ui->rb_ucase_apm, &QRadioButton::clicked, this, &SettingsDialog::updateTimeFormat);
  connect(ui->rb_lcase_apm, &QRadioButton::clicked, this, &SettingsDialog::updateTimeFormat);

  ui->use_time_zone->setChecked(!acfg.getShowLocalTime());
  ui->time_zone_edit->setCurrentText(tz_name(acfg.getTimeZone()));
}

void SettingsDialog::initAppearanceTab(size_t idx)
{
  const InstanceConfig& acfg = *app->config()->instance(idx);

  ui->font_rbtn->setChecked(!acfg.getUseSkin());
  ui->skin_rbtn->setChecked(acfg.getUseSkin());

  ui->skin_cbox->setCurrentIndex(-1);   // if skin is available, next line will update the index
  ui->skin_cbox->setCurrentText(app->getSkinManager()->info(acfg.getSkin()).metadata["name"]);

  ui->is_separator_flashes->setChecked(acfg.getFlashingSeparator());
  ui->scaling_edit->setValue(acfg.getScaling());
  ui->opacity_edit->setValue(acfg.getOpacity());
  ui->colorize_group->setChecked(acfg.getColorizationEnabled());
  ui->colorization_strength_edit->setValue(acfg.getColorizationStrength());

  if (acfg.getTextureType() != tx::None)
    setIndexByValue(ui->tx_options_box, QVariant::fromValue(acfg.getTextureType()));
  ui->tx_per_element_cb->setChecked(acfg.getTextureCustomization() == LinesRenderer::PerChar);
  on_tx_options_box_currentIndexChanged(ui->tx_options_box->currentIndex());
  ui->texture_group->setChecked(acfg.getTextureType() != tx::None);

  if (acfg.getBackgroundType() != tx::None)
    setIndexByValue(ui->bg_options_box, QVariant::fromValue(acfg.getBackgroundType()));
  ui->bg_per_element_cb->setChecked(acfg.getBackgroundCustomization() == LinesRenderer::PerChar);
  on_bg_options_box_currentIndexChanged(ui->bg_options_box->currentIndex());
  ui->background_group->setChecked(acfg.getBackgroundType() != tx::None);

  ui->anchor_left->setChecked(app->window(idx)->anchorPoint() == ClockWindow::AnchorLeft);
  ui->anchor_center->setChecked(app->window(idx)->anchorPoint() == ClockWindow::AnchorCenter);
  ui->anchor_right->setChecked(app->window(idx)->anchorPoint() == ClockWindow::AnchorRight);

  ui->hs_edit->setValue(acfg.getCharSpacing());
  ui->vs_edit->setValue(acfg.getLineSpacing());
}

void SettingsDialog::initMiscTab(size_t idx)
{
  InstanceConfig& acfg = *app->config()->instance(idx);

  const auto& cm = acfg.getCharMargins();
  ui->m_char_l->setValue(cm.left());
  ui->m_char_t->setValue(cm.top());
  ui->m_char_r->setValue(cm.right());
  ui->m_char_b->setValue(cm.bottom());

  connect(ui->m_char_l, &QDoubleSpinBox::valueChanged, this, &SettingsDialog::onCharMarginsChanged);
  connect(ui->m_char_t, &QDoubleSpinBox::valueChanged, this, &SettingsDialog::onCharMarginsChanged);
  connect(ui->m_char_r, &QDoubleSpinBox::valueChanged, this, &SettingsDialog::onCharMarginsChanged);
  connect(ui->m_char_b, &QDoubleSpinBox::valueChanged, this, &SettingsDialog::onCharMarginsChanged);

  const auto& tm = acfg.getWidgetMargins();
  ui->m_text_l->setValue(tm.left());
  ui->m_text_t->setValue(tm.top());
  ui->m_text_r->setValue(tm.right());
  ui->m_text_b->setValue(tm.bottom());

  connect(ui->m_text_l, &QDoubleSpinBox::valueChanged, this, &SettingsDialog::onTextMarginsChanged);
  connect(ui->m_text_t, &QDoubleSpinBox::valueChanged, this, &SettingsDialog::onTextMarginsChanged);
  connect(ui->m_text_r, &QDoubleSpinBox::valueChanged, this, &SettingsDialog::onTextMarginsChanged);
  connect(ui->m_text_b, &QDoubleSpinBox::valueChanged, this, &SettingsDialog::onTextMarginsChanged);

  ui->true_per_char->setChecked(acfg.getTruePerCharRendering());
  ui->respect_line_spacing->setChecked(acfg.getRespectLineSpacing());

  ui->layout_spacing_edit->setValue(acfg.getLayoutSpacing());

  ui->hide_clock_widget->setChecked(acfg.getHideClockWidget());
}

void SettingsDialog::initPluginsTab()
{
  ui->plugins_list->clear();

  auto plugins = app->getPluginManager()->plugins();
  auto by_name = [](const PluginHandle& a, const PluginHandle& b) {
    return QString::localeAwareCompare(a.name(), b.name()) < 0;
  };
  std::ranges::sort(plugins, by_name);

  for (const PluginHandle& p : std::as_const(plugins)) {
    auto widget = new PluginListItemWidget;
    widget->SetDisplayName(p.name());
    widget->SetVersion(p.metadata().value("version", "0.0.0").toString());
    widget->SetConfigurable(p.isConfigurable());
    widget->SetChecked(p.isActive());
    // *INDENT-OFF*
    connect(widget, &PluginListItemWidget::StateChanged,
            this, [this, p](bool checked) { onPluginStateChanged(p, checked); });
    connect(widget, &PluginListItemWidget::ConfigureRequested,
            [this, ph = p]() mutable { ph.configure(this, _curr_idx); });
    connect(widget, &PluginListItemWidget::InfoDialogRequested,
            this, [this, p]() { showPluginInfoDialog(p); });
    // *INDENT-ON*
    auto item = new QListWidgetItem;
    item->setSizeHint(widget->sizeHint());
    ui->plugins_list->addItem(item);
    ui->plugins_list->setItemWidget(item, widget);
  }
}

void SettingsDialog::initControlsState()
{
  initAppGlobalTab();
  initGeneralTab(_curr_idx);
  initAppearanceTab(_curr_idx);
  initMiscTab(_curr_idx);
  initPluginsTab();
}

void SettingsDialog::onPluginStateChanged(PluginHandle ph, bool enabled)
{
  ph.setActive(enabled);

  auto current_plugins = app->config()->global()->getPlugins();
  if (enabled)
    current_plugins.push_back(ph.id());
  else
    current_plugins.removeOne(ph.id());

  app->config()->global()->setPlugins(current_plugins);
}

void SettingsDialog::showPluginInfoDialog(const PluginHandle& ph)
{
  auto dlg = new PluginInfoDialog(ph, this);
  dlg->setAttribute(Qt::WA_DeleteOnClose, true);
  dlg->setWindowModality(Qt::WindowModal);
  dlg->show();
}

void SettingsDialog::initWindowsList()
{
  QSignalBlocker _(ui->windows_box);
  const auto act_insts = app->config()->global()->getActiveInstancesList();
  for (auto i : act_insts)
    ui->windows_box->addItem(tr("window %1").arg(i+1));
  ui->windows_box->setCurrentIndex(_curr_idx);
  ui->windows_box->setVisible(act_insts.size() > 1);
}

void SettingsDialog::fillLanguagesList()
{
  QSignalBlocker _(ui->lang_list);
  ui->lang_list->addItem(tr("Automatic"), QString("auto"));
  QSettings sl(":/langs.ini", QSettings::IniFormat);
  sl.beginGroup("langs");
  const auto ll = sl.childKeys();
  for (const auto& l : ll) ui->lang_list->addItem(sl.value(l).toString(), l);
  sl.endGroup();
}

void SettingsDialog::fillUpdatePeriodsList()
{
  QSignalBlocker _(ui->update_period_edit);
  ui->update_period_edit->addItem(tr("1 day"), 1);
  ui->update_period_edit->addItem(tr("3 days"), 3);
  ui->update_period_edit->addItem(tr("1 week"), 7);
  ui->update_period_edit->addItem(tr("2 weeks"), 14);
  ui->update_period_edit->addItem(tr("1 month"), 30);
}

void SettingsDialog::fillTrayIconActions(QComboBox* box)
{
  QSignalBlocker _(box);
  box->addItem(tr("open settings"), QVariant::fromValue(opt::OpenSettings));
  box->addItem(tr("show/hide clock"), QVariant::fromValue(opt::ShowHideClock));
  box->addItem(tr("toggle \"stay on top\""), QVariant::fromValue(opt::ToggleStayOnTop));
}

void SettingsDialog::fillTimeZonesList()
{
  QSignalBlocker _(ui->time_zone_edit);
  auto now = QDateTime::currentDateTimeUtc();
  for (const auto& tz_id : QTimeZone::availableTimeZoneIds()) {
    QTimeZone tz(tz_id);
    ui->time_zone_edit->addItem(tz_name(tz), QVariant::fromValue(tz));
    auto tooltip = QString("%1 (%2)").arg(
                     tz.displayName(now, QTimeZone::LongName),
                     tz.displayName(now, QTimeZone::OffsetName));
    ui->time_zone_edit->setItemData(ui->time_zone_edit->count() - 1, tooltip, Qt::ToolTipRole);
  }
}

void SettingsDialog::fillSkinsList()
{
  QSignalBlocker _(ui->skin_cbox);
  auto locale_cmp = [](QStringView lhs, QStringView rhs) { return QString::localeAwareCompare(lhs, rhs) < 0; };
  std::multimap<QString, QString, decltype(locale_cmp)> sorted_skins(locale_cmp);

  const auto avail_skins = app->getSkinManager()->infoList();
  for (const auto& s : avail_skins)
    sorted_skins.insert({s.metadata["name"], s.id});

  for (const auto& [t, s] : std::as_const(sorted_skins))
    ui->skin_cbox->addItem(t, s);

  auto user_fnt = ui->skin_cbox->font();
  user_fnt.setItalic(true);
  for (int i = 0; i < ui->skin_cbox->count(); i++) {
    auto s = ui->skin_cbox->itemData(i).toString();
    if (app->getSkinManager()->info(s).user_provided)
      ui->skin_cbox->setItemData(i, user_fnt, Qt::FontRole);
  }
}

void SettingsDialog::fillTextureTypes(QComboBox* box)
{
  QSignalBlocker _(box);
  box->addItem(tr("solid color"), QVariant::fromValue(tx::Color));
  box->addItem(tr("gradient"), QVariant::fromValue(tx::Gradient));
  box->addItem(tr("pattern"), QVariant::fromValue(tx::Pattern));
}
