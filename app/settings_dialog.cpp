/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"

#include <QDesktopServices>

#include "application.hpp"

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

SettingsDialog::SettingsDialog(Application* app, QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
  , app(app)
  , _curr_idx(0)
{
  ui->setupUi(this);

  // TODO: fill windows instances and set current
  ui->windows_box->setVisible(app->config().global().getNumInstances() > 1);

  initAppGlobalTab();
  // TODO: pass index to constructor
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
  // qreal ssf = arg1 / 100.;
  // impl->skin->setTokenTransform("ss", QTransform::fromScale(ssf, ssf));
  // impl->scfg->setSecondsScaleFactor(arg1);
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
  app->window(_curr_idx)->clock()->setFormat(time_format);
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

void SettingsDialog::on_custom_seps_edit_textEdited(const QString& arg1)
{
  app->window(_curr_idx)->clock()->setCustomSeparators(arg1);
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

  // ui->smaller_seconds->setChecked(impl->scfg->getSecondsScaleFactor() != 100);
  // if (ui->smaller_seconds->isChecked())
  //   ui->seconds_scale_factor_edit->setValue(impl->scfg->getSecondsScaleFactor());

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

  // ui->custom_seps_label->setEnabled(impl->skin->supportsCustomSeparator());
  // ui->custom_seps_edit->setEnabled(impl->skin->supportsCustomSeparator());
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
