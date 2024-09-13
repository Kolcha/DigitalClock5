/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"

#include <QDesktopServices>

#include <QColorDialog>
#include <QFileDialog>
#include <QFontDialog>

#include <QGraphicsColorizeEffect>

#include <gradient_dialog.h>

#include "core/application.hpp"
#include "platform/autostart.h"
#include "plugin_list_item_widget.hpp"

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

QFont actual_font(const QFont& fnt)
{
  QFont res_font = fnt;
  QFontInfo fi(fnt);
  res_font.setFamily(fi.family());
  res_font.setPointSizeF(fi.pointSizeF());
  res_font.setStyle(fi.style());
  res_font.setStyleHint(fi.styleHint());
  return res_font;
}

} // namespace

SettingsDialog::SettingsDialog(Application* app, int idx, QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
  , app(app)
  , _curr_idx(idx)
{
  ui->setupUi(this);

  // import/export is not implemnted for now
  ui->import_btn->hide();
  ui->export_btn->hide();

  app->window(_curr_idx)->enableFrame();

  QSignalBlocker _(ui->windows_box);
  for (int i = 0; i < app->config().global().getNumInstances(); i++)
    ui->windows_box->addItem(tr("window %1").arg(i+1));
  ui->windows_box->setCurrentIndex(_curr_idx);
  ui->windows_box->setVisible(app->config().global().getNumInstances() > 1);

  initAppGlobalTab();
  initGeneralTab(_curr_idx);
  initAppearanceTab(_curr_idx);
  initMiscTab(_curr_idx);
  initPluginsTab();
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
  app->config().storage().commitAll();
  QDialog::accept();
}

void SettingsDialog::reject()
{
  const auto prev_plugins = app->config().global().getPlugins();
  for (const auto& p : prev_plugins) app->pluginManager().unloadPlugin(p);
  app->config().storage().discardAll();
  app->configureWindows();
  const auto curr_plugins = app->config().global().getPlugins();
  for (const auto& p : curr_plugins) app->pluginManager().loadPlugin(p);
  app->retranslateUI();
  QDialog::reject();
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
  app->config().global().setLocale(ui->lang_list->itemData(index).toString());
  app->retranslateUI();
  ui->retranslateUi(this);
  initPluginsTab();
}

void SettingsDialog::on_enable_autostart_clicked(bool checked)
{
  Q_UNUSED(checked);
  // applied on changes commit
}

void SettingsDialog::on_enable_stay_on_top_clicked(bool checked)
{
  app->config().global().setStayOnTop(checked);
}

void SettingsDialog::on_fullscreen_detect_clicked(bool checked)
{
  app->config().global().setFullscreenDetect(!checked);
  applyWindowOption(&ClockWindow::setFullscreenDetect, !checked);
}

void SettingsDialog::on_enable_transp_for_input_clicked(bool checked)
{
  app->config().global().setTransparentForMouse(checked);
}

void SettingsDialog::on_snap_to_edges_clicked(bool checked)
{
  app->config().global().setSnapToEdge(checked);
  applyWindowOption(&ClockWindow::setSnapToEdge, checked, app->config().global().getSnapThreshold());
}

void SettingsDialog::on_snap_threshold_valueChanged(int arg1)
{
  app->config().global().setSnapThreshold(arg1);
  applyWindowOption(&ClockWindow::setSnapThreshold, arg1);
}

void SettingsDialog::on_keep_always_visible_clicked(bool checked)
{
  app->config().global().setPreventOutOfScreen(checked);
  applyWindowOption(&ClockWindow::setKeepVisible, checked);
}

void SettingsDialog::on_enable_multiwindow_clicked(bool checked)
{
  app->config().global().setNumInstances(checked ? ui->wnd_count_edit->value() : 1);
}

void SettingsDialog::on_wnd_count_edit_valueChanged(int arg1)
{
  app->config().global().setNumInstances(arg1);
}

void SettingsDialog::on_use_same_appearance_clicked(bool checked)
{
  app->config().global().setAppearancePerInstance(!checked);
}

void SettingsDialog::on_transparent_on_hover_clicked(bool checked)
{
  app->config().global().setTransparentOnHover(checked);
  applyWindowOption(&ClockWindow::setTransparentOnHover, checked);
}

void SettingsDialog::on_hide_on_mouse_hover_clicked(bool checked)
{
  int opacity = checked ? 0 : 15;
  app->config().global().setOpacityOnHover(opacity);
  applyWindowOption(&ClockWindow::setOpacityOnHover, opacity / 100.);
}

void SettingsDialog::on_enable_autoupdate_clicked(bool checked)
{
  app->config().global().setCheckForUpdates(checked);
}

void SettingsDialog::on_update_period_edit_activated(int index)
{
  int period = ui->update_period_edit->itemData(index).toInt();
  app->config().global().setUpdatePeriodDays(period);
}

void SettingsDialog::on_check_for_beta_clicked(bool checked)
{
  app->config().global().setCheckForBetaVersion(checked);
}

void SettingsDialog::on_enable_debug_options_clicked(bool checked)
{
}

void SettingsDialog::on_smaller_seconds_clicked(bool checked)
{
  int v = checked ? ui->seconds_scale_factor_edit->value() : 100;
  on_seconds_scale_factor_edit_valueChanged(v);
}

void SettingsDialog::on_seconds_scale_factor_edit_valueChanged(int arg1)
{
  qreal ssf = arg1 / 100.;
  applyClockOption(&GraphicsDateTimeWidget::setSecondsScaleFactor, ssf);
  app->config().window(_curr_idx).appearance().setSecondsScaleFactor(arg1);
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
  applyClockOption(&GraphicsDateTimeWidget::setFormat, time_format);
  app->config().window(_curr_idx).appearance().setTimeFormat(time_format);
}

void SettingsDialog::on_layout_cfg_edit_textEdited(const QString& arg1)
{
  applyClockOption(&GraphicsDateTimeWidget::setLayoutConfig, arg1);
  app->config().window(_curr_idx).appearance().setLayoutConfig(arg1);
}

void SettingsDialog::on_layout_cfg_help_btn_clicked()
{
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClock5/wiki/Multiple-lines"));
}

void SettingsDialog::on_use_custom_seps_clicked(bool checked)
{
  applyClockOption(&GraphicsDateTimeWidget::setUseCustomSeparators, checked);
  app->config().window(_curr_idx).appearance().setUseCustomSeparators(checked);
}

void SettingsDialog::on_custom_seps_edit_textEdited(const QString& arg1)
{
  applyClockOption(&GraphicsDateTimeWidget::setCustomSeparators, arg1);
  app->config().window(_curr_idx).appearance().setCustomSeparators(arg1);
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
  app->config().window(_curr_idx).generic().setShowLocalTime(!checked);
  applyTimeZoneSettings();
}

void SettingsDialog::on_time_zone_edit_activated(int index)
{
  app->config().window(_curr_idx).generic().setTimeZone(ui->time_zone_edit->itemData(index).value<QTimeZone>());
  applyTimeZoneSettings();
}

void SettingsDialog::on_multi_timezone_help_btn_clicked()
{
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClock5/wiki/Multiple-timezones"));
}

void SettingsDialog::on_font_rbtn_clicked()
{
  SectionAppearance& acfg = app->config().window(_curr_idx).appearance();
  std::shared_ptr<Skin> skin = app->skinManager().loadSkin(acfg.getFont());
  applySkin(skin);
  acfg.setUseFontInsteadOfSkin(true);
  notifyOptionChanged(&SettingsChangeTransmitter::setUseFontInsteadOfSkin, true);
}

void SettingsDialog::on_select_font_btn_clicked()
{
  SectionAppearance& acfg = app->config().window(_curr_idx).appearance();
  bool ok = false;
  QFont fnt = QFontDialog::getFont(&ok, actual_font(acfg.getFont()), this);
  if (!ok) return;
  std::shared_ptr<Skin> skin = app->skinManager().loadSkin(fnt);
  applySkin(skin);
  acfg.setFont(fnt);
  notifyOptionChanged(&SettingsChangeTransmitter::setFont, fnt);
}

void SettingsDialog::on_skin_rbtn_clicked()
{
  SectionAppearance& acfg = app->config().window(_curr_idx).appearance();
  std::shared_ptr<Skin> skin = app->skinManager().loadSkin(acfg.getSkin());
  applySkin(skin);
  acfg.setUseFontInsteadOfSkin(false);
  notifyOptionChanged(&SettingsChangeTransmitter::setUseFontInsteadOfSkin, false);
}

void SettingsDialog::on_skin_cbox_activated(int index)
{
  QString skin_name = ui->skin_cbox->itemData(index).toString();
  std::shared_ptr<Skin> skin = app->skinManager().loadSkin(skin_name);
  applySkin(skin);
  app->config().window(_curr_idx).appearance().setSkin(skin_name);
  notifyOptionChanged(&SettingsChangeTransmitter::setSkin, skin_name);
}

void SettingsDialog::on_is_separator_flashes_clicked(bool checked)
{
  applyClockOption(&GraphicsDateTimeWidget::setFlashSeparator, checked);
  app->config().window(_curr_idx).appearance().setSeparatorFlashes(checked);
  notifyOptionChanged(&SettingsChangeTransmitter::setSeparatorFlashes, checked);
}

void SettingsDialog::on_scaling_edit_valueChanged(int arg1)
{
  SectionAppearance& acfg = app->config().window(_curr_idx).appearance();
  applyWindowOption(&ClockWindow::setScaling, arg1/100., arg1/100.);
  acfg.setScalingH(arg1);
  acfg.setScalingV(arg1);
  notifyOptionChanged(&SettingsChangeTransmitter::setScalingH, arg1);
  notifyOptionChanged(&SettingsChangeTransmitter::setScalingV, arg1);
}

void SettingsDialog::on_opacity_edit_valueChanged(int arg1)
{
  applyWindowOption(&ClockWindow::setWindowOpacity, arg1/100.);
  app->config().window(_curr_idx).appearance().setOpacity(arg1);
  notifyOptionChanged(&SettingsChangeTransmitter::setOpacity, arg1);
}

void SettingsDialog::on_colorize_group_clicked(bool checked)
{
  if (checked) {
    applyColorization();
  } else {
    applyWindowOption(&ClockWindow::setGraphicsEffect, nullptr);
  }
  app->config().window(_curr_idx).appearance().setApplyColorization(checked);
  notifyOptionChanged(&SettingsChangeTransmitter::setApplyColorization, checked);
}

void SettingsDialog::on_select_colorization_color_clicked()
{
  SectionAppearance& acfg = app->config().window(_curr_idx).appearance();
  auto color = QColorDialog::getColor(acfg.getColorizationColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;
  acfg.setColorizationColor(color);
  updateColorization();
  notifyOptionChanged(&SettingsChangeTransmitter::setColorizationColor, color);
}

void SettingsDialog::on_colorization_strength_edit_valueChanged(int arg1)
{
  app->config().window(_curr_idx).appearance().setColorizationStrength(arg1/100.);
  updateColorization();
  notifyOptionChanged(&SettingsChangeTransmitter::setColorizationStrength, arg1/100.);
}

void SettingsDialog::on_texture_group_clicked(bool checked)
{
  auto& acfg = app->config().window(_curr_idx).appearance();

  if (checked) {
    if (ui->tx_options_box->currentIndex() == 0)
      acfg.setTextureType(SectionAppearance::SolidColor);

    if (ui->tx_options_box->currentIndex() == 1)
      acfg.setTextureType(SectionAppearance::Gradient);

    if (ui->tx_options_box->currentIndex() == 2)
      acfg.setTextureType(SectionAppearance::Pattern);
  } else {
    acfg.setTextureType(SectionAppearance::None);
  }

  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setTexture), acfg.getTexture());
  notifyOptionChanged(&SettingsChangeTransmitter::setTexture, acfg.getTexture());
}

