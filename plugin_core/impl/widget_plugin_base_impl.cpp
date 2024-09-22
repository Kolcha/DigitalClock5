/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "widget_plugin_base_impl.hpp"

void WidgetPluginBaseImpl::applyAppearanceSettings()
{
  if (!widget)
    return;

  custom_skin->setMargins(char_margins);

  if (use_clock_skin)
    widget->setSkin(skin);
  else
    widget->setSkin(custom_skin);

  bool use_clock_settings = appearance == FollowClock;
  if (use_clock_settings) {
    widget->setTexture(skin_cfg[cs::Texture].value<QBrush>(),
                       skin_cfg[cs::TextureStretch].toBool(),
                       skin_cfg[cs::TexturePerCharacter].toBool());
    widget->setUseSystemForeground(widget->texture().style() == Qt::SolidPattern &&
                                   skin_cfg[cs::UseSystemForeground].toBool());
    widget->setBackground(skin_cfg[cs::Background].value<QBrush>(),
                          skin_cfg[cs::BackgroundStretch].toBool(),
                          skin_cfg[cs::BackgroundPerCharacter].toBool());
    widget->setUseSystemBackground(widget->background().style() == Qt::SolidPattern &&
                                   skin_cfg[cs::UseSystemBackground].toBool());
  } else {
    widget->setTexture(tx, tx_stretch, tx_per_char);
    widget->setBackground(bg, bg_stretch, bg_per_char);
    widget->setUseSystemForeground(tx.style() == Qt::SolidPattern && use_sys_fg);
    widget->setUseSystemBackground(bg.style() == Qt::SolidPattern && use_sys_bg);
  }

  widget->setMargins(text_margins);

  widget->setIgnoreAX(ignore_ax);
  widget->setIgnoreAY(ignore_ay);

  widget->setCharSpacing(spacing_h);
  widget->setLineSpacing(spacing_v);

  widget->setLayoutConfig(layout_cfg);
}

void WidgetPluginBaseImpl::repositionWidget()
{
  if (!widget)
    return;

  layout->removeWidget(widget.get());

  bool fixed_size = resize_policy == Fixed;

  if (position == Bottom) {
    widget->setAutoResizePolicy(fixed_size ? GraphicsWidgetBase::None : GraphicsWidgetBase::KeepWidth);
    int r = layout->rowCount();
    int cs = layout->columnCount();
    layout->addWidget(widget.get(), r, 0, 1, cs, fixed_size ? content_alignment : Qt::Alignment());
  } else {
    widget->setAutoResizePolicy(fixed_size ? GraphicsWidgetBase::None : GraphicsWidgetBase::KeepHeight);
    int c = layout->columnCount();
    int rs = layout->rowCount();
    layout->addWidget(widget.get(), 0, c, rs, 1, fixed_size ? content_alignment : Qt::Alignment());
  }
}

void WidgetPluginBaseImpl::initSettings(PluginSettingsStorage& st)
{
  plugin_impl::SectionAppearance sa(st);

  position = sa.getWidgetPosition();
  resize_policy = sa.getResizePolicy();
  appearance = sa.getFollowClock() ? FollowClock : Custom;
  content_alignment = sa.getAlignment();
  layout_cfg = sa.getLayoutConfig();
  use_clock_skin = sa.getUseClockSkin();

  custom_skin = std::make_shared<FontSkin>(sa.getCustomFont());

  tx = sa.getTexture();
  tx_stretch = sa.getTextureStretch();
  tx_per_char = sa.getTexturePerCharacter();

  bg = sa.getBackground();
  bg_stretch = sa.getBackgroundStretch();
  bg_per_char = sa.getBackgroundPerCharacter();

  use_sys_fg = sa.getUseSystemForeground();
  use_sys_bg = sa.getUseSystemBackground();

  char_margins = sa.getCharMargins();
  text_margins = sa.getTextMargins();

  ignore_ax = sa.getIgnoreAX();
  ignore_ay = sa.getIgnoreAY();

  spacing_h = sa.getSpacingH();
  spacing_v = sa.getSpacingV();
}
