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

#include <QPainter>

#include <gradient_dialog.h>

#include "application.hpp"
#include "state.hpp"
#include "config/custom_converters.hpp"

namespace {

class SettingsDialogState : public StateImpl {
public:
  using StateImpl::StateImpl;

  CONFIG_OPTION_Q(QColor, TextureColor, QColor(112, 96, 240))
  CONFIG_OPTION_Q(QGradient, TextureGradient, sample_conical_gradient())
  CONFIG_OPTION_Q(QPixmap, TexturePattern, sample_pattern())
  CONFIG_OPTION_Q(QColor, BackgroundColor, QColor(0, 0, 0, 160))
  CONFIG_OPTION_Q(QGradient, BackgroundGradient, sample_linear_gradient())
  CONFIG_OPTION_Q(QPixmap, BackgroundPattern, sample_pattern())

private:
  static QGradient sample_conical_gradient();
  static QGradient sample_linear_gradient();
  static QPixmap sample_pattern();
};

QGradient SettingsDialogState::sample_conical_gradient()
{
  QConicalGradient g(0.5, 0.5, 45.0);
  g.setStops({
    {0.00, {170,   0,   0}},  // #aa0000
    {0.20, {  0,  85, 255}},  // #0055ff
    {0.45, {  0, 170,   0}},  // #00aa00
    {0.65, {255, 255,   0}},  // #ffff00
    {1.00, {170,   0,   0}},  // #aa0000
  });
  g.setCoordinateMode(QGradient::ObjectMode);
  return g;
}

QGradient SettingsDialogState::sample_linear_gradient()
{
  QLinearGradient g(0., 0., 0., 1.);
  g.setColorAt(0.0, Qt::transparent);
  g.setColorAt(0.6, Qt::transparent);
  g.setColorAt(1.0, QColor(85, 0, 255));
  g.setCoordinateMode(QGradient::ObjectMode);
  return g;
}

QPixmap SettingsDialogState::sample_pattern()
{
  QPixmap pxm(8, 8);
  pxm.fill(QColor(160, 0, 160));
  QPainter p(&pxm);
  p.fillRect(0, 0, 4, 4, QColor(224, 0, 224));
  p.fillRect(4, 4, 8, 8, QColor(224, 0, 224));
  return pxm;
}

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

  QSignalBlocker _(ui->windows_box);
  for (int i = 0; i < app->config().global().getNumInstances(); i++)
    ui->windows_box->addItem(tr("window %1").arg(i+1));
  ui->windows_box->setCurrentIndex(_curr_idx);
  ui->windows_box->setVisible(app->config().global().getNumInstances() > 1);

  initAppGlobalTab();
  initGeneralTab(_curr_idx);
  initAppearanceTab(_curr_idx);
  initPluginsTab();
}

SettingsDialog::~SettingsDialog()
{
  delete ui;
}

void SettingsDialog::accept()
{
  app->config().storage().commitAll();
  QDialog::accept();
}

void SettingsDialog::reject()
{
  app->config().storage().discardAll();
  app->configureWindows();
  QDialog::reject();
}