void SettingsDialog::on_tx_options_box_activated(int index)
{
  auto& acfg = app->config().window(_curr_idx).appearance();
  switch (index) {
    case 0: // color
      acfg.setTextureType(SectionAppearance::SolidColor);
      break;
    case 1: // gradient
      acfg.setTextureType(SectionAppearance::Gradient);
      break;
    case 2: // pattern
      acfg.setTextureType(SectionAppearance::Pattern);
      break;
  }
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setTexture), acfg.getTexture());
  notifyOptionChanged(&SettingsChangeTransmitter::setTexture, acfg.getTexture());
}

void SettingsDialog::on_tx_options_box_currentIndexChanged(int index)
{
  disconnect(ui->tx_btn, &QToolButton::clicked, nullptr, nullptr);
  disconnect(ui->tx_option, &QCheckBox::clicked, nullptr, nullptr);
  ui->tx_option->hide();

  switch (index) {
    case 0: // color
      connect(ui->tx_btn, &QToolButton::clicked, this, &SettingsDialog::tx_select_color);
      ui->tx_option->setText(tr("follow system theme"));
      // ui->tx_option->show();
      // connect(ui->tx_option, &QCheckBox::clicked, this, &SettingsDialog::applySystemColor);
      break;
    case 1: // gradient
      connect(ui->tx_btn, &QToolButton::clicked, this, &SettingsDialog::tx_select_gradient);
      ui->tx_option->hide();
      break;
    case 2: // pattern
      connect(ui->tx_btn, &QToolButton::clicked, this, &SettingsDialog::tx_select_pattern);
      ui->tx_option->setText(tr("stretch instead of tile"));
      ui->tx_option->show();
      connect(ui->tx_option, &QCheckBox::clicked, this, &SettingsDialog::tx_pattern_stretch);
      break;
  }
}

