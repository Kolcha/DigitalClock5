/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/text/skinned_text_plugin_base.hpp"

using namespace plugin::text;

class QuickNotePlugin : public TextPluginInstanceBase
{
  Q_OBJECT

public:
  QuickNotePlugin(const PluginInstanceConfig& cfg, std::unique_ptr<SettingsStorage> st);

  void startup() override;

protected:
  QString text() const override { return _last_text; }

  SkinnedTextWidget* createWidget(QWidget* parent) const override;

  void pluginReloadConfig() override {}

private slots:
  void onWidgetClicked();

private:
  QString _last_text;
  std::unique_ptr<SettingsStorage> _st;
};


class QuickNotePluginFactory : public TextPluginBase
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "quick_note.json")
  Q_INTERFACES(ClockPlugin)

public:
  QString name() const override { return tr("Quick note"); }
  QString description() const override;

protected:
  Instance createInstance(size_t idx) const override
  {
    return std::make_unique<QuickNotePlugin>(*instanceConfig(idx), instanceState(idx));
  }

  QVector<QWidget*> configPagesBeforeCommonPages() override;
};
