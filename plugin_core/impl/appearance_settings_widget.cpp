/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
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

AppearanceSettingsWidget::AppearanceSettingsWidget(WidgetPluginBaseImpl* w, PluginSettingsStorage& s, StateClient& t, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AppearanceSettingsWidget)
    , impl(w)
    , widget(w->widget.get())
    , cfg(s)
    , st(t)
{
  ui->setupUi(this);

  ui->pos_bottom->setChecked(cfg.getWidgetPosition() == WidgetPosition::Bottom);
  ui->pos_right->setChecked(cfg.getWidgetPosition() == WidgetPosition::Right);

  ui->resize_auto->setChecked(cfg.getResizePolicy() == ResizePolicy::Autoscale);
  ui->resize_fixed->setChecked(cfg.getResizePolicy() == ResizePolicy::Fixed);

  ui->appear_follow->setChecked(cfg.getFollowClock());
  ui->appear_custom->setChecked(!cfg.getFollowClock());

  ui->font_rbtn->setChecked(!cfg.getUseClockSkin());
  ui->skin_rbtn->setChecked(cfg.getUseClockSkin());

  const auto& tx = cfg.getTexture();
  ui->tx_group->setChecked(tx.style() != Qt::NoBrush);
  if (tx.style() == Qt::SolidPattern) {
    ui->tx_options_box->setCurrentIndex(0);
  }
  if (tx.gradient() != nullptr) {
    ui->tx_options_box->setCurrentIndex(1);
  }
  if (tx.style() == Qt::TexturePattern) {
    ui->tx_options_box->setCurrentIndex(2);
    ui->tx_option->setChecked(cfg.getTextureStretch());
  }
  ui->tx_per_char->setChecked(cfg.getTexturePerCharacter());
  on_tx_options_box_currentIndexChanged(ui->tx_options_box->currentIndex());

  const auto& bg = cfg.getBackground();
  ui->bg_group->setChecked(bg.style() != Qt::NoBrush);
  if (bg.style() == Qt::SolidPattern) {
    ui->bg_options_box->setCurrentIndex(0);
  }
  if (bg.gradient() != nullptr) {
    ui->bg_options_box->setCurrentIndex(1);
  }
  if (bg.style() == Qt::TexturePattern) {
    ui->bg_options_box->setCurrentIndex(2);
    ui->bg_option->setChecked(cfg.getBackgroundStretch());
  }
  ui->bg_per_char->setChecked(cfg.getBackgroundPerCharacter());
  on_bg_options_box_currentIndexChanged(ui->bg_options_box->currentIndex());

  QSignalBlocker ah(ui->align_h_box);
  QSignalBlocker av(ui->align_v_box);

  ui->align_h_box->addItem(tr("left", "alignment"), Qt::AlignLeft);
  ui->align_h_box->addItem(tr("center", "alignment"), Qt::AlignHCenter);
  ui->align_h_box->addItem(tr("right", "alignment"), Qt::AlignRight);

  ui->align_v_box->addItem(tr("top", "alignment"), Qt::AlignTop);
  ui->align_v_box->addItem(tr("center", "alignment"), Qt::AlignVCenter);
  ui->align_v_box->addItem(tr("bottom", "alignment"), Qt::AlignBottom);

  int hidx = -1;
  auto halign = cfg.getAlignment() & Qt::AlignHorizontal_Mask;
  if (halign == Qt::AlignLeft)
    hidx = 0;
  if (halign == Qt::AlignHCenter)
    hidx = 1;
  if (halign == Qt::AlignRight)
    hidx = 2;

  int vidx = -1;
  auto valign = cfg.getAlignment() & Qt::AlignVertical_Mask;
  if (valign == Qt::AlignTop)
    vidx = 0;
  if (valign == Qt::AlignVCenter)
    vidx = 1;
  if (valign == Qt::AlignBottom)
    vidx = 2;

  ui->align_h_box->setCurrentIndex(hidx);
  ui->align_v_box->setCurrentIndex(vidx);

  ui->layout_cfg_edit->setText(cfg.getLayoutConfig());
}

AppearanceSettingsWidget::~AppearanceSettingsWidget()
{
  delete ui;
}