void SettingsDialog::tx_select_color()
{
  auto& acfg = app->config().window(_curr_idx).appearance();
  auto color = QColorDialog::getColor(acfg.getTextureColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;
  acfg.setTextureColor(color);
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setTexture), color);
  notifyOptionChanged(&SettingsChangeTransmitter::setTexture, color);
}

void SettingsDialog::tx_select_gradient()
{
  auto& acfg = app->config().window(_curr_idx).appearance();
  bool ok = false;
  auto gradient = GradientDialog::getGradient(&ok,
                                              acfg.getTextureGradient(),
                                              this);
  if (!ok) return;
  gradient.setCoordinateMode(QGradient::ObjectMode);
  acfg.setTextureGradient(gradient);
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setTexture), gradient);
  notifyOptionChanged(&SettingsChangeTransmitter::setTexture, gradient);
}

void SettingsDialog::tx_select_pattern()
{
  auto& acfg = app->config().window(_curr_idx).appearance();
  auto file = QFileDialog::getOpenFileName(this,
                                           QString(),
                                           acfg.getTexturePatternFile(),
                                           tr("Images (*.png *.bmp *.jpg)"));
  if (file.isEmpty()) return;
  QPixmap pxm(file);
  acfg.setTexturePatternFile(file);
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setTexture), pxm);
  notifyOptionChanged(&SettingsChangeTransmitter::setTexture, pxm);
}

