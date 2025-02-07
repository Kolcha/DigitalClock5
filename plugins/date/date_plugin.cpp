/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "date_plugin.hpp"

#include "utilities/datetime_formatter.hpp"

#include "gui/settings_widget.hpp"

namespace {

class DateStringBuilder : public DateTimeStringBuilder
{
public:
  explicit DateStringBuilder(QString& str) noexcept
    : _str(str)
  {}

  void addCharacter(char32_t ch) { _str += QString::fromUcs4(&ch, 1); }
  void addSeparator(char32_t ch) { addCharacter(ch); }

private:
  QString& _str;
};

} // namespace

DatePlugin::DatePlugin(const DatePluginInstanceConfig* cfg)
  : TextPluginInstanceBase(*cfg)
  , _cfg(cfg)
{
}

void DatePlugin::startup()
{
  _last_date = QDateTime::currentDateTime();
  TextPluginInstanceBase::startup();
}

void DatePlugin::update(const QDateTime& dt)
{
  if (dt.date() != _last_date.date()) {
    _last_date = dt;
    updateDateStr();
  }
  TextPluginInstanceBase::update(dt);
}

void DatePlugin::onDateFormatChanged()
{
  updateDateFmt();
  updateDateStr();
  repaintWidget();
}

void DatePlugin::pluginReloadConfig()
{
  updateDateFmt();
  updateDateStr();
}

void DatePlugin::updateDateFmt()
{
  _date_fmt = _cfg->getFormatStr();
  if (_cfg->getFormatType() == DatePluginInstanceConfig::System) {
    auto fmt_type = QLocale::LongFormat;
    if (_cfg->getFormatSys() != fmt_type)
      fmt_type = QLocale::ShortFormat;
    _date_fmt = QLocale().dateFormat(fmt_type);
  }
}

void DatePlugin::updateDateStr()
{
  _date_str.clear();
  DateStringBuilder builder(_date_str);
  FormatDateTime(_last_date, _date_fmt, builder);
}


QString DatePluginFactory::description() const
{
  return tr("Allows to display current date under the clock.");
}

QVector<QWidget*> DatePluginFactory::configPagesBeforeCommonPages()
{
  using plugin::date::SettingsWidget;
  auto page = new SettingsWidget();
  connect(this, &DatePluginFactory::instanceSwitched, page, [this, page](size_t idx) {
    page->initControls(qobject_cast<DatePluginInstanceConfig*>(instanceConfig(idx)));
    if (!hasInstances()) return;
    disconnect(page, &SettingsWidget::dateFormatChanged, nullptr, nullptr);
    auto inst = qobject_cast<DatePlugin*>(instance(idx));
    connect(page, &SettingsWidget::dateFormatChanged, inst, &DatePlugin::onDateFormatChanged);
  });
  return { page };
}