void AppearanceSettingsWidget::on_pos_bottom_clicked()
{
  cfg.setWidgetPosition(WidgetPosition::Bottom);
  impl->position = WidgetPosition::Bottom;
  impl->repositionWidget();
}


void AppearanceSettingsWidget::on_pos_right_clicked()
{
  cfg.setWidgetPosition(WidgetPosition::Right);
  impl->position = WidgetPosition::Right;
  impl->repositionWidget();
}


void AppearanceSettingsWidget::on_resize_auto_clicked()
{
  if (widget) {
    widget->setAutoResizePolicy(cfg.getWidgetPosition() == WidgetPosition::Bottom ?
                                    GraphicsWidgetBase::KeepWidth :
                                    GraphicsWidgetBase::KeepHeight);
    impl->layout->setAlignment(widget, Qt::Alignment());
  }
  impl->resize_policy = ResizePolicy::Autoscale;
  cfg.setResizePolicy(ResizePolicy::Autoscale);
}

void AppearanceSettingsWidget::on_resize_fixed_clicked()
{
  if (widget) {
    widget->setAutoResizePolicy(GraphicsWidgetBase::None);
    impl->layout->setAlignment(widget, cfg.getAlignment());
  }
  impl->resize_policy = ResizePolicy::Fixed;
  cfg.setResizePolicy(ResizePolicy::Fixed);
}

void AppearanceSettingsWidget::on_appear_follow_clicked()
{
  cfg.setFollowClock(true);
  impl->appearance = WidgetAppearance::FollowClock;
  impl->applyAppearanceSettings();
}

void AppearanceSettingsWidget::on_appear_custom_clicked()
{
  cfg.setFollowClock(false);
  impl->appearance = WidgetAppearance::Custom;
  impl->applyAppearanceSettings();
}

void AppearanceSettingsWidget::on_align_h_box_activated(int index)
{
  auto halign = ui->align_h_box->itemData(index).value<Qt::Alignment>();
  cfg.setAlignment((cfg.getAlignment() & Qt::AlignVertical_Mask) | halign);
  impl->content_alignment = cfg.getAlignment();

  if (impl->layout && widget)
    impl->layout->setAlignment(widget, cfg.getAlignment());
}

void AppearanceSettingsWidget::on_align_v_box_activated(int index)
{
  auto valign = ui->align_h_box->itemData(index).value<Qt::Alignment>();
  cfg.setAlignment((cfg.getAlignment() & Qt::AlignHorizontal_Mask) | valign);
  impl->content_alignment = cfg.getAlignment();

  if (impl->layout && widget)
    impl->layout->setAlignment(widget, cfg.getAlignment());
}

void AppearanceSettingsWidget::on_layout_cfg_edit_textEdited(const QString& arg1)
{
  if (widget)
    widget->setLayoutConfig(arg1);
  impl->layout_cfg = arg1;
  cfg.setLayoutConfig(arg1);
}

void AppearanceSettingsWidget::on_layout_cfg_help_btn_clicked()
{
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClock5/wiki/Multiple-lines"));
}

void AppearanceSettingsWidget::on_font_rbtn_clicked()
{
  if (widget)
    widget->setSkin(impl->custom_skin);
  impl->use_clock_skin = false;
  cfg.setUseClockSkin(false);
}

void AppearanceSettingsWidget::on_font_select_clicked()
{
  bool ok = false;
  QFont fnt = QFontDialog::getFont(&ok, cfg.getCustomFont(), this);
  if (!ok) return;

  cfg.setCustomFont(fnt);
  impl->custom_skin = std::make_shared<FontSkin>(fnt);

  if (widget)
    widget->setSkin(impl->custom_skin);
}

void AppearanceSettingsWidget::on_skin_rbtn_clicked()
{
  if (widget)
    widget->setSkin(impl->skin);
  impl->use_clock_skin = true;
  cfg.setUseClockSkin(true);
}