void SettingsDialog::tx_pattern_stretch(bool checked)
{
  app->config().window(_curr_idx).appearance().setTextureStretch(checked);
  applyClockOption(&GraphicsDateTimeWidget::setTextureStretch, checked);
  notifyOptionChanged(&SettingsChangeTransmitter::setTextureStretch, checked);
}

void SettingsDialog::on_tx_per_element_cb_clicked(bool checked)
{
  app->config().window(_curr_idx).appearance().setTexturePerCharacter(checked);
  applyClockOption(&GraphicsDateTimeWidget::setTexturePerChar, checked);
  notifyOptionChanged(&SettingsChangeTransmitter::setTexturePerCharacter, checked);
}

void SettingsDialog::on_background_group_clicked(bool checked)
{
  auto& acfg = app->config().window(_curr_idx).appearance();

  if (checked) {
    if (ui->bg_options_box->currentIndex() == 0)
      acfg.setBackgroundType(SectionAppearance::SolidColor);

    if (ui->bg_options_box->currentIndex() == 1)
      acfg.setBackgroundType(SectionAppearance::Gradient);

    if (ui->bg_options_box->currentIndex() == 2)
      acfg.setBackgroundType(SectionAppearance::Pattern);
  } else {
    acfg.setBackgroundType(SectionAppearance::None);
  }

  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setBackground), acfg.getBackground());
  notifyOptionChanged(&SettingsChangeTransmitter::setBackground, acfg.getBackground());
}

void SettingsDialog::on_bg_options_box_activated(int index)
{
  auto& acfg = app->config().window(_curr_idx).appearance();
  switch (index) {
    case 0: // color
      acfg.setBackgroundType(SectionAppearance::SolidColor);
      break;
    case 1: // gradient
      acfg.setBackgroundType(SectionAppearance::Gradient);
      break;
    case 2: // pattern
      acfg.setBackgroundType(SectionAppearance::Pattern);
      break;
  }
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setBackground), acfg.getBackground());
  notifyOptionChanged(&SettingsChangeTransmitter::setBackground, acfg.getBackground());
}

