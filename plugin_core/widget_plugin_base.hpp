/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "clock_plugin.hpp"

#include "skin/graphics_widgets.hpp"

class WidgetPluginBaseImpl;

// base class for plugins which want to add some widget to the clock
// plugins inheriting this base class must set 'configurable' and
// 'settings_listener' to 'true' in plugin's metadata
// inherited class can override any of methods overridden here (except 'configure()')
// in such case, it should call base implementation
class PLUGIN_CORE_EXPORT WidgetPluginBase
    : public SettingsPlugin, public ConfigurablePlugin,
      public SkinAccessExtension, public WindowAccessExtension
{
  Q_OBJECT

public:
  WidgetPluginBase();
  ~WidgetPluginBase();

  // SettingsPlugin interface
  void initSharedSettings(const SharedSettings& s) override;

  // ConfigurablePlugin interface
  void initSettings(const SettingsClient& st) override;

  QWidget* configure(SettingsClient& s, StateClient& t) final;

  // SkinAccessExtension interface
  void initSkin(std::shared_ptr<Skin> skin) override;
  void initAppearance(const AppearanceSettings& s) override;

  // WindowAccessExtension interface
  void initWindow(QWidget* wnd) override;

public slots:
  // generic (ClockPluginBase) interface
  void init() override;
  void shutdown() override;

  // SettingsPlugin interface
  void onOptionChanged(cs::SharedConfigKeys opt, const QVariant& val) override;

protected:
  // the same as configure() and called by reimplemented configure()
  // should return a set of configuration widgets (tabs)
  // widgets should have window title property set, it is used as tab label
  virtual QList<QWidget*> customConfigure(SettingsClient& s, StateClient& t) = 0;

  // should create and set initial data to the widget, called on init()
  // plugin is allowed to keep reference to the widget
  virtual std::shared_ptr<GraphicsWidgetBase> createWidget() = 0;
  // destroy widget if any, called on shutdown()
  // after this point the only widget reference should be in base class
  virtual void destroyWidget() = 0;

  // utility function that allows to replace already known widget
  // caller is allowed to keep the reference to the provided widget
  // after this point the base class doesn't track the old widget anymore
  // and it's a caller responsibility to destroy any its remaining copies
  void replaceWidget(std::shared_ptr<GraphicsWidgetBase> widget);

private:
  std::unique_ptr<WidgetPluginBaseImpl> _impl;
};

// unfortunately, there is no way to share factory implementation,
// as factory implementation must be in plugin itself
// so, some boilerplate code will present in each plugin that uses this base class :(
