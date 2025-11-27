/*
 * SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QObject>

#include "config/config_base.hpp"
#include "config/variant_converters.hpp"

namespace talking_clock {

class TalkingClockPluginConfig : public QObject, public ConfigBase
{
  Q_OBJECT

public:
  using ConfigBase::ConfigBase;

  CONFIG_OPTION(EveryHourEnabled, "every_hour/enabled", bool, true)
  CONFIG_OPTION(EveryHourFormat, "every_hour/format", QString, QString("'It''s' ha 'o''clock'"))

  CONFIG_OPTION(QuarterHourEnabled, "quarter_hour/enabled", bool, false)
  CONFIG_OPTION(QuarterHourFormat, "quarter_hour/format", QString, QString("h:m ap"))

  CONFIG_OPTION(Volume, "volume", int, 70)

  CONFIG_OPTION(SynthesisRate, "synthesis/rate", int, 0)
  CONFIG_OPTION(SynthesisPitch, "synthesis/pitch", int, 0)
  CONFIG_OPTION(SynthesisEngine, "synthesis/engine", QString, QString("default"))
  CONFIG_OPTION(SynthesisLanguage, "synthesis/language", int, -1)
  CONFIG_OPTION(SynthesisVoice, "synthesis/voice", int, -1)

public slots:
  void commit() { storage()->commit(); }
  void discard() { storage()->discard(); }
};

} // namespace talking_clock