void SettingsDialog::on_bg_options_box_currentIndexChanged(int index)
{
  disconnect(ui->bg_btn, &QToolButton::clicked, nullptr, nullptr);
  disconnect(ui->bg_option, &QCheckBox::clicked, nullptr, nullptr);
  ui->bg_option->hide();

  switch (index) {
    case 0: // color
      connect(ui->bg_btn, &QToolButton::clicked, this, &SettingsDialog::bg_select_color);
      ui->bg_option->setText(tr("follow system theme"));
      // ui->bg_option->show();
      // connect(ui->bg_option, &QCheckBox::clicked, this, &SettingsDialog::applySystemColor);
      break;
    case 1: // gradient
      connect(ui->bg_btn, &QToolButton::clicked, this, &SettingsDialog::bg_select_gradient);
      ui->bg_option->hide();
      break;
    case 2: // pattern
      connect(ui->bg_btn, &QToolButton::clicked, this, &SettingsDialog::bg_select_pattern);
      ui->bg_option->setText(tr("stretch instead of tile"));
      ui->bg_option->show();
      connect(ui->bg_option, &QCheckBox::clicked, this, &SettingsDialog::bg_pattern_stretch);
      break;
  }
}

void SettingsDialog::bg_select_color()
{
  auto& acfg = app->config().window(_curr_idx).appearance();
  auto color = QColorDialog::getColor(acfg.getBackgroundColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;
  acfg.setBackgroundColor(color);
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setBackground), color);
  notifyOptionChanged(&SettingsChangeTransmitter::setBackground, color);
}

void SettingsDialog::bg_select_gradient()
{
  auto& acfg = app->config().window(_curr_idx).appearance();
  bool ok = false;
  auto gradient = GradientDialog::getGradient(&ok,
                                              acfg.getBackgroundGradient(),
                                              this);
  if (!ok) return;
  gradient.setCoordinateMode(QGradient::ObjectMode);
  acfg.setBackgroundGradient(gradient);
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setBackground), gradient);
  notifyOptionChanged(&SettingsChangeTransmitter::setBackground, gradient);
}

void SettingsDialog::bg_select_pattern()
{
  auto& acfg = app->config().window(_curr_idx).appearance();
  auto file = QFileDialog::getOpenFileName(this,
                                           QString(),
                                           acfg.getBackgroundPatternFile(),
                                           tr("Images (*.png *.bmp *.jpg)"));
  if (file.isEmpty()) return;
  QPixmap pxm(file);
  acfg.setBackgroundPatternFile(file);
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setBackground), pxm);
  notifyOptionChanged(&SettingsChangeTransmitter::setBackground, pxm);
}

void SettingsDialog::bg_pattern_stretch(bool checked)
{
  app->config().window(_curr_idx).appearance().setBackgroundStretch(checked);
  applyClockOption(&GraphicsDateTimeWidget::setBackgroundStretch, checked);
  notifyOptionChanged(&SettingsChangeTransmitter::setBackgroundStretch, checked);
}

void SettingsDialog::on_bg_per_element_cb_clicked(bool checked)
{
  app->config().window(_curr_idx).appearance().setBackgroundPerCharacter(checked);
  applyClockOption(&GraphicsDateTimeWidget::setBackgroundPerChar, checked);
  notifyOptionChanged(&SettingsChangeTransmitter::setBackgroundPerCharacter, checked);
}

void SettingsDialog::on_anchor_left_clicked()
{
  applyWindowOption(&ClockWindow::setAnchorPoint, ClockWindow::AnchorLeft);
  app->config().window(_curr_idx).generic().setAnchorPoint(ClockWindow::AnchorLeft);
}

void SettingsDialog::on_anchor_center_clicked()
{
  applyWindowOption(&ClockWindow::setAnchorPoint, ClockWindow::AnchorCenter);
  app->config().window(_curr_idx).generic().setAnchorPoint(ClockWindow::AnchorCenter);
}

void SettingsDialog::on_anchor_right_clicked()
{
  applyWindowOption(&ClockWindow::setAnchorPoint, ClockWindow::AnchorRight);
  app->config().window(_curr_idx).generic().setAnchorPoint(ClockWindow::AnchorRight);
}

void SettingsDialog::on_hs_edit_valueChanged(int arg1)
{
  applyClockOption(&GraphicsDateTimeWidget::setCharSpacing, arg1);
  app->config().window(_curr_idx).appearance().setSpacingH(arg1);
}

void SettingsDialog::on_vs_edit_valueChanged(int arg1)
{
  applyClockOption(&GraphicsDateTimeWidget::setLineSpacing, arg1);
  app->config().window(_curr_idx).appearance().setSpacingV(arg1);
}

