/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/text/skinned_text_plugin_base.hpp"

#include "impl/quick_note_settings.hpp"

using namespace plugin::text;
using plugin::quick_note::QuickNotePluginInstanceConfig;

class QuickNotePlugin : public TextPluginInstanceBase
{
  Q_OBJECT

public:
  QuickNotePlugin(QuickNotePluginInstanceConfig* cfg, std::unique_ptr<SettingsStorage> st);

  void startup() override;

  void onTextChanged(const QString& txt);

protected:
  QString text() const override { return _last_text; }

  SkinnedTextWidget* createWidget(QWidget* parent) const override;

  void pluginReloadConfig() override;

private slots:
  void onWidgetClicked();

private:
  QString _last_text;
  QuickNotePluginInstanceConfig* _cfg;
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
    auto cfg = qobject_cast<QuickNotePluginInstanceConfig*>(instanceConfig(idx));
    Q_ASSERT(cfg);
    return std::make_unique<QuickNotePlugin>(cfg, instanceState(idx));
  }

  std::unique_ptr<PluginConfig> createConfig(
    std::unique_ptr<PluginSettingsBackend> b) const override
  {
    return std::make_unique<plugin::quick_note::QuickNotePluginConfig>(std::move(b));
  }

  QVector<QWidget*> configPagesBeforeCommonPages() override;
};