void SettingsDialog::on_windows_box_currentIndexChanged(int index)
{
  if (index < 0) return;
  _curr_idx = index;
  initGeneralTab(index);
  initAppearanceTab(index);
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

void SettingsDialog::on_enable_transp_for_input_clicked(bool checked)
{
  app->config().global().setTransparentForMouse(checked);
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
}

void SettingsDialog::on_hide_on_mouse_hover_clicked(bool checked)
{
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

void SettingsDialog::on_format_edit_textChanged(const QString& arg1)
{
  bool is_multiline = arg1.contains("\\n");
  ui->layout_cfg_label->setEnabled(is_multiline);
  ui->layout_cfg_edit->setEnabled(is_multiline);
  ui->layout_cfg_help_btn->setEnabled(is_multiline);
}

void SettingsDialog::on_format_edit_textEdited(const QString& arg1)
{
  ui->format_apply_btn->setEnabled(!arg1.isEmpty());
}

void SettingsDialog::on_format_help_btn_clicked()
{
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClockNext/wiki/Supported-date-time-formats"));
}

void SettingsDialog::on_format_apply_btn_clicked()
{
  auto time_format = ui->format_edit->text();
  applyClockOption(&GraphicsDateTimeWidget::setFormat, time_format);
  app->config().window(_curr_idx).appearance().setTimeFormat(time_format);
}

void SettingsDialog::on_layout_cfg_edit_textEdited(const QString& arg1)
{
  // impl->skin->setLayoutConfig(arg1);
  // impl->scfg->setLayoutConfig(arg1);
}

void SettingsDialog::on_layout_cfg_help_btn_clicked()
{
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClockNext/wiki/Multiple-lines"));
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

  ui->format_edit->setText(time_format);
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

void SettingsDialog::on_font_rbtn_clicked()
{
  SectionAppearance& acfg = app->config().window(_curr_idx).appearance();
  std::shared_ptr<Skin> skin = app->skinManager().loadSkin(acfg.getFont());
  applyClockOption(&GraphicsDateTimeWidget::setSkin, skin);
  acfg.setUseFontInsteadOfSkin(true);
}

void SettingsDialog::on_select_font_btn_clicked()
{
  SectionAppearance& acfg = app->config().window(_curr_idx).appearance();
  bool ok = false;
  QFont fnt = QFontDialog::getFont(&ok, actual_font(acfg.getFont()), this);
  if (!ok) return;
  std::shared_ptr<Skin> skin = app->skinManager().loadSkin(fnt);
  applyClockOption(&GraphicsDateTimeWidget::setSkin, skin);
  acfg.setFont(fnt);
}

void SettingsDialog::on_skin_rbtn_clicked()
{
  SectionAppearance& acfg = app->config().window(_curr_idx).appearance();
  std::shared_ptr<Skin> skin = app->skinManager().loadSkin(acfg.getSkin());
  applyClockOption(&GraphicsDateTimeWidget::setSkin, skin);
  acfg.setUseFontInsteadOfSkin(false);
}

void SettingsDialog::on_skin_cbox_activated(int index)
{
  QString skin_name = ui->skin_cbox->itemData(index).toString();
  std::shared_ptr<Skin> skin = app->skinManager().loadSkin(skin_name);
  applyClockOption(&GraphicsDateTimeWidget::setSkin, skin);
  app->config().window(_curr_idx).appearance().setSkin(skin_name);
}

void SettingsDialog::on_is_separator_flashes_clicked(bool checked)
{
  applyClockOption(&GraphicsDateTimeWidget::setFlashSeparator, checked);
  app->config().window(_curr_idx).appearance().setSeparatorFlashes(checked);
}

void SettingsDialog::on_scaling_x_edit_valueChanged(int arg1)
{
  SectionAppearance& acfg = app->config().window(_curr_idx).appearance();
  applyWindowOption(&ClockWindow::setScaling, arg1/100., acfg.getScalingV()/100.);
  acfg.setScalingH(arg1);
}

void SettingsDialog::on_scaling_y_edit_valueChanged(int arg1)
{
  SectionAppearance& acfg = app->config().window(_curr_idx).appearance();
  applyWindowOption(&ClockWindow::setScaling, acfg.getScalingH()/100., arg1/100.);
  acfg.setScalingV(arg1);
}

void SettingsDialog::on_scaling_same_btn_clicked(bool checked)
{
  if (checked) {
    ui->scaling_y_edit->setValue(ui->scaling_x_edit->value());
    connect(ui->scaling_x_edit, &QSpinBox::valueChanged, ui->scaling_y_edit, &QSpinBox::setValue);
    connect(ui->scaling_y_edit, &QSpinBox::valueChanged, ui->scaling_x_edit, &QSpinBox::setValue);
  } else {
    disconnect(ui->scaling_x_edit, &QSpinBox::valueChanged, ui->scaling_y_edit, &QSpinBox::setValue);
    disconnect(ui->scaling_y_edit, &QSpinBox::valueChanged, ui->scaling_x_edit, &QSpinBox::setValue);
  }
}

void SettingsDialog::on_opacity_edit_valueChanged(int arg1)
{
  applyWindowOption(&ClockWindow::setWindowOpacity, arg1/100.);
  app->config().window(_curr_idx).appearance().setOpacity(arg1);
}

void SettingsDialog::on_use_colorization_clicked(bool checked)
{
  if (checked) {
    applyColorization();
  } else {
    applyWindowOption(&ClockWindow::setGraphicsEffect, nullptr);
  }
  app->config().window(_curr_idx).appearance().setApplyColorization(checked);
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
}

void SettingsDialog::on_colorization_strength_edit_valueChanged(int arg1)
{
  app->config().window(_curr_idx).appearance().setColorizationStrength(arg1/100.);
  updateColorization();
}

void SettingsDialog::on_texture_group_clicked(bool checked)
{
  SettingsDialogState state(app->config().window(_curr_idx).state());

  QBrush brush(Qt::NoBrush);

  if (checked) {
    if (ui->tx_solid_color_rbtn->isChecked())
      brush = QBrush(state.getTextureColor());

    if (ui->tx_gradient_rbtn->isChecked())
      brush = QBrush(state.getTextureGradient());

    if (ui->tx_pattern_rbtn->isChecked())
      brush = QBrush(state.getTexturePattern());
  }

  app->config().window(_curr_idx).appearance().setTexture(brush);
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setTexture), brush);
}

void SettingsDialog::on_tx_solid_color_rbtn_clicked()
{
  SettingsDialogState state(app->config().window(_curr_idx).state());
  app->config().window(_curr_idx).appearance().setTexture(state.getTextureColor());
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setTexture), state.getTextureColor());
}