void SettingsDialog::onPluginStateChanged(const QString& id, bool enabled)
{
  auto current_plugins = app->config().global().getPlugins();
  if (enabled) {
    current_plugins.append(id);
    app->pluginManager().loadPlugin(id);
  } else {
    current_plugins.removeAll(id);
    app->pluginManager().unloadPlugin(id);
  }
  app->config().global().setPlugins(current_plugins);
}

void SettingsDialog::initAppGlobalTab()
{
  // app global
  SectionAppGlobal& gs = app->config().global();
  {
    QSignalBlocker _(ui->lang_list);
    ui->lang_list->addItem(tr("Automatic"), QString("auto"));
    QSettings sl(":/langs.ini", QSettings::IniFormat);
    sl.beginGroup("langs");
    const auto ll = sl.childKeys();
    for (const auto& l : ll) ui->lang_list->addItem(sl.value(l).toString(), l);
    sl.endGroup();
    setIndexByValue(ui->lang_list, gs.getLocale());
  }
#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
  ui->enable_autostart->setChecked(IsAutoStartEnabled());
#else
  ui->enable_autostart->setVisible(false);  // not implemented
#endif
  ui->enable_stay_on_top->setChecked(gs.getStayOnTop());
#if defined(Q_OS_WINDOWS)
  ui->fullscreen_detect->setChecked(!gs.getFullscreenDetect());
#else
  ui->fullscreen_detect->setVisible(false); // not implemented
#endif
  ui->enable_transp_for_input->setChecked(gs.getTransparentForMouse());

  ui->snap_to_edges->setChecked(gs.getSnapToEdge());
  ui->snap_threshold->setValue(gs.getSnapThreshold());
  ui->keep_always_visible->setChecked(gs.getPreventOutOfScreen());

  ui->enable_multiwindow->setChecked(gs.getNumInstances() > 1);
  ui->wnd_count_edit->setValue(gs.getNumInstances());
  ui->use_same_appearance->setChecked(!gs.getAppearancePerInstance());

  ui->transparent_on_hover->setChecked(gs.getTransparentOnHover());
  ui->hide_on_mouse_hover->setChecked(gs.getOpacityOnHover() == 0);

  ui->enable_autoupdate->setChecked(gs.getCheckForUpdates());
  ui->check_for_beta->setChecked(gs.getCheckForBetaVersion());

  QSignalBlocker _(ui->update_period_edit);
  ui->update_period_edit->addItem(tr("1 day"), 1);
  ui->update_period_edit->addItem(tr("3 days"), 3);
  ui->update_period_edit->addItem(tr("1 week"), 7);
  ui->update_period_edit->addItem(tr("2 weeks"), 14);
  ui->update_period_edit->addItem(tr("1 month"), 30);
  setIndexByValue(ui->update_period_edit, gs.getUpdatePeriodDays());

  // ui->enable_debug_options->setChecked(impl->config.getEnableDebugOptions());
  ui->enable_debug_options->setVisible(false);  // no debug options for now
}

void SettingsDialog::initGeneralTab(int idx)
{
  SectionGeneric& gcfg = app->config().window(idx).generic();
  SectionAppearance& acfg = app->config().window(idx).appearance();

  QString time_format = acfg.getTimeFormat();

  ui->smaller_seconds->setChecked(acfg.getSecondsScaleFactor() < 100);
  if (ui->smaller_seconds->isChecked())
    ui->seconds_scale_factor_edit->setValue(acfg.getSecondsScaleFactor());

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
  ui->use_custom_seps->setEnabled(app->window(idx)->clock()->skin()->supportsCustomSeparator());
  ui->custom_seps_edit->setEnabled(app->window(idx)->clock()->skin()->supportsCustomSeparator());
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

  auto now = QDateTime::currentDateTimeUtc();
  for (const auto& tz_id : QTimeZone::availableTimeZoneIds()) {
    QTimeZone tz(tz_id);
    ui->time_zone_edit->addItem(tz_name(tz), QVariant::fromValue(tz));
    auto tooltip = QString("%1 (%2)").arg(
                     tz.displayName(now, QTimeZone::LongName),
                     tz.displayName(now, QTimeZone::OffsetName));
    ui->time_zone_edit->setItemData(ui->time_zone_edit->count() - 1, tooltip, Qt::ToolTipRole);
  }

  ui->use_time_zone->setChecked(!gcfg.getShowLocalTime());
  ui->time_zone_edit->setCurrentText(tz_name(gcfg.getTimeZone()));
}

