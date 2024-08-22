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

  const auto& fnt = cfg.getCustomFont();
  ui->font_value->setText(QString("%1, %2").arg(fnt.family()).arg(fnt.pointSize()));

  const auto& tx = cfg.getTexture();
  ui->tx_group->setChecked(tx.style() != Qt::NoBrush);
  ui->tx_color->setChecked(tx.style() == Qt::SolidPattern);
  ui->tx_gradient->setChecked(tx.gradient() != nullptr);
  ui->tx_pattern->setChecked(tx.style() == Qt::TexturePattern);
  ui->tx_stretch->setChecked(cfg.getTextureStretch());
  ui->tx_per_char->setChecked(cfg.getTexturePerChar());

  const auto& bg = cfg.getBackground();
  ui->bg_group->setChecked(bg.style() != Qt::NoBrush);
  ui->bg_color->setChecked(bg.style() == Qt::SolidPattern);
  ui->bg_gradient->setChecked(bg.gradient() != nullptr);
  ui->bg_pattern->setChecked(bg.style() == Qt::TexturePattern);
  ui->bg_stretch->setChecked(cfg.getBackgroundStretch());
  ui->bg_per_char->setChecked(cfg.getBackgroundPerChar());

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
  if (widget)
    widget->setAutoResizePolicy(cfg.getWidgetPosition() == WidgetPosition::Bottom ?
                                    GraphicsWidgetBase::KeepWidth :
                                    GraphicsWidgetBase::KeepHeight);
  impl->resize_policy = ResizePolicy::Autoscale;
  cfg.setResizePolicy(ResizePolicy::Autoscale);
}

void AppearanceSettingsWidget::on_resize_fixed_clicked()
{
  if (widget)
    widget->setAutoResizePolicy(GraphicsWidgetBase::None);
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

void AppearanceSettingsWidget::on_font_select_clicked()
{
  bool ok = false;
  QFont fnt = QFontDialog::getFont(&ok, cfg.getCustomFont(), this);
  if (!ok) return;

  ui->font_value->setText(QString("%1, %2").arg(fnt.family()).arg(fnt.pointSize()));

  cfg.setCustomFont(fnt);
  impl->custom_skin = std::make_shared<FontSkin>(fnt);

  if (widget)
    widget->setSkin(impl->custom_skin);
}

void AppearanceSettingsWidget::on_tx_group_clicked(bool checked)
{
  CommonAppearanceState state(st);

  QBrush brush(Qt::NoBrush);

  if (checked) {
    if (ui->tx_color->isChecked())
      brush = QBrush(state.getTextureColor());

    if (ui->tx_gradient->isChecked())
      brush = QBrush(state.getTextureGradient());

    if (ui->tx_pattern->isChecked())
      brush = QBrush(state.getTexturePattern());
  }

  cfg.setTexture(brush);
  impl->tx = brush;

  if (widget)
    widget->setTexture(std::move(brush));
}

void AppearanceSettingsWidget::on_tx_color_clicked()
{
  CommonAppearanceState state(st);
  cfg.setTexture(state.getTextureColor());
  impl->tx = state.getTextureColor();

  if (widget)
    widget->setTexture(state.getTextureColor());
}

void AppearanceSettingsWidget::on_tx_color_select_clicked()
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

void AppearanceSettingsWidget::on_tx_gradient_clicked()
{
  CommonAppearanceState state(st);
  cfg.setTexture(state.getTextureGradient());
  impl->tx = state.getTextureGradient();

  if (widget)
    widget->setTexture(state.getTextureGradient());
}

void AppearanceSettingsWidget::on_tx_gradient_select_clicked()
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

void AppearanceSettingsWidget::on_tx_pattern_clicked()
{
  CommonAppearanceState state(st);
  cfg.setTexture(state.getTexturePattern());
  impl->tx = state.getTexturePattern();

  if (widget)
    widget->setTexture(state.getTexturePattern());
}

void AppearanceSettingsWidget::on_tx_pattern_select_clicked()
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

void AppearanceSettingsWidget::on_tx_stretch_clicked(bool checked)
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
    if (ui->bg_color->isChecked())
      brush = QBrush(state.getBackgroundColor());

    if (ui->bg_gradient->isChecked())
      brush = QBrush(state.getBackgroundGradient());

    if (ui->bg_pattern->isChecked())
      brush = QBrush(state.getBackgroundPattern());
  }

  cfg.setBackground(brush);
  impl->bg = brush;

  if (widget)
    widget->setBackground(std::move(brush));
}

void AppearanceSettingsWidget::on_bg_color_clicked()
{
  CommonAppearanceState state(st);
  cfg.setBackground(state.getBackgroundColor());
  impl->bg = state.getBackgroundColor();

  if (widget)
    widget->setBackground(state.getBackgroundColor());
}

void AppearanceSettingsWidget::on_bg_color_select_clicked()
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

void AppearanceSettingsWidget::on_bg_gradient_clicked()
{
  CommonAppearanceState state(st);
  cfg.setBackground(state.getBackgroundGradient());
  impl->bg = state.getBackgroundGradient();

  if (widget)
    widget->setBackground(state.getBackgroundGradient());
}

void AppearanceSettingsWidget::on_bg_gradient_select_clicked()
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

void AppearanceSettingsWidget::on_bg_pattern_clicked()
{
  CommonAppearanceState state(st);
  cfg.setBackground(state.getBackgroundPattern());
  impl->bg = state.getBackgroundPattern();

  if (widget)
    widget->setBackground(state.getBackgroundPattern());
}

void AppearanceSettingsWidget::on_bg_pattern_select_clicked()
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

void AppearanceSettingsWidget::on_bg_stretch_clicked(bool checked)
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