void SettingsDialog::on_tx_select_color_btn_clicked()
{
  SettingsDialogState state(app->config().window(_curr_idx).state());
  auto color = QColorDialog::getColor(state.getTextureColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;
  app->config().window(_curr_idx).appearance().setTexture(color);
  state.setTextureColor(color);
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setTexture), color);
}

void SettingsDialog::on_tx_gradient_rbtn_clicked()
{
  SettingsDialogState state(app->config().window(_curr_idx).state());
  app->config().window(_curr_idx).appearance().setTexture(state.getTextureGradient());
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setTexture), state.getTextureGradient());
}

void SettingsDialog::on_tx_select_gradient_btn_clicked()
{
  SettingsDialogState state(app->config().window(_curr_idx).state());
  bool ok = false;
  auto gradient = GradientDialog::getGradient(&ok,
                                              state.getTextureGradient(),
                                              this);
  if (!ok) return;
  gradient.setCoordinateMode(QGradient::ObjectMode);
  app->config().window(_curr_idx).appearance().setTexture(gradient);
  state.setTextureGradient(gradient);
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setTexture), gradient);
}

void SettingsDialog::on_tx_pattern_rbtn_clicked()
{
  SettingsDialogState state(app->config().window(_curr_idx).state());
  app->config().window(_curr_idx).appearance().setTexture(state.getTexturePattern());
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setTexture), state.getTexturePattern());
}

void SettingsDialog::on_tx_select_pattern_btn_clicked()
{
  SettingsDialogState state(app->config().window(_curr_idx).state());
  auto file = QFileDialog::getOpenFileName(this,
                                           QString(),
                                           _last_path,
                                           tr("Images (*.png *.bmp *.jpg)"));
  if (file.isEmpty()) return;
  _last_path = file;
  QPixmap pxm(file);
  app->config().window(_curr_idx).appearance().setTexture(pxm);
  state.setTexturePattern(pxm);
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setTexture), pxm);
}

void SettingsDialog::on_tx_pattern_stretch_clicked(bool checked)
{
  app->config().window(_curr_idx).appearance().setTextureStretch(checked);
  applyClockOption(&GraphicsDateTimeWidget::setTextureStretch, checked);
}

void SettingsDialog::on_tx_per_element_cb_clicked(bool checked)
{
  app->config().window(_curr_idx).appearance().setTexturePerCharacter(checked);
  applyClockOption(&GraphicsDateTimeWidget::setTexturePerChar, checked);
}

void SettingsDialog::on_background_group_clicked(bool checked)
{
  SettingsDialogState state(app->config().window(_curr_idx).state());

  QBrush brush(Qt::NoBrush);

  if (checked) {
    if (ui->bg_solid_color_rbtn->isChecked())
      brush = QBrush(state.getBackgroundColor());

    if (ui->bg_gradient_rbtn->isChecked())
      brush = QBrush(state.getBackgroundGradient());

    if (ui->bg_pattern_rbtn->isChecked())
      brush = QBrush(state.getBackgroundPattern());
  }

  app->config().window(_curr_idx).appearance().setBackground(brush);
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setBackground), brush);
}

void SettingsDialog::on_bg_solid_color_rbtn_clicked()
{
  SettingsDialogState state(app->config().window(_curr_idx).state());
  app->config().window(_curr_idx).appearance().setBackground(state.getBackgroundColor());
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setBackground), state.getBackgroundColor());
}

void SettingsDialog::on_bg_select_color_btn_clicked()
{
  SettingsDialogState state(app->config().window(_curr_idx).state());
  auto color = QColorDialog::getColor(state.getBackgroundColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;
  app->config().window(_curr_idx).appearance().setBackground(color);
  state.setBackgroundColor(color);
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setBackground), color);
}

void SettingsDialog::on_bg_gradient_rbtn_clicked()
{
  SettingsDialogState state(app->config().window(_curr_idx).state());
  app->config().window(_curr_idx).appearance().setBackground(state.getBackgroundGradient());
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setBackground), state.getBackgroundGradient());
}

void SettingsDialog::on_bg_select_gradient_btn_clicked()
{
  SettingsDialogState state(app->config().window(_curr_idx).state());
  bool ok = false;
  auto gradient = GradientDialog::getGradient(&ok,
                                              state.getBackgroundGradient(),
                                              this);
  if (!ok) return;
  gradient.setCoordinateMode(QGradient::ObjectMode);
  app->config().window(_curr_idx).appearance().setBackground(gradient);
  state.setBackgroundGradient(gradient);
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setBackground), gradient);
}

void SettingsDialog::on_bg_pattern_rbtn_clicked()
{
  SettingsDialogState state(app->config().window(_curr_idx).state());
  app->config().window(_curr_idx).appearance().setBackground(state.getBackgroundPattern());
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setBackground), state.getBackgroundPattern());
}

void SettingsDialog::on_bg_select_pattern_btn_clicked()
{
  SettingsDialogState state(app->config().window(_curr_idx).state());
  auto file = QFileDialog::getOpenFileName(this,
                                           QString(),
                                           _last_path,
                                           tr("Images (*.png *.bmp *.jpg)"));
  if (file.isEmpty()) return;
  _last_path = file;
  QPixmap pxm(file);
  app->config().window(_curr_idx).appearance().setBackground(pxm);
  state.setBackgroundPattern(pxm);
  applyClockOption(qOverload<QBrush>(&GraphicsDateTimeWidget::setBackground), pxm);
}

void SettingsDialog::on_bg_pattern_stretch_clicked(bool checked)
{
  app->config().window(_curr_idx).appearance().setBackgroundStretch(checked);
  applyClockOption(&GraphicsDateTimeWidget::setBackgroundStretch, checked);
}

void SettingsDialog::on_bg_per_element_cb_clicked(bool checked)
{
  app->config().window(_curr_idx).appearance().setBackgroundPerCharacter(checked);
  applyClockOption(&GraphicsDateTimeWidget::setBackgroundPerChar, checked);
}

