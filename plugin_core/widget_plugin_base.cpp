/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "widget_plugin_base.hpp"

#include <QTabWidget>

#include "impl/appearance_settings_widget.hpp"
#include "impl/widget_plugin_base_impl.hpp"

WidgetPluginBase::WidgetPluginBase()
    : _impl(std::make_unique<WidgetPluginBaseImpl>())
{
}

WidgetPluginBase::~WidgetPluginBase() = default;

void WidgetPluginBase::initSettings(const SettingsClient& st)
{
  _impl->initSettings(st);
  _impl->applyAppearanceSettings();
}

QWidget* WidgetPluginBase::configure(SettingsClient& s, StateClient& t)
{
  auto tab_widget = new QTabWidget;
  auto common_tab = new AppearanceSettingsWidget(_impl.get(), s, t);
  tab_widget->addTab(common_tab, common_tab->windowTitle());
  auto custom_tab = customConfigure(s, t);
  if (custom_tab->windowTitle().isEmpty())
    custom_tab->setWindowTitle(tr("Plugin"));
  tab_widget->addTab(custom_tab, custom_tab->windowTitle());
  return tab_widget;
}

void WidgetPluginBase::initSkin(std::shared_ptr<Skin> skin)
{
  if (_impl->widget && _impl->appearance == FollowClock)
    _impl->widget->setSkin(skin);
  _impl->skin = std::move(skin);
}

void WidgetPluginBase::initAppearance(const AppearanceSettings& s)
{
  _impl->skin_cfg = s;
}

void WidgetPluginBase::initWindow(QWidget* wnd)
{
  _impl->wnd = wnd;
  _impl->layout = dynamic_cast<QGridLayout*>(wnd->layout());
}

void WidgetPluginBase::init()
{
  _impl->widget = createWidget();
  _impl->applyAppearanceSettings();
  _impl->repositionWidget();
}

void WidgetPluginBase::shutdown()
{
  destroyWidget();
  Q_ASSERT(_impl->widget.use_count() == 1);
  _impl->layout->removeWidget(_impl->widget.get());
  _impl->widget.reset();
}

void WidgetPluginBase::onOptionChanged(clock_settings::SharedConfigKeys opt, const QVariant& val)
{
  _impl->skin_cfg[opt] = val;

  if (!_impl->widget)
    return;

  if (_impl->appearance != FollowClock)
    return;

  // handle only minimal set of settings
  switch (opt) {
    case cs::Texture:
      _impl->widget->setTexture(val.value<QBrush>());
      break;
    case cs::TextureStretch:
      _impl->widget->setTextureStretch(val.toBool());
      break;
    case cs::TexturePerCharacter:
      _impl->widget->setTexturePerChar(val.toBool());
      break;
    case cs::Background:
      _impl->widget->setBackground(val.value<QBrush>());
      break;
    case cs::BackgroundStretch:
      _impl->widget->setBackgroundStretch(val.toBool());
      break;
    case cs::BackgroundPerCharacter:
      _impl->widget->setBackgroundPerChar(val.toBool());
      break;
    default:  // ugly default in modern world...
      break;  // just to make everything happy
  }
}