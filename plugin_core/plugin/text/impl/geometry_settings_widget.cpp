/*
 * SPDX-FileCopyrightText: 2024-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "geometry_settings_widget.hpp"
#include "ui_geometry_settings_widget.h"

namespace plugin::text::impl {

GeometrySettingsWidget::GeometrySettingsWidget(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::GeometrySettingsWidget)
{
  ui->setupUi(this);

  connect(ui->m_char_l, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onCharMarginsChanged);
  connect(ui->m_char_t, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onCharMarginsChanged);
  connect(ui->m_char_r, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onCharMarginsChanged);
  connect(ui->m_char_b, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onCharMarginsChanged);

  connect(ui->m_text_l, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onTextMarginsChanged);
  connect(ui->m_text_t, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onTextMarginsChanged);
  connect(ui->m_text_r, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onTextMarginsChanged);
  connect(ui->m_text_b, &QDoubleSpinBox::valueChanged, this, &GeometrySettingsWidget::onTextMarginsChanged);
}

GeometrySettingsWidget::~GeometrySettingsWidget()
{
  delete ui;
}

void GeometrySettingsWidget::initControls(PluginInstanceConfig* icfg)
{
  Q_ASSERT(icfg);
  cfg = icfg;

  QSignalBlocker _(this);
  // TODO: disable if clock skin is used
  const auto& cm = cfg->getCharMargins();
  ui->m_char_l->setValue(cm.left());
  ui->m_char_t->setValue(cm.top());
  ui->m_char_r->setValue(cm.right());
  ui->m_char_b->setValue(cm.bottom());

  const auto& tm = cfg->getWidgetMargins();
  ui->m_text_l->setValue(tm.left());
  ui->m_text_t->setValue(tm.top());
  ui->m_text_r->setValue(tm.right());
  ui->m_text_b->setValue(tm.bottom());

  ui->true_per_char->setChecked(cfg->getTruePerCharRendering());
  ui->respect_line_spacing->setChecked(cfg->getRespectLineSpacing());

  ui->hs_edit->setValue(cfg->getCharSpacing());
  ui->vs_edit->setValue(cfg->getLineSpacing());
}

void GeometrySettingsWidget::onCharMarginsChanged()
{
  QMarginsF m(ui->m_char_l->value(), ui->m_char_t->value(), ui->m_char_r->value(), ui->m_char_b->value());
  cfg->setCharMargins(m);
  emit sharedOptionChanged(opt::CharMargins, QVariant::fromValue(m));
}

void GeometrySettingsWidget::onTextMarginsChanged()
{
  QMarginsF m(ui->m_text_l->value(), ui->m_text_t->value(), ui->m_text_r->value(), ui->m_text_b->value());
  cfg->setWidgetMargins(m);
  emit sharedOptionChanged(opt::WidgetMargins, QVariant::fromValue(m));
}

void GeometrySettingsWidget::on_true_per_char_clicked(bool checked)
{
  cfg->setTruePerCharRendering(checked);
  emit sharedOptionChanged(opt::TruePerCharRendering, checked);
}

void GeometrySettingsWidget::on_respect_line_spacing_clicked(bool checked)
{
  cfg->setRespectLineSpacing(checked);
  emit sharedOptionChanged(opt::RespectLineSpacing, checked);
}

void GeometrySettingsWidget::on_hs_edit_valueChanged(int arg1)
{
  cfg->setCharSpacing(arg1);
  emit sharedOptionChanged(opt::CharSpacing, arg1);
}

void GeometrySettingsWidget::on_vs_edit_valueChanged(int arg1)
{
  cfg->setLineSpacing(arg1);
  emit sharedOptionChanged(opt::LineSpacing, arg1);
}

} // namespace plugin::text::impl
