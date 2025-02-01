/*
 * SPDX-FileCopyrightText: 2024-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "appearance_settings_widget.hpp"
#include "ui_appearance_settings_widget.h"

#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>

#include <QDesktopServices>

#include <gradient_dialog.h>

namespace {

int setIndexByValue(QComboBox* box, const QVariant& value)
{
  auto idx = box->findData(value);
  if (idx >= 0)
    box->setCurrentIndex(idx);
  return idx;
}

} // namespace

namespace plugin::text::impl {

AppearanceSettingsWidget::AppearanceSettingsWidget(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::AppearanceSettingsWidget)
{
  ui->setupUi(this);
  ui->layout_cfg_help_btn->setIcon(QIcon(":/icons/help-hint.svg"));

  QSignalBlocker _(this);
  fillAlignmentList();

  fillTextureTypes(ui->tx_options_box);
  fillTextureTypes(ui->bg_options_box);
}

AppearanceSettingsWidget::~AppearanceSettingsWidget()
{
  delete ui;
}

void AppearanceSettingsWidget::initControls(PluginInstanceConfig* icfg)
{
  Q_ASSERT(icfg);
  cfg = icfg;

  QSignalBlocker _(this);

  ui->pos_bottom->setChecked(icfg->getWidgetPosition() == WidgetPosition::Bottom);
  ui->pos_right->setChecked(icfg->getWidgetPosition() == WidgetPosition::Right);

  ui->resize_auto->setChecked(icfg->getAutoscaleEnabled());
  ui->resize_fixed->setChecked(!icfg->getAutoscaleEnabled());

  ui->appear_follow->setChecked(icfg->getFollowClockAppearance());
  ui->appear_custom->setChecked(!icfg->getFollowClockAppearance());

  ui->font_rbtn->setChecked(!icfg->getUseClockSkin());
  ui->skin_rbtn->setChecked(icfg->getUseClockSkin());

  ui->tx_group->setChecked(cfg->getTextureType() != tx::None);
  if (cfg->getTextureType() != tx::None)
    setIndexByValue(ui->tx_options_box, QVariant::fromValue(cfg->getTextureType()));
  ui->tx_per_char->setChecked(cfg->getTextureCustomization() == LinesRenderer::PerChar);
  on_tx_options_box_currentIndexChanged(ui->tx_options_box->currentIndex());

  ui->bg_group->setChecked(cfg->getBackgroundType() != tx::None);
  if (cfg->getBackgroundType() != tx::None)
    setIndexByValue(ui->bg_options_box, QVariant::fromValue(cfg->getBackgroundType()));
  ui->bg_per_char->setChecked(cfg->getBackgroundCustomization() == LinesRenderer::PerChar);
  on_bg_options_box_currentIndexChanged(ui->bg_options_box->currentIndex());

  auto halign = cfg->getWidgetAlignment() & Qt::AlignHorizontal_Mask;
  setIndexByValue(ui->align_h_box, QVariant::fromValue(halign));

  auto valign = cfg->getWidgetAlignment() & Qt::AlignVertical_Mask;
  setIndexByValue(ui->align_v_box, QVariant::fromValue(valign));

  ui->layout_cfg_edit->setText(cfg->getLayoutConfig());
}

void AppearanceSettingsWidget::on_pos_bottom_clicked()
{
  cfg->setWidgetPosition(WidgetPosition::Bottom);
  emit pluginOptionChanged(wpo::WidgetPosition, QVariant::fromValue(WidgetPosition::Bottom));
}

void AppearanceSettingsWidget::on_pos_right_clicked()
{
  cfg->setWidgetPosition(WidgetPosition::Right);
  emit pluginOptionChanged(wpo::WidgetPosition, QVariant::fromValue(WidgetPosition::Right));
}

void AppearanceSettingsWidget::on_resize_auto_clicked()
{
  cfg->setAutoscaleEnabled(true);
  emit pluginOptionChanged(wpo::AutoscaleEnabled, true);
}

void AppearanceSettingsWidget::on_resize_fixed_clicked()
{
  cfg->setAutoscaleEnabled(false);
  emit pluginOptionChanged(wpo::AutoscaleEnabled, false);
}

void AppearanceSettingsWidget::on_appear_follow_clicked()
{
  cfg->setFollowClockAppearance(true);
  emit pluginOptionChanged(wpo::FollowClockAppearance, true);
}

void AppearanceSettingsWidget::on_appear_custom_clicked()
{
  cfg->setFollowClockAppearance(false);
  emit pluginOptionChanged(wpo::FollowClockAppearance, false);
}

void AppearanceSettingsWidget::on_align_h_box_activated(int index)
{
  auto halign = ui->align_h_box->itemData(index).value<Qt::Alignment>();
  cfg->setWidgetAlignment((cfg->getWidgetAlignment() & Qt::AlignVertical_Mask) | halign);
  emit pluginOptionChanged(wpo::WidgetAlignment, QVariant::fromValue(cfg->getWidgetAlignment()));
}

void AppearanceSettingsWidget::on_align_v_box_activated(int index)
{
  auto valign = ui->align_h_box->itemData(index).value<Qt::Alignment>();
  cfg->setWidgetAlignment((cfg->getWidgetAlignment() & Qt::AlignHorizontal_Mask) | valign);
  emit pluginOptionChanged(wpo::WidgetAlignment, QVariant::fromValue(cfg->getWidgetAlignment()));
}

void AppearanceSettingsWidget::on_layout_cfg_edit_textEdited(const QString& arg1)
{
  cfg->setLayoutConfig(arg1);
  emit sharedOptionChanged(opt::LayoutConfig, arg1);
}

void AppearanceSettingsWidget::on_layout_cfg_help_btn_clicked()
{
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClock5/wiki/Multiple-lines"));
}

void AppearanceSettingsWidget::on_font_rbtn_clicked()
{
  cfg->setUseClockSkin(false);
  cfg->setUseSkin(false);
  emit pluginOptionChanged(wpo::UseClockSkin, false);
  emit sharedOptionChanged(opt::UseSkin, false);
}

void AppearanceSettingsWidget::on_font_select_clicked()
{
  bool ok = false;
  QFont fnt = QFontDialog::getFont(&ok, cfg->getFont(), this);
  if (!ok) return;

  cfg->setFont(fnt);
  emit sharedOptionChanged(opt::Font, fnt);
}

void AppearanceSettingsWidget::on_skin_rbtn_clicked()
{
  cfg->setUseClockSkin(true);
  emit pluginOptionChanged(wpo::UseClockSkin, true);
}

void AppearanceSettingsWidget::on_tx_group_clicked(bool checked)
{
  auto tx_tt = checked ? ui->tx_options_box->currentData().value<tx::TextureType>() : tx::None;
  cfg->setTextureType(tx_tt);
  emit sharedOptionChanged(opt::TextureType, tx_tt);
}

void AppearanceSettingsWidget::on_tx_options_box_activated(int index)
{
  auto tx_tt = ui->tx_options_box->itemData(index).value<tx::TextureType>();
  cfg->setTextureType(tx_tt);
  emit sharedOptionChanged(opt::TextureType, tx_tt);
}

void AppearanceSettingsWidget::on_tx_options_box_currentIndexChanged(int index)
{
  disconnect(ui->tx_btn, &QToolButton::clicked, nullptr, nullptr);
  disconnect(ui->tx_option, &QCheckBox::clicked, nullptr, nullptr);
  ui->tx_option->hide();

  switch (ui->tx_options_box->itemData(index).value<tx::TextureType>()) {
    case tx::None:
      // do nothing, just to make clang happy...
      break;
    case tx::Color:
      connect(ui->tx_btn, &QToolButton::clicked, this, &AppearanceSettingsWidget::tx_select_color);
      ui->tx_option->setText(tr("follow system theme"));
      ui->tx_option->show();
      ui->tx_option->setChecked(cfg->getTextureColorUseTheme());
      ui->tx_btn->setDisabled(cfg->getTextureColorUseTheme());
      connect(ui->tx_option, &QCheckBox::toggled, ui->tx_btn, &QWidget::setDisabled);
      connect(ui->tx_option, &QCheckBox::clicked, this, &AppearanceSettingsWidget::tx_use_system_color);
      break;
    case tx::Gradient:
      connect(ui->tx_btn, &QToolButton::clicked, this, &AppearanceSettingsWidget::tx_select_gradient);
      ui->tx_option->hide();
      ui->tx_btn->setEnabled(true);
      break;
    case tx::Pattern:
      connect(ui->tx_btn, &QToolButton::clicked, this, &AppearanceSettingsWidget::tx_select_pattern);
      ui->tx_option->setText(tr("stretch instead of tile"));
      ui->tx_option->show();
      ui->tx_option->setChecked(!cfg->getTexturePatternTile());
      ui->tx_btn->setEnabled(true);
      connect(ui->tx_option, &QCheckBox::clicked, this, &AppearanceSettingsWidget::tx_pattern_stretch);
      break;
  }
}

void AppearanceSettingsWidget::tx_use_system_color(bool use)
{
  cfg->setTextureColorUseTheme(use);
  emit sharedOptionChanged(opt::TextureColorUseTheme, use);
}

void AppearanceSettingsWidget::tx_select_color()
{
  auto color = QColorDialog::getColor(cfg->getTextureColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;

  cfg->setTextureColor(color);
  emit sharedOptionChanged(opt::TextureColor, color);
}

void AppearanceSettingsWidget::tx_select_gradient()
{
  bool ok = false;
  // *INDENT-OFF*
  auto gradient = GradientDialog::getGradient(&ok,
                                              cfg->getTextureGradient(),
                                              this);
  // *INDENT-ON*
  if (!ok) return;

  gradient.setCoordinateMode(QGradient::ObjectMode);
  cfg->setTextureGradient(gradient);
  emit sharedOptionChanged(opt::TextureGradient, QVariant::fromValue(gradient));
}

void AppearanceSettingsWidget::tx_select_pattern()
{
  // *INDENT-OFF*
  auto file = QFileDialog::getOpenFileName(this,
                                           QString(),
                                           cfg->getTexturePattern(),
                                           tr("Images (*.png *.bmp *.jpg)"));
  // *INDENT-ON*
  if (file.isEmpty()) return;

  cfg->setTexturePattern(file);
  emit sharedOptionChanged(opt::TexturePattern, file);
}

void AppearanceSettingsWidget::tx_pattern_stretch(bool checked)
{
  cfg->setTexturePatternTile(!checked);
  emit sharedOptionChanged(opt::TexturePatternTile, !checked);
}

void AppearanceSettingsWidget::on_tx_per_char_clicked(bool checked)
{
  auto tx_ct = checked ? LinesRenderer::PerChar : LinesRenderer::AllText;
  cfg->setTextureCustomization(tx_ct);
  emit sharedOptionChanged(opt::TextureCustomization, tx_ct);
}

void AppearanceSettingsWidget::on_bg_group_clicked(bool checked)
{
  auto bg_tt = checked ? ui->bg_options_box->currentData().value<tx::TextureType>() : tx::None;
  cfg->setBackgroundType(bg_tt);
  emit sharedOptionChanged(opt::BackgroundType, bg_tt);
}

void AppearanceSettingsWidget::on_bg_options_box_activated(int index)
{
  auto bg_tt = ui->bg_options_box->itemData(index).value<tx::TextureType>();
  cfg->setBackgroundType(bg_tt);
  emit sharedOptionChanged(opt::BackgroundType, bg_tt);
}

void AppearanceSettingsWidget::on_bg_options_box_currentIndexChanged(int index)
{
  disconnect(ui->bg_btn, &QToolButton::clicked, nullptr, nullptr);
  disconnect(ui->bg_option, &QCheckBox::clicked, nullptr, nullptr);
  ui->bg_option->hide();

  switch (ui->bg_options_box->itemData(index).value<tx::TextureType>()) {
    case tx::None:
      // do nothing, just to make clang happy...
      break;
    case tx::Color:
      connect(ui->bg_btn, &QToolButton::clicked, this, &AppearanceSettingsWidget::bg_select_color);
      ui->bg_option->setText(tr("follow system theme"));
      ui->bg_option->show();
      ui->bg_option->setChecked(cfg->getBackgroundColorUseTheme());
      ui->bg_btn->setDisabled(cfg->getBackgroundColorUseTheme());
      connect(ui->bg_option, &QCheckBox::toggled, ui->bg_btn, &QWidget::setDisabled);
      connect(ui->bg_option, &QCheckBox::clicked, this, &AppearanceSettingsWidget::bg_use_system_color);
      break;
    case tx::Gradient:
      connect(ui->bg_btn, &QToolButton::clicked, this, &AppearanceSettingsWidget::bg_select_gradient);
      ui->bg_option->hide();
      ui->bg_btn->setEnabled(true);
      break;
    case tx::Pattern:
      connect(ui->bg_btn, &QToolButton::clicked, this, &AppearanceSettingsWidget::bg_select_pattern);
      ui->bg_option->setText(tr("stretch instead of tile"));
      ui->bg_option->show();
      ui->bg_option->setChecked(!cfg->getBackgroundPatternTile());
      ui->bg_btn->setEnabled(true);
      connect(ui->bg_option, &QCheckBox::clicked, this, &AppearanceSettingsWidget::bg_pattern_stretch);
      break;
  }
}

void AppearanceSettingsWidget::bg_use_system_color(bool use)
{
  cfg->setBackgroundColorUseTheme(use);
  emit sharedOptionChanged(opt::BackgroundColorUseTheme, use);
}

void AppearanceSettingsWidget::bg_select_color()
{
  auto color = QColorDialog::getColor(cfg->getBackgroundColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;

  cfg->setBackgroundColor(color);
  emit sharedOptionChanged(opt::BackgroundColor, color);
}

void AppearanceSettingsWidget::bg_select_gradient()
{
  bool ok = false;
  // *INDENT-OFF*
  auto gradient = GradientDialog::getGradient(&ok,
                                              cfg->getBackgroundGradient(),
                                              this);
  // *INDENT-ON*
  if (!ok) return;

  gradient.setCoordinateMode(QGradient::ObjectMode);
  cfg->setBackgroundGradient(gradient);
  emit sharedOptionChanged(opt::BackgroundGradient, QVariant::fromValue(gradient));
}

void AppearanceSettingsWidget::bg_select_pattern()
{
  // *INDENT-OFF*
  auto file = QFileDialog::getOpenFileName(this,
                                           QString(),
                                           cfg->getBackgroundPattern(),
                                           tr("Images (*.png *.bmp *.jpg)"));
  // *INDENT-ON*
  if (file.isEmpty()) return;

  cfg->setBackgroundPattern(file);
  emit sharedOptionChanged(opt::BackgroundPattern, file);
}

void AppearanceSettingsWidget::bg_pattern_stretch(bool checked)
{
  cfg->setBackgroundPatternTile(!checked);
  emit sharedOptionChanged(opt::BackgroundPatternTile, !checked);
}

void AppearanceSettingsWidget::on_bg_per_char_clicked(bool checked)
{
  auto bg_ct = checked ? LinesRenderer::PerChar : LinesRenderer::AllText;
  cfg->setBackgroundCustomization(bg_ct);
  emit sharedOptionChanged(opt::BackgroundCustomization, bg_ct);
}

void AppearanceSettingsWidget::fillAlignmentList()
{
  ui->align_h_box->addItem(tr("left", "alignment"), Qt::AlignLeft);
  ui->align_h_box->addItem(tr("center", "alignment"), Qt::AlignHCenter);
  ui->align_h_box->addItem(tr("right", "alignment"), Qt::AlignRight);

  ui->align_v_box->addItem(tr("top", "alignment"), Qt::AlignTop);
  ui->align_v_box->addItem(tr("center", "alignment"), Qt::AlignVCenter);
  ui->align_v_box->addItem(tr("bottom", "alignment"), Qt::AlignBottom);
}

void AppearanceSettingsWidget::fillTextureTypes(QComboBox* box)
{
  QSignalBlocker _(box);
  box->addItem(tr("solid color"), QVariant::fromValue(tx::Color));
  box->addItem(tr("gradient"), QVariant::fromValue(tx::Gradient));
  box->addItem(tr("pattern"), QVariant::fromValue(tx::Pattern));
}

} // namespace plugin::text::impl
