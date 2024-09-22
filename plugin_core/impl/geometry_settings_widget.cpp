/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "geometry_settings_widget.hpp"
#include "ui_geometry_settings_widget.h"

GeometrySettingsWidget::GeometrySettingsWidget(WidgetPluginBaseImpl* w, PluginSettingsStorage& s, StateClient& t, QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::GeometrySettingsWidget)
  , impl(w)
  , widget(w->widget.get())
  , cfg(s)
  , st(t)
{
  ui->setupUi(this);

  // TODO: disable if clock skin is used
  const auto& cm = cfg.getCharMargins();
  ui->m_char_l->setValue(cm.left());
  ui->m_char_t->setValue(cm.top());
  ui->m_char_r->setValue(cm.right());
  ui->m_char_b->setValue(cm.bottom());

  connect(ui->m_char_l, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onCharMarginsChanged);
  connect(ui->m_char_t, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onCharMarginsChanged);
  connect(ui->m_char_r, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onCharMarginsChanged);
  connect(ui->m_char_b, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onCharMarginsChanged);

  const auto& tm = cfg.getTextMargins();
  ui->m_text_l->setValue(tm.left());
  ui->m_text_t->setValue(tm.top());
  ui->m_text_r->setValue(tm.right());
  ui->m_text_b->setValue(tm.bottom());

  connect(ui->m_text_l, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onTextMarginsChanged);
  connect(ui->m_text_t, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onTextMarginsChanged);
  connect(ui->m_text_r, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onTextMarginsChanged);
  connect(ui->m_text_b, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onTextMarginsChanged);

  ui->ignore_ax->setChecked(cfg.getIgnoreAX());
  ui->ignore_ay->setChecked(cfg.getIgnoreAY());

  ui->hs_edit->setValue(cfg.getSpacingH());
  ui->vs_edit->setValue(cfg.getSpacingV());
}

GeometrySettingsWidget::~GeometrySettingsWidget()
{
  delete ui;
}

void GeometrySettingsWidget::onCharMarginsChanged()
{
  QMarginsF m(ui->m_char_l->value(), ui->m_char_t->value(), ui->m_char_r->value(), ui->m_char_b->value());
  if (impl->custom_skin)
    impl->custom_skin->setMargins(m);

  if (widget)
    widget->rebuildLayout();

  impl->char_margins = m;
  cfg.setCharMargins(m);
}

void GeometrySettingsWidget::onTextMarginsChanged()
{
  QMarginsF m(ui->m_text_l->value(), ui->m_text_t->value(), ui->m_text_r->value(), ui->m_text_b->value());

  if (widget)
    widget->setMargins(m);

  impl->text_margins = m;
  cfg.setTextMargins(m);
}

void GeometrySettingsWidget::on_ignore_ax_clicked(bool checked)
{
  if (widget)
    widget->setIgnoreAX(checked);

  impl->ignore_ax = checked;
  cfg.setIgnoreAX(checked);
}

void GeometrySettingsWidget::on_ignore_ay_clicked(bool checked)
{
  if (widget)
    widget->setIgnoreAY(checked);

  impl->ignore_ay = checked;
  cfg.setIgnoreAY(checked);
}

void GeometrySettingsWidget::on_hs_edit_valueChanged(int arg1)
{
  if (widget)
    widget->setCharSpacing(arg1);

  impl->spacing_h = arg1;
  cfg.setSpacingH(arg1);
}

void GeometrySettingsWidget::on_vs_edit_valueChanged(int arg1)
{
  if (widget)
    widget->setLineSpacing(arg1);

  impl->spacing_v = arg1;
  cfg.setSpacingV(arg1);
}