void AppearanceSettingsWidget::on_tx_group_clicked(bool checked)
{
  if (checked) {
    if (ui->tx_options_box->currentIndex() == 0)
      cfg.setTextureType(AppearanceSectionBase::SolidColor);

    if (ui->tx_options_box->currentIndex() == 1)
      cfg.setTextureType(AppearanceSectionBase::Gradient);

    if (ui->tx_options_box->currentIndex() == 2)
      cfg.setTextureType(AppearanceSectionBase::Pattern);
  } else {
    cfg.setTextureType(AppearanceSectionBase::None);
  }

  impl->tx = cfg.getTexture();

  if (widget)
    widget->setTexture(cfg.getTexture());
}

void AppearanceSettingsWidget::on_tx_options_box_activated(int index)
{
  switch (index) {
    case 0: // color
      cfg.setTextureType(AppearanceSectionBase::SolidColor);
      break;
    case 1: // gradient
      cfg.setTextureType(AppearanceSectionBase::Gradient);
      break;
    case 2: // pattern
      cfg.setTextureType(AppearanceSectionBase::Pattern);
      break;
  }

  impl->tx = cfg.getTexture();

  if (widget)
    widget->setTexture(cfg.getTexture());
}

void AppearanceSettingsWidget::on_tx_options_box_currentIndexChanged(int index)
{
  disconnect(ui->tx_btn, &QToolButton::clicked, nullptr, nullptr);
  disconnect(ui->tx_option, &QCheckBox::clicked, nullptr, nullptr);
  ui->tx_option->hide();

  switch (index) {
    case 0: // color
      connect(ui->tx_btn, &QToolButton::clicked, this, &AppearanceSettingsWidget::tx_select_color);
      ui->tx_option->setText(tr("follow system theme"));
      // ui->tx_option->show();
      // connect(ui->tx_option, &QCheckBox::clicked, this, &AppearanceSettingsWidget::applySystemColor);
      break;
    case 1: // gradient
      connect(ui->tx_btn, &QToolButton::clicked, this, &AppearanceSettingsWidget::tx_select_gradient);
      ui->tx_option->hide();
      break;
    case 2: // pattern
      connect(ui->tx_btn, &QToolButton::clicked, this, &AppearanceSettingsWidget::tx_select_pattern);
      ui->tx_option->setText(tr("stretch instead of tile"));
      ui->tx_option->show();
      connect(ui->tx_option, &QCheckBox::clicked, this, &AppearanceSettingsWidget::tx_pattern_stretch);
      break;
  }
}

