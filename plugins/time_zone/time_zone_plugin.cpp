/*
 * SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "time_zone_plugin.hpp"

#include "utilities/datetime_formatter.hpp"

#include "gui/settings_widget.hpp"

namespace {

char32_t escape_char(char32_t c) noexcept
{
  switch (c) {
    case 'n':
      return '\n';
    default:
      return c;
  }
}

void add_char(QString& str, char32_t c)
{
  str.append(QString::fromUcs4(&c, 1));
}

// Z - IANA ID
// C - city (unlocalized)
// c - OS-specific comment
// n - OS-specific name
// a - OS-specific abbreviation
// O - UTC offset +/-00:00
// o - UTC offset +/-0000

QString FormatTimeZone(const QDateTime& dt, const QString& sfmt)
{
  QString res;
  const auto& tz = dt.timeZone();

  const auto fp = sfmt.toUcs4();
  const QVector<char32_t> fmt(fp.begin(), fp.end());

  bool escaped = false;
  bool quoted = false;

  for (qsizetype i = 0; i < fmt.size(); ++i) {
    const auto& c = fmt[i];

    if (escaped) {
      escaped = false;
      add_char(res, escape_char(c));
      continue;
    }

    if (c == '\\') {
      escaped = true;
      continue;
    }

    if (c == '\'') {
      quoted = !quoted;
      continue;
    }

    if (quoted) {
      add_char(res, c);
      continue;
    }

    switch (c) {
      case 'Z':
        res.append(QString::fromLatin1(tz.id()));
        break;
      case 'C':
        res.append(QString::fromLatin1(tz.id().mid(tz.id().indexOf('/') + 1)));
        break;
      case 'c':
        res.append(tz.comment());
        break;
      case 'n':
        res.append(tz.displayName(dt));
        break;
      case 'a':
        res.append(tz.abbreviation(dt));
        break;
      case 'O':
        res.append(dt.toString("ttt"));
        break;
      case 'o':
        res.append(dt.toString("tt"));
        break;
      default:
        add_char(res, c);
        break;
    }
  }

  return res;
}

} // namespace

TimeZonePlugin::TimeZonePlugin(const TimeZonePluginInstanceConfig* cfg)
  : TextPluginInstanceBase(*cfg)
  , _cfg(cfg)
{
}

void TimeZonePlugin::startup()
{
  _last_date = QDateTime::currentDateTime();
  TextPluginInstanceBase::startup();
}

void TimeZonePlugin::update(const QDateTime& dt)
{
  if (dt.timeZone() != _last_date.timeZone()) {
    _last_date = dt;
    updateTimeZoneStr();
  }
  TextPluginInstanceBase::update(dt);
}

void TimeZonePlugin::onTimeZoneFormatChanged()
{
  updateTimeZoneFmt();
  updateTimeZoneStr();
  repaintWidget();
}

void TimeZonePlugin::pluginReloadConfig()
{
  updateTimeZoneFmt();
  updateTimeZoneStr();
}

void TimeZonePlugin::updateTimeZoneFmt()
{
  _tz_fmt = _cfg->getFormatStr();
}

void TimeZonePlugin::updateTimeZoneStr()
{
  _tz_str = FormatTimeZone(_last_date, _tz_fmt);
}


QString TimeZonePluginFactory::description() const
{
  return tr("Displays current time zone under the clock.");
}

QVector<QWidget*> TimeZonePluginFactory::configPagesBeforeCommonPages()
{
  using plugin::time_zone::SettingsWidget;
  auto page = new SettingsWidget();
  connect(this, &TimeZonePluginFactory::instanceSwitched, page, [this, page](size_t idx) {
    page->initControls(qobject_cast<TimeZonePluginInstanceConfig*>(instanceConfig(idx)));
    if (!hasInstances()) return;
    disconnect(page, &SettingsWidget::tzFormatChanged, nullptr, nullptr);
    auto inst = qobject_cast<TimeZonePlugin*>(instance(idx));
    connect(page, &SettingsWidget::tzFormatChanged, inst, &TimeZonePlugin::onTimeZoneFormatChanged);
  });
  return { page };
}
