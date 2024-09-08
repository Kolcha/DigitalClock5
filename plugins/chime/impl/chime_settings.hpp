/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "config/settings_storage.hpp"
#include "config/custom_converters.hpp"

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

class EveryHourSignalSection : public SettingsStorageClient {
public:
  explicit EveryHourSignalSection(ISettingsStorage& st)
      : SettingsStorageClient("EveryHour", st) {}

  CONFIG_OPTION_Q(bool, Enabled, true);
  CONFIG_OPTION_Q(QUrl, Signal, QUrl("qrc:/chime/hour_signal.wav"))
  CONFIG_OPTION_Q(int, Volume, 90)
  CONFIG_OPTION_Q(chime::Repeat, Repeat, chime::Once)
};


class QuarterHourSignalSection : public SettingsStorageClient {
public:
  explicit QuarterHourSignalSection(ISettingsStorage& st)
      : SettingsStorageClient("QuarterHour", st) {}

  CONFIG_OPTION_Q(bool, Enabled, false);
  CONFIG_OPTION_Q(QUrl, Signal, QUrl("qrc:/chime/quarter_signal.wav"))
  CONFIG_OPTION_Q(int, Volume, 90)
  CONFIG_OPTION_Q(chime::Repeat, Repeat, chime::Once)
};


class QuietHoursSection : public SettingsStorageClient {
public:
  explicit QuietHoursSection(ISettingsStorage& st)
      : SettingsStorageClient("QuietHours", st) {}

  CONFIG_OPTION_Q(bool, Enabled, false)
  CONFIG_OPTION_Q(QTime, StartTime, QTime(23, 1))
  CONFIG_OPTION_Q(QTime, EndTime, QTime(6, 59))
};


class ChimePluginSettings {
public:
  explicit ChimePluginSettings(ISettingsStorage& c)
      : _eh(c)
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
  EveryHourSignalSection _eh;
  QuarterHourSignalSection _qh;
  QuietHoursSection _qhrs;
};

} // namespace plugin_impl
