/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/text/skinned_text_plugin_base.hpp"

#include "impl/date_settings.hpp"

using namespace plugin::text;
using plugin::date::DatePluginInstanceConfig;

class DatePlugin : public TextPluginInstanceBase
{
  Q_OBJECT

public:
  DatePlugin(const DatePluginInstanceConfig* cfg);

public slots:
  void startup() override;

  void update(const QDateTime& dt) override;

  void onDateFormatChanged();

protected:
  QString text() const override { return _date_str; }

private:
  void updateDateFmt();
  void updateDateStr();

private:
  const DatePluginInstanceConfig* _cfg;
  QDateTime _last_date;
  QString _date_fmt;
  QString _date_str;
};


class DatePluginFactory : public TextPluginBase
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "date.json")
  Q_INTERFACES(ClockPlugin)

public:
  QString name() const override { return tr("Date"); }
  QString description() const override;

protected:
  Instance createInstance(size_t idx) const override
  {
    auto cfg = qobject_cast<DatePluginInstanceConfig*>(instanceConfig(idx));
    Q_ASSERT(cfg);
    return std::make_unique<DatePlugin>(cfg);
  }

  std::unique_ptr<PluginConfig> createConfig(
      std::unique_ptr<PluginSettingsBackend> b) const override
  {
    return std::make_unique<plugin::date::DatePluginConfig>(std::move(b));
  }

  QVector<QWidget*> configPagesBeforeCommonPages() override;
};
