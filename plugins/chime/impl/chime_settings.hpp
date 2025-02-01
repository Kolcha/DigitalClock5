/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QObject>

#include <QTime>
#include <QUrl>

#include "common/config_base.hpp"
#include "common/variant_converters.hpp"

namespace chime {

Q_NAMESPACE

enum Repeat {
  Once = 1,
  Dynamic
};

Q_ENUM_NS(Repeat)

class ChimePluginConfig : public QObject, public ConfigBase
{
  Q_OBJECT

public:
  using ConfigBase::ConfigBase;

  CONFIG_OPTION(EveryHourEnabled, "every_hour/enabled", bool, true)
  CONFIG_OPTION(EveryHourSignal, "every_hour/signal", QUrl, QUrl("qrc:/chime/hour_signal.wav"))
  CONFIG_OPTION(EveryHourVolume, "every_hour/volume", int, 90)
  CONFIG_OPTION(EveryHourRepeat, "every_hour/repeat", Repeat, Repeat::Once)

  CONFIG_OPTION(QuarterHourEnabled, "quarter_hour/enabled", bool, false)
  CONFIG_OPTION(QuarterHourSignal, "quarter_hour/signal", QUrl, QUrl("qrc:/chime/quarter_signal.wav"))
  CONFIG_OPTION(QuarterHourVolume, "quarter_hour/volume", int, 90)
  CONFIG_OPTION(QuarterHourRepeat, "quarter_hour/repeat", Repeat, Repeat::Once)

  CONFIG_OPTION(QuietHoursEnabled, "quiet_hours/enabled", bool, false)
  CONFIG_OPTION(QuietHoursStartTime, "quiet_hours/start_time", QTime, QTime(23, 1))
  CONFIG_OPTION(QuietHoursEndTime, "quiet_hours/end_time", QTime, QTime(6, 59))

public slots:
  void commit() { storage()->commit(); }
  void discard() { storage()->discard(); }
};

} // namespace chime
