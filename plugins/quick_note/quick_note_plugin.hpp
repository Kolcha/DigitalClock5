/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "widget_plugin_base.hpp"

class QuickNotePlugin : public WidgetPluginBase
{
  Q_OBJECT

public:
  void saveState(StateClient& st) const override;
  void loadState(const StateClient& st) override;

protected:
  QWidget* customConfigure(SettingsClient& s, StateClient& t) override;

  std::shared_ptr<GraphicsWidgetBase> createWidget() override;
  void destroyWidget() override;

private slots:
  void onWidgetClicked();

private:
  QString _last_text;
  std::shared_ptr<GraphicsTextWidget> _widget;
};


class QuickNotePluginFactory : public ClockPluginFactory
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPluginFactory_iid FILE "quick_note.json")
  Q_INTERFACES(ClockPluginFactory)

public:
  std::unique_ptr<ClockPluginBase> create() const override;

  QString title() const override { return tr("Quick note"); }
  QString description() const override;

  bool perClockInstance() const noexcept override { return true; }
};
