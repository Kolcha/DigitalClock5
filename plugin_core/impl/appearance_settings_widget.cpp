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

#include "common_appearance_state.hpp"

AppearanceSettingsWidget::AppearanceSettingsWidget(
    WidgetPluginBaseImpl* w, SettingsClient& s, StateClient& t, QWidget* parent)
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
  ui->tx_per_char->setChecked(cfg.getTexturePerChar());
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
  ui->bg_per_char->setChecked(cfg.getBackgroundPerChar());
  on_bg_options_box_currentIndexChanged(ui->bg_options_box->currentIndex());

  QSignalBlocker ah(ui->align_h_box);
  QSignalBlocker av(ui->align_v_box);

  ui->align_h_box->addItem(tr("left"), Qt::AlignLeft);
  ui->align_h_box->addItem(tr("center"), Qt::AlignHCenter);
  ui->align_h_box->addItem(tr("right"), Qt::AlignRight);

  ui->align_v_box->addItem(tr("top"), Qt::AlignTop);
  ui->align_v_box->addItem(tr("center"), Qt::AlignVCenter);
  ui->align_v_box->addItem(tr("bottom"), Qt::AlignBottom);

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
  QDesktopServices::openUrl(QUrl("https://github.com/Kolcha/DigitalClockNext/wiki/Multiple-lines"));
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
  CommonAppearanceState state(st);

  QBrush brush(Qt::NoBrush);

  if (checked) {
    if (ui->tx_options_box->currentIndex() == 0)
      brush = QBrush(state.getTextureColor());

    if (ui->tx_options_box->currentIndex() == 1)
      brush = QBrush(state.getTextureGradient());

    if (ui->tx_options_box->currentIndex() == 2)
      brush = QBrush(state.getTexturePattern());
  }

  cfg.setTexture(brush);
  impl->tx = brush;

  if (widget)
    widget->setTexture(std::move(brush));
}

void AppearanceSettingsWidget::on_tx_options_box_activated(int index)
{
  CommonAppearanceState state(st);
  switch (index) {
    case 0: // color
      cfg.setTexture(state.getTextureColor());
      impl->tx = state.getTextureColor();
      if (widget)
        widget->setTexture(state.getTextureColor());
      break;
    case 1: // gradient
      cfg.setTexture(state.getTextureGradient());
      impl->tx = state.getTextureGradient();
      if (widget)
        widget->setTexture(state.getTextureGradient());
      break;
    case 2: // pattern
      cfg.setTexture(state.getTexturePattern());
      impl->tx = state.getTexturePattern();
      if (widget)
        widget->setTexture(state.getTexturePattern());
      break;
  }
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
  CommonAppearanceState state(st);
  auto color = QColorDialog::getColor(state.getTextureColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;

  cfg.setTexture(color);
  state.setTextureColor(color);
  impl->tx = color;

  if (widget)
    widget->setTexture(std::move(color));
}

void AppearanceSettingsWidget::tx_select_gradient()
{
  CommonAppearanceState state(st);
  bool ok = false;
  auto gradient = GradientDialog::getGradient(&ok,
                                              state.getTextureGradient(),
                                              this);
  if (!ok) return;

  gradient.setCoordinateMode(QGradient::ObjectMode);
  cfg.setTexture(gradient);
  state.setTextureGradient(gradient);
  impl->tx = gradient;

  if (widget)
    widget->setTexture(std::move(gradient));
}

void AppearanceSettingsWidget::tx_select_pattern()
{
  CommonAppearanceState state(st);
  auto file = QFileDialog::getOpenFileName(this,
                                           QString(),
                                           _last_path,
                                           tr("Images (*.png *.bmp *.jpg)"));
  if (file.isEmpty()) return;

  _last_path = file;

  QPixmap pxm(file);
  cfg.setTexture(pxm);
  state.setTexturePattern(pxm);
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
  cfg.setTexturePerChar(checked);
  impl->tx_per_char = checked;

  if (widget)
    widget->setTexturePerChar(checked);
}

void AppearanceSettingsWidget::on_bg_group_clicked(bool checked)
{
  CommonAppearanceState state(st);

  QBrush brush(Qt::NoBrush);

  if (checked) {
    if (ui->bg_options_box->currentIndex() == 0)
      brush = QBrush(state.getBackgroundColor());

    if (ui->bg_options_box->currentIndex() == 1)
      brush = QBrush(state.getBackgroundGradient());

    if (ui->bg_options_box->currentIndex() == 2)
      brush = QBrush(state.getBackgroundPattern());
  }

  cfg.setBackground(brush);
  impl->bg = brush;

  if (widget)
    widget->setBackground(std::move(brush));
}

void AppearanceSettingsWidget::on_bg_options_box_activated(int index)
{
  CommonAppearanceState state(st);
  switch (index) {
    case 0: // color
      cfg.setBackground(state.getBackgroundColor());
      impl->bg = state.getBackgroundColor();
      if (widget)
        widget->setBackground(state.getBackgroundColor());
      break;
    case 1: // gradient
      cfg.setBackground(state.getBackgroundGradient());
      impl->bg = state.getBackgroundGradient();
      if (widget)
        widget->setBackground(state.getBackgroundGradient());
      break;
    case 2: // pattern
      cfg.setBackground(state.getBackgroundPattern());
      impl->bg = state.getBackgroundPattern();
      if (widget)
        widget->setBackground(state.getBackgroundPattern());
      break;
  }
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
  CommonAppearanceState state(st);
  auto color = QColorDialog::getColor(state.getBackgroundColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;

  cfg.setBackground(color);
  state.setBackgroundColor(color);
  impl->bg = color;

  if (widget)
    widget->setBackground(std::move(color));
}

void AppearanceSettingsWidget::bg_select_gradient()
{
  CommonAppearanceState state(st);
  bool ok = false;
  auto gradient = GradientDialog::getGradient(&ok,
                                              state.getBackgroundGradient(),
                                              this);
  if (!ok) return;

  gradient.setCoordinateMode(QGradient::ObjectMode);
  cfg.setBackground(gradient);
  state.setBackgroundGradient(gradient);
  impl->bg = gradient;

  if (widget)
    widget->setBackground(std::move(gradient));
}

void AppearanceSettingsWidget::bg_select_pattern()
{
  CommonAppearanceState state(st);
  auto file = QFileDialog::getOpenFileName(this,
                                           QString(),
                                           _last_path,
                                           tr("Images (*.png *.bmp *.jpg)"));
  if (file.isEmpty()) return;

  _last_path = file;

  QPixmap pxm(file);
  cfg.setBackground(pxm);
  state.setBackgroundPattern(pxm);
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
  cfg.setBackgroundPerChar(checked);
  impl->bg_per_char = checked;

  if (widget)
    widget->setBackgroundPerChar(checked);
}