void SettingsDialog::initAppearanceTab(int idx)
{
  SectionAppearance& acfg = app->config().window(idx).appearance();

  ui->font_rbtn->setChecked(acfg.getUseFontInsteadOfSkin());
  ui->skin_rbtn->setChecked(!acfg.getUseFontInsteadOfSkin());

  auto locale_cmp = [](QStringView lhs, QStringView rhs) { return QString::localeAwareCompare(lhs, rhs) < 0; };
  std::multimap<QString, QString, decltype(locale_cmp)> sorted_skins(locale_cmp);

  const auto avail_skins = app->skinManager().availableSkins();
  for (const auto& s : avail_skins)
    sorted_skins.insert({app->skinManager().metadata(s)["name"], s});
  for (const auto& [t, s] : std::as_const(sorted_skins))
    ui->skin_cbox->addItem(t, s);

  auto user_fnt = ui->skin_cbox->font();
  user_fnt.setItalic(true);
  for (int i = 0; i < ui->skin_cbox->count(); i++) {
    auto s = ui->skin_cbox->itemData(i).toString();
    if (app->skinManager().isUserSkin(s))
      ui->skin_cbox->setItemData(i, user_fnt, Qt::FontRole);
  }

  ui->skin_cbox->setCurrentIndex(-1);   // if skin is available, next line will update the index
  ui->skin_cbox->setCurrentText(app->skinManager().metadata(acfg.getSkin())["name"]);

  ui->is_separator_flashes->setChecked(acfg.getSeparatorFlashes());
  ui->scaling_edit->setValue(acfg.getScalingH());
  ui->opacity_edit->setValue(acfg.getOpacity());
  ui->colorize_group->setChecked(acfg.getApplyColorization());
  ui->colorization_strength_edit->setValue(qRound(acfg.getColorizationStrength() * 100));

  auto tx = acfg.getTexture();
  ui->texture_group->setChecked(tx.style() != Qt::NoBrush);
  if (tx.style() == Qt::SolidPattern) {
    ui->tx_options_box->setCurrentIndex(0);
  }
  if (tx.gradient() != nullptr) {
    ui->tx_options_box->setCurrentIndex(1);
  }
  if (tx.style() == Qt::TexturePattern) {
    ui->tx_options_box->setCurrentIndex(2);
    ui->tx_option->setChecked(acfg.getTextureStretch());
  }
  ui->tx_per_element_cb->setChecked(acfg.getTexturePerCharacter());
  on_tx_options_box_currentIndexChanged(ui->tx_options_box->currentIndex());

  auto bg = acfg.getBackground();
  ui->background_group->setChecked(bg.style() != Qt::NoBrush);
  if (bg.style() == Qt::SolidPattern) {
    ui->bg_options_box->setCurrentIndex(0);
  }
  if (bg.gradient() != nullptr) {
    ui->bg_options_box->setCurrentIndex(1);
  }
  if (bg.style() == Qt::TexturePattern) {
    ui->bg_options_box->setCurrentIndex(2);
    ui->bg_option->setChecked(acfg.getBackgroundStretch());
  }
  ui->bg_per_element_cb->setChecked(acfg.getBackgroundPerCharacter());
  on_bg_options_box_currentIndexChanged(ui->bg_options_box->currentIndex());

  SectionGeneric& gcfg = app->config().window(idx).generic();
  ui->anchor_left->setChecked(gcfg.getAnchorPoint() == ClockWindow::AnchorLeft);
  ui->anchor_center->setChecked(gcfg.getAnchorPoint() == ClockWindow::AnchorCenter);
  ui->anchor_right->setChecked(gcfg.getAnchorPoint() == ClockWindow::AnchorRight);

  ui->hs_edit->setValue(acfg.getSpacingH());
  ui->vs_edit->setValue(acfg.getSpacingV());
}

void SettingsDialog::initMiscTab(int idx)
{
  Q_UNUSED(idx);
  ui->tabWidget->tabBar()->setTabVisible(3, false);
}