void SettingsDialog::initAppGlobalTab()
{
  // app global
  SectionAppGlobal& gs = app->config().global();
#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
  // ui->enable_autostart->setChecked(IsAutoStartEnabled());
#else
  ui->enable_autostart->setVisible(false);  // not implemented
#endif
  ui->enable_stay_on_top->setChecked(gs.getStayOnTop());
  ui->enable_transp_for_input->setChecked(gs.getTransparentForMouse());

  ui->enable_multiwindow->setChecked(gs.getNumInstances() > 1);
  ui->wnd_count_edit->setValue(gs.getNumInstances());
  ui->use_same_appearance->setChecked(!gs.getAppearancePerInstance());

  // ui->transparent_on_hover->setChecked(impl->config.getChangeOpacityOnMouseHover());
  // ui->hide_on_mouse_hover->setChecked(qFuzzyIsNull(impl->config.getOpacityOnMouseHover()));

  ui->enable_autoupdate->setChecked(gs.getCheckForUpdates());
  ui->check_for_beta->setChecked(gs.getCheckForBetaVersion());

  QSignalBlocker _(ui->update_period_edit);
  ui->update_period_edit->addItem(tr("1 day"), 1);
  ui->update_period_edit->addItem(tr("1 week"), 7);
  ui->update_period_edit->addItem(tr("2 weeks"), 14);
  ui->update_period_edit->addItem(tr("1 month"), 30);
  setIndexByValue(ui->update_period_edit, gs.getUpdatePeriodDays());

  // ui->enable_debug_options->setChecked(impl->config.getEnableDebugOptions());
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
  ui->format_edit->setText(time_format);

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

  // ui->layout_cfg_edit->setText(impl->scfg->getLayoutConfig());

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

  const auto avail_skins = app->skinManager().availableSkins();
  for (const auto& s : avail_skins) {
    auto t = app->skinManager().metadata(s)["name"];
    ui->skin_cbox->addItem(t, s);
  }
  ui->skin_cbox->setCurrentIndex(-1);   // if skin is available, next line will update the index
  ui->skin_cbox->setCurrentText(app->skinManager().metadata(acfg.getSkin())["name"]);

  ui->is_separator_flashes->setChecked(acfg.getSeparatorFlashes());
  ui->scaling_x_edit->setValue(acfg.getScalingH());
  ui->scaling_y_edit->setValue(acfg.getScalingV());
  ui->opacity_edit->setValue(acfg.getOpacity());
  ui->use_colorization->setChecked(acfg.getApplyColorization());
  ui->colorization_strength_edit->setValue(qRound(acfg.getColorizationStrength() * 100));

  ui->scaling_same_btn->setChecked(ui->scaling_x_edit->value() == ui->scaling_y_edit->value());
  on_scaling_same_btn_clicked(ui->scaling_same_btn->isChecked());

  auto tx = acfg.getTexture();
  ui->texture_group->setChecked(tx.style() != Qt::NoBrush);
  ui->tx_solid_color_rbtn->setChecked(tx.style() == Qt::SolidPattern);
  ui->tx_gradient_rbtn->setChecked(tx.gradient() != nullptr);
  ui->tx_pattern_rbtn->setChecked(tx.style() == Qt::TexturePattern);
  ui->tx_pattern_stretch->setChecked(acfg.getTextureStretch());
  ui->tx_per_element_cb->setChecked(acfg.getTexturePerCharacter());

  auto bg = acfg.getBackground();
  ui->background_group->setChecked(bg.style() != Qt::NoBrush);
  ui->bg_solid_color_rbtn->setChecked(bg.style() == Qt::SolidPattern);
  ui->bg_gradient_rbtn->setChecked(bg.gradient() != nullptr);
  ui->bg_pattern_rbtn->setChecked(bg.style() == Qt::TexturePattern);
  ui->bg_pattern_stretch->setChecked(acfg.getBackgroundStretch());
  ui->bg_per_element_cb->setChecked(acfg.getBackgroundPerCharacter());
}

void SettingsDialog::initPluginsTab()
{
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
