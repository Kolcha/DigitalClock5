/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_bridge.hpp"

#include <QBrush>
#include <QFont>

#include <QVariant>

namespace {
// `emit` is a macro, so it is "resolved" if used in another macro,
// so use helper function just to emit the necessary signal
inline void emitOptionChanged(SettingsChangeTransmitter* t,
                              cs::SharedConfigKeys opt,
                              const QVariant& val)
{
  emit t->optionChanged(opt, val);
}

} // namespace

#define OPTION_CHANGE_NOTIFIER_IMPL(name, type)                   \
{                                                                 \
  emitOptionChanged(this, cs::name, QVariant::fromValue(value));  \
}

#define OPTION_CHANGE_NOTIFIER_R(name, type)                      \
void SettingsChangeTransmitter::set##name(const type& value)      \
  OPTION_CHANGE_NOTIFIER_IMPL(name, type)

#define OPTION_CHANGE_NOTIFIER_V(name, type)                      \
void SettingsChangeTransmitter::set##name(type value)             \
  OPTION_CHANGE_NOTIFIER_IMPL(name, type)

#define HANDLE_OPTION_CHANGE(val, name, type)                     \
    case cs::name:                                                \
      on##name##Changed(val.value<type>());                       \
      break;


SettingsChangeTransmitter::SettingsChangeTransmitter(QObject* parent)
    : QObject(parent)
{}

OPTION_CHANGE_NOTIFIER_V(Opacity, int)

OPTION_CHANGE_NOTIFIER_V(ScalingH, int)
OPTION_CHANGE_NOTIFIER_V(ScalingV, int)

OPTION_CHANGE_NOTIFIER_V(SpacingH, int)
OPTION_CHANGE_NOTIFIER_V(SpacingV, int)

OPTION_CHANGE_NOTIFIER_V(ApplyColorization, bool)
OPTION_CHANGE_NOTIFIER_V(ColorizationStrength, qreal)
OPTION_CHANGE_NOTIFIER_R(ColorizationColor, QColor)

OPTION_CHANGE_NOTIFIER_V(UseFontInsteadOfSkin, bool)
OPTION_CHANGE_NOTIFIER_R(Font, QFont)
OPTION_CHANGE_NOTIFIER_R(Skin, QString)

OPTION_CHANGE_NOTIFIER_V(SeparatorFlashes, bool)
OPTION_CHANGE_NOTIFIER_V(UseCustomSeparators, bool)
OPTION_CHANGE_NOTIFIER_R(CustomSeparators, QString)

OPTION_CHANGE_NOTIFIER_R(TimeFormat, QString)
OPTION_CHANGE_NOTIFIER_V(SecondsScaleFactor, int)

OPTION_CHANGE_NOTIFIER_R(Texture, QBrush)
OPTION_CHANGE_NOTIFIER_V(TextureStretch, bool)
OPTION_CHANGE_NOTIFIER_V(TexturePerCharacter, bool)

OPTION_CHANGE_NOTIFIER_R(Background, QBrush)
OPTION_CHANGE_NOTIFIER_V(BackgroundStretch, bool)
OPTION_CHANGE_NOTIFIER_V(BackgroundPerCharacter, bool)


SettingsChangeReceiver::SettingsChangeReceiver(QObject* parent)
    : QObject(parent)
{}

void SettingsChangeReceiver::onOptionChanged(cs::SharedConfigKeys opt, const QVariant& val)
{
  switch (opt) {
    HANDLE_OPTION_CHANGE(val, Opacity, int)

    HANDLE_OPTION_CHANGE(val, ScalingH, int)
    HANDLE_OPTION_CHANGE(val, ScalingV, int)

    HANDLE_OPTION_CHANGE(val, SpacingH, int)
    HANDLE_OPTION_CHANGE(val, SpacingV, int)

    HANDLE_OPTION_CHANGE(val, ApplyColorization, bool)
    HANDLE_OPTION_CHANGE(val, ColorizationStrength, qreal)
    HANDLE_OPTION_CHANGE(val, ColorizationColor, QColor)

    HANDLE_OPTION_CHANGE(val, UseFontInsteadOfSkin, bool)
    HANDLE_OPTION_CHANGE(val, Font, QFont)
    HANDLE_OPTION_CHANGE(val, Skin, QString)

    HANDLE_OPTION_CHANGE(val, SeparatorFlashes, bool)
    HANDLE_OPTION_CHANGE(val, UseCustomSeparators, bool)
    HANDLE_OPTION_CHANGE(val, CustomSeparators, QString)

    HANDLE_OPTION_CHANGE(val, TimeFormat, QString)
    HANDLE_OPTION_CHANGE(val, SecondsScaleFactor, int)

    HANDLE_OPTION_CHANGE(val, Texture, QBrush)
    HANDLE_OPTION_CHANGE(val, TextureStretch, bool)
    HANDLE_OPTION_CHANGE(val, TexturePerCharacter, bool)

    HANDLE_OPTION_CHANGE(val, Background, QBrush)
    HANDLE_OPTION_CHANGE(val, BackgroundStretch, bool)
    HANDLE_OPTION_CHANGE(val, BackgroundPerCharacter, bool)
  }
}