void SettingsDialog::initPluginsTab()
{
  ui->plugins_list->clear();

  const auto plugins_ids = app->pluginManager().availablePlugins();

  QVector<PluginInfo> plugins_meta(plugins_ids.size());
  std::transform(plugins_ids.begin(), plugins_ids.end(), plugins_meta.begin(),
                 [this](auto& id) { return app->pluginManager().pluginInfo(id); });


  auto by_title = [](const PluginInfo& a, const PluginInfo& b)
  {
    return QString::localeAwareCompare(a.title, b.title) < 0;
  };
  std::sort(plugins_meta.begin(), plugins_meta.end(), by_title);

  for (const auto& plugin : std::as_const(plugins_meta)) {
    QListWidgetItem* item = new QListWidgetItem();
    PluginListItemWidget* widget = new PluginListItemWidget(ui->plugins_list);
    widget->SetInfo(plugin);
    item->setSizeHint(widget->sizeHint());
    ui->plugins_list->addItem(item);
    ui->plugins_list->setItemWidget(item, widget);
    connect(widget, &PluginListItemWidget::StateChanged, this, &SettingsDialog::onPluginStateChanged);
    connect(widget, &PluginListItemWidget::ConfigureRequested, &app->pluginManager(), &PluginManager::configurePlugin);
  }
}

void SettingsDialog::applySkin(std::shared_ptr<Skin> skin)
{
  applyClockOption(&GraphicsDateTimeWidget::setSkin, skin);
  applyClockOption(&GraphicsDateTimeWidget::setUseAlternateSeparator, skin->hasAlternateSeparator());
  applyClockOption(&GraphicsDateTimeWidget::setUseCustomSeparators, skin->supportsCustomSeparator() && ui->use_custom_seps->isChecked());

  ui->use_custom_seps->setEnabled(skin->supportsCustomSeparator());
  ui->custom_seps_edit->setEnabled(skin->supportsCustomSeparator());
  ui->custom_seps_help_btn->setEnabled(skin->supportsCustomSeparator());
}

void SettingsDialog::applyTimeZoneSettings()
{
  if (app->config().window(_curr_idx).generic().getShowLocalTime())
    app->window(_curr_idx)->clock()->setTimeZone(QDateTime::currentDateTime().timeZone());
  else
    app->window(_curr_idx)->clock()->setTimeZone(app->config().window(_curr_idx).generic().getTimeZone());
}

void SettingsDialog::applyColorization()
{
  auto create_effect = [this]() {
    SectionAppearance& acfg = app->config().window(_curr_idx).appearance();
    auto effect = new QGraphicsColorizeEffect;
    effect->setColor(acfg.getColorizationColor());
    effect->setStrength(acfg.getColorizationStrength());
    return effect;
  };

  if (app->config().global().getAppearancePerInstance()) {
    app->window(_curr_idx)->setGraphicsEffect(create_effect());
  } else {
    for (const auto& wnd : app->windows())
      wnd->setGraphicsEffect(create_effect());
  }
}

void SettingsDialog::updateColorization()
{
  SectionAppearance& acfg = app->config().window(_curr_idx).appearance();

  if (app->config().global().getAppearancePerInstance()) {
    auto effect = qobject_cast<QGraphicsColorizeEffect*>(app->window(_curr_idx)->graphicsEffect());
    if (!effect) return;
    effect->setColor(acfg.getColorizationColor());
    effect->setStrength(acfg.getColorizationStrength());
  } else {
    for (const auto& wnd : app->windows()) {
      auto effect = qobject_cast<QGraphicsColorizeEffect*>(wnd->graphicsEffect());
      if (!effect) continue;
      effect->setColor(acfg.getColorizationColor());
      effect->setStrength(acfg.getColorizationStrength());
    }
  }
}

template<typename Method, typename... Args>
void SettingsDialog::applyWindowOption(Method method, Args&&... args)
{
  if (app->config().global().getAppearancePerInstance()) {
    auto wnd = app->window(_curr_idx);
    (*wnd.*method)(std::forward<Args>(args)...);
  } else {
    for (const auto& wnd : app->windows())
      (*wnd.*method)(std::forward<Args>(args)...);
  }
}

template<typename Method, typename... Args>
void SettingsDialog::applyClockOption(Method method, Args&&... args)
{
  if (app->config().global().getAppearancePerInstance()) {
    auto wnd = app->window(_curr_idx);
    (*wnd->clock().*method)(std::forward<Args>(args)...);
  } else {
    for (const auto& wnd : app->windows())
      (*wnd->clock().*method)(std::forward<Args>(args)...);
  }
}

template<typename Method, typename... Args>
void SettingsDialog::notifyOptionChanged(Method method, Args&&... args)
{
  if (app->config().global().getAppearancePerInstance()) {
    auto t = app->settingsTransmitter(_curr_idx);
    (*t.*method)(std::forward<Args>(args)...);
  } else {
    for (const auto& t : app->settingsTransmitters())
      (*t.*method)(std::forward<Args>(args)...);
  }
}