void AppearanceSettingsWidget::tx_select_color()
{
  auto color = QColorDialog::getColor(cfg.getTextureColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;

  cfg.setTextureColor(color);
  impl->tx = color;

  if (widget)
    widget->setTexture(std::move(color));
}

void AppearanceSettingsWidget::tx_select_gradient()
{
  bool ok = false;
  auto gradient = GradientDialog::getGradient(&ok,
                                              cfg.getTextureGradient(),
                                              this);
  if (!ok) return;

  gradient.setCoordinateMode(QGradient::ObjectMode);
  cfg.setTextureGradient(gradient);
  impl->tx = gradient;

  if (widget)
    widget->setTexture(std::move(gradient));
}

void AppearanceSettingsWidget::tx_select_pattern()
{
  auto file = QFileDialog::getOpenFileName(this,
                                           QString(),
                                           cfg.getTexturePatternFile(),
                                           tr("Images (*.png *.bmp *.jpg)"));
  if (file.isEmpty()) return;

  QPixmap pxm(file);
  cfg.setTexturePatternFile(file);
  impl->tx = pxm;

  if (widget)
    widget->setTexture(std::move(pxm));
}

void AppearanceSettingsWidget::tx_pattern_stretch(bool checked)
{
  cfg.setTextureStretch(checked);
  impl->tx_stretch = checked;

  if (widget)
    widget->setTextureStretch(checked);
}

void AppearanceSettingsWidget::on_tx_per_char_clicked(bool checked)
{
  cfg.setTexturePerCharacter(checked);
  impl->tx_per_char = checked;

  if (widget)
    widget->setTexturePerChar(checked);
}

void AppearanceSettingsWidget::on_bg_group_clicked(bool checked)
{
  if (checked) {
    if (ui->bg_options_box->currentIndex() == 0)
      cfg.setBackgroundType(AppearanceSectionBase::SolidColor);

    if (ui->bg_options_box->currentIndex() == 1)
      cfg.setBackgroundType(AppearanceSectionBase::Gradient);

    if (ui->bg_options_box->currentIndex() == 2)
      cfg.setBackgroundType(AppearanceSectionBase::Pattern);
  } else {
    cfg.setBackgroundType(AppearanceSectionBase::None);
  }

  impl->bg = cfg.getBackground();

  if (widget)
    widget->setBackground(cfg.getBackground());
}

void AppearanceSettingsWidget::on_bg_options_box_activated(int index)
{
  switch (index) {
    case 0: // color
      cfg.setBackgroundType(AppearanceSectionBase::SolidColor);
      break;
    case 1: // gradient
      cfg.setBackgroundType(AppearanceSectionBase::Gradient);
      break;
    case 2: // pattern
      cfg.setBackgroundType(AppearanceSectionBase::Pattern);
      break;
  }

  impl->bg = cfg.getBackground();

  if (widget)
    widget->setBackground(cfg.getBackground());
}

void AppearanceSettingsWidget::on_bg_options_box_currentIndexChanged(int index)
{
  disconnect(ui->bg_btn, &QToolButton::clicked, nullptr, nullptr);
  disconnect(ui->bg_option, &QCheckBox::clicked, nullptr, nullptr);
  ui->bg_option->hide();

  switch (index) {
    case 0: // color
      connect(ui->bg_btn, &QToolButton::clicked, this, &AppearanceSettingsWidget::bg_select_color);
      ui->bg_option->setText(tr("follow system theme"));
      // ui->bg_option->show();
      // connect(ui->bg_option, &QCheckBox::clicked, this, &AppearanceSettingsWidget::applySystemColor);
      break;
    case 1: // gradient
      connect(ui->bg_btn, &QToolButton::clicked, this, &AppearanceSettingsWidget::bg_select_gradient);
      ui->bg_option->hide();
      break;
    case 2: // pattern
      connect(ui->bg_btn, &QToolButton::clicked, this, &AppearanceSettingsWidget::bg_select_pattern);
      ui->bg_option->setText(tr("stretch instead of tile"));
      ui->bg_option->show();
      connect(ui->bg_option, &QCheckBox::clicked, this, &AppearanceSettingsWidget::bg_pattern_stretch);
      break;
  }
}

void AppearanceSettingsWidget::bg_select_color()
{
  auto color = QColorDialog::getColor(cfg.getBackgroundColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;

  cfg.setBackgroundColor(color);
  impl->bg = color;

  if (widget)
    widget->setBackground(std::move(color));
}

void AppearanceSettingsWidget::bg_select_gradient()
{
  bool ok = false;
  auto gradient = GradientDialog::getGradient(&ok,
                                              cfg.getBackgroundGradient(),
                                              this);
  if (!ok) return;

  gradient.setCoordinateMode(QGradient::ObjectMode);
  cfg.setBackgroundGradient(gradient);
  impl->bg = gradient;

  if (widget)
    widget->setBackground(std::move(gradient));
}

void AppearanceSettingsWidget::bg_select_pattern()
{
  auto file = QFileDialog::getOpenFileName(this,
                                           QString(),
                                           cfg.getBackgroundPatternFile(),
                                           tr("Images (*.png *.bmp *.jpg)"));
  if (file.isEmpty()) return;

  QPixmap pxm(file);
  cfg.setBackgroundPatternFile(file);
  impl->bg = pxm;

  if (widget)
    widget->setBackground(std::move(pxm));
}

void AppearanceSettingsWidget::bg_pattern_stretch(bool checked)
{
  cfg.setBackgroundStretch(checked);
  impl->bg_stretch = checked;

  if (widget)
    widget->setBackgroundStretch(checked);
}

void AppearanceSettingsWidget::on_bg_per_char_clicked(bool checked)
{
  cfg.setBackgroundPerCharacter(checked);
  impl->bg_per_char = checked;

  if (widget)
    widget->setBackgroundPerChar(checked);
}
