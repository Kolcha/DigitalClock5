// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "plugin_core_global.hpp"

#include "plugin/clock_plugin.hpp"

#include "skinned_widget.hpp"
#include "plugin_config.hpp"

#include "impl/shared_option_router.hpp"

namespace plugin::text {

// if plugin implementation overrides any of methods of this base class,
// base class implementation must be called at some point
class PLUGIN_CORE_EXPORT TextPluginInstanceBase : public SettingsPluginInstance,
                                                  public WidgetAccess,
                                                  public SkinSystemAccess
{
  Q_OBJECT

public:
  TextPluginInstanceBase(const PluginInstanceConfig& cfg);

  void init(const InstanceOptionsHash& settings) override;
  void init(QWidget* w) override;
  void init(const SkinManager* sm) override;

  SkinnedTextWidget* widget() const noexcept { return _widget; }

public slots:
  void startup() override;
  void shutdown() override;

  // this calls text() to get current text to display
  // if plugin overrides this method to update the text,
  // base implementation should be called last
  void update(const QDateTime& dt) override;

  void onOptionChanged(opt::InstanceOptions o, const QVariant& v) override;

  void applyPluginOption(wpo::InstanceOptions opt, const QVariant& val);
  void applySharedOption(opt::InstanceOptions opt, const QVariant& val);

  void reloadConfig();

protected:
  // implementation should return desired text to display
  // function is called on startup() and on update()
  virtual QString text() const = 0;
  // allows to customize the display widget
  // default implementation creates SkinnedTextWidget
  // base implementation takes ownership of the created widget
  virtual SkinnedTextWidget* createWidget(QWidget* parent) const;
  // point when custom config value should be reloaded
  // called during reloadConfig()
  virtual void pluginReloadConfig() = 0;

  // enforce widget repainting, uses text() to get new text
  void repaintWidget();

  bool eventFilter(QObject* obj, QEvent* e) override;

private:
  void loadPluginSettings();
  void loadSkinSettings();
  void loadAppearanceSettings();

  QSize availSize() const;

private:
  const PluginInstanceConfig& _cfg;
  impl::SharedOptionRouter _opt_router;

  QWidget* _clock_wnd = nullptr;

  const SkinManager* _sm = nullptr;

  SkinnedTextWidget* _widget = nullptr;

  int _row = 0;
  int _col = 0;
  int _row_span = 1;
  int _col_span = 1;

  QString _widget_text;
  LinesRenderer::Options _text_opts;
};


class PLUGIN_CORE_EXPORT TextPluginBase : public ClockPlugin
{
  Q_OBJECT

public:
  void init(Context&& ctx) override;

  ClockPluginInstance* instance(size_t idx) override;

public slots:
  void configure(QWidget* parent, size_t idx) override;

signals:
  // emitted when settings dialog changes active instance
  // if plugin provides some settings pages,
  // it may be interested in listening to it
  void instanceSwitched(size_t idx);

protected:
  using Instance = std::unique_ptr<TextPluginInstanceBase>;
  virtual Instance createInstance(size_t idx) const = 0;

  // default implementation creates PluginConfig object
  virtual std::unique_ptr<PluginConfig> createConfig(
      std::unique_ptr<PluginSettingsBackend> b) const;
  // plugin implementation may provide "configuration pages" which
  // will be inserted into settings dialog at the corresponding place
  // default implementations do nothing (return empty containers)
  // widgets should have "window title" set, this is used as "page title"
  virtual QVector<QWidget*> configPagesBeforeCommonPages() { return {}; }
  virtual QVector<QWidget*> configPagesAfterCommonPages() { return {}; }

  PluginInstanceConfig* instanceConfig(size_t idx) const;
  std::unique_ptr<SettingsStorage> instanceState(size_t idx) const;

  bool hasInstances() const noexcept { return !_insts.empty(); }

private:
  QVector<size_t> _clock_insts;

  std::map<size_t, Instance> _insts;
  std::unique_ptr<PluginConfig> _cfg;
  std::unique_ptr<PluginSettingsBackend> _state;
};

} // namespace plugin::text
