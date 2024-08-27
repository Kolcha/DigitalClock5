/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin_config.hpp"

#include <QTime>
#include <QUrl>

namespace chime {

Q_NAMESPACE

enum Repeat {
  Once = 1,
  Dynamic
};

Q_ENUM_NS(Repeat)

} // namespace chime

namespace plugin_impl {

template<class C>
class EveryHourSignalSection : public PluginConfigSection<C> {
public:
  explicit EveryHourSignalSection(C& c) : PluginConfigSection<C>(c) {}

  PLG_CONFIG_OPTION_Q(bool, Enabled, true);
  PLG_CONFIG_OPTION_Q(QUrl, Signal, QUrl("qrc:/chime/hour_signal.wav"))
  PLG_CONFIG_OPTION_Q(int, Volume, 90)
  PLG_CONFIG_OPTION_Q(chime::Repeat, Repeat, chime::Once)

protected:
  QLatin1String section() const override { return QLatin1String("EveryHour"); }
};


template<class C>
class QuarterHourSignalSection : public PluginConfigSection<C> {
public:
  explicit QuarterHourSignalSection(C& c) : PluginConfigSection<C>(c) {}

  PLG_CONFIG_OPTION_Q(bool, Enabled, false);
  PLG_CONFIG_OPTION_Q(QUrl, Signal, QUrl("qrc:/chime/quarter_signal.wav"))
  PLG_CONFIG_OPTION_Q(int, Volume, 90)
  PLG_CONFIG_OPTION_Q(chime::Repeat, Repeat, chime::Once)

protected:
  QLatin1String section() const override { return QLatin1String("QuarterHour"); }
};


template<class C>
class QuietHoursSection : public PluginConfigSection<C> {
public:
  explicit QuietHoursSection(C& c) : PluginConfigSection<C>(c) {}

  PLG_CONFIG_OPTION_Q(bool, Enabled, false)
  PLG_CONFIG_OPTION_Q(QTime, StartTime, QTime(23, 1))
  PLG_CONFIG_OPTION_Q(QTime, EndTime, QTime(6, 59))

protected:
  QLatin1String section() const override { return QLatin1String("QuietHours"); }
};


template<class C>
class ChimePluginSettings : public PluginConfig<C> {
public:
  explicit ChimePluginSettings(C& c)
      : PluginConfig<C>(c)
      , _eh(c)
      , _qh(c)
      , _qhrs(c)
  {}

  auto& hour() { return _eh; }
  auto& hour() const { return _eh; }
  auto& quarter() { return _qh; }
  auto& quarter() const { return _qh; }

  auto& quietTime() { return _qhrs; }
  auto& quietTime() const { return _qhrs; }

private:
  EveryHourSignalSection<C> _eh;
  QuarterHourSignalSection<C> _qh;
  QuietHoursSection<C> _qhrs;
};

} // namespace plugin_impl
