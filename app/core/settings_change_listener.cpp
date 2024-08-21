/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_change_listener.hpp"

#include <QGraphicsColorizeEffect>

#include "application.hpp"
#include "gui/clock_window.hpp"

struct SettingsChangeListener::Impl {
  qreal sx;
  qreal sy;

  bool colorize;
  qreal colorize_strength;
  QColor colorize_color;

  bool use_font;
  QFont font;
  QString skin;
};

SettingsChangeListener::SettingsChangeListener(Application* a, size_t i, QObject* parent)
    : SettingsChangeReceiver(parent)
    , _app(a)
    , _wnd(a->window(i))
    , _impl(std::make_unique<Impl>())
{
  const auto& wcfg = a->config().window(i).appearance();

  _impl->sx = wcfg.getScalingH()/100.;
  _impl->sy = wcfg.getScalingV()/100.;

  _impl->colorize = wcfg.getApplyColorization();
  _impl->colorize_strength = wcfg.getColorizationStrength();
  _impl->colorize_color = wcfg.getColorizationColor();

  _impl->use_font = wcfg.getUseFontInsteadOfSkin();
  _impl->font = wcfg.getFont();
  _impl->skin = wcfg.getSkin();
}

SettingsChangeListener::~SettingsChangeListener() = default;

void SettingsChangeListener::onOpacityChanged(int o)
{
  _wnd->setWindowOpacity(o/100.);
}

void SettingsChangeListener::onScalingHChanged(int s)
{
  _impl->sx = s/100.;
  _wnd->setScaling(_impl->sx, _impl->sy);
}

void SettingsChangeListener::onScalingVChanged(int s)
{
  _impl->sy = s/100.;
  _wnd->setScaling(_impl->sx, _impl->sy);
}

void SettingsChangeListener::onSpacingHChanged(int s)
{
  _wnd->clock()->setCharSpacing(s);
}

void SettingsChangeListener::onSpacingVChanged(int s)
{
  _wnd->clock()->setLineSpacing(s);
}

void SettingsChangeListener::onApplyColorizationChanged(bool en)
{
  _impl->colorize = en;
  if (_impl->colorize) {
    auto eff = new QGraphicsColorizeEffect();
    eff->setColor(_impl->colorize_color);
    eff->setStrength(_impl->colorize_strength);
    _wnd->setGraphicsEffect(eff);
  } else {
    _wnd->setGraphicsEffect(nullptr);
  }
}

void SettingsChangeListener::onColorizationStrengthChanged(qreal v)
{
  if (auto e = qobject_cast<QGraphicsColorizeEffect*>(_wnd->graphicsEffect()))
    e->setStrength(v);
  _impl->colorize_strength = v;
}

void SettingsChangeListener::onColorizationColorChanged(const QColor& c)
{
  if (auto e = qobject_cast<QGraphicsColorizeEffect*>(_wnd->graphicsEffect()))
    e->setColor(c);
  _impl->colorize_color = c;
}

void SettingsChangeListener::onUseFontInsteadOfSkinChanged(bool en)
{
  _impl->use_font = en;
  if (_impl->use_font)
    _wnd->clock()->setSkin(_app->skinManager().loadSkin(_impl->font));
  else
    _wnd->clock()->setSkin(_app->skinManager().loadSkin(_impl->skin));
}

void SettingsChangeListener::onFontChanged(const QFont& f)
{
  if (_impl->use_font)
    _wnd->clock()->setSkin(_app->skinManager().loadSkin(f));
  _impl->font = f;
}

void SettingsChangeListener::onSkinChanged(const QString& s)
{
  if (!_impl->use_font)
    _wnd->clock()->setSkin(_app->skinManager().loadSkin(s));
  _impl->skin = s;
}

void SettingsChangeListener::onSeparatorFlashesChanged(bool en)
{
  _wnd->clock()->setFlashSeparator(en);
}

void SettingsChangeListener::onUseCustomSeparatorsChanged(bool en)
{
  _wnd->clock()->setUseCustomSeparators(en);
}

void SettingsChangeListener::onCustomSeparatorsChanged(const QString& s)
{
  _wnd->clock()->setCustomSeparators(s);
}

void SettingsChangeListener::onTimeFormatChanged(const QString& s)
{
  _wnd->clock()->setFormat(s);
}

void SettingsChangeListener::onSecondsScaleFactorChanged(int s)
{
  _wnd->clock()->setSecondsScaleFactor(s/100.);
}

void SettingsChangeListener::onTextureChanged(const QBrush& tx)
{
  _wnd->clock()->setTexture(tx);
}

void SettingsChangeListener::onTextureStretchChanged(bool en)
{
  _wnd->clock()->setTextureStretch(en);
}

void SettingsChangeListener::onTexturePerCharacterChanged(bool en)
{
  _wnd->clock()->setTexturePerChar(en);
}

void SettingsChangeListener::onBackgroundChanged(const QBrush& bg)
{
  _wnd->clock()->setBackground(bg);
}

void SettingsChangeListener::onBackgroundStretchChanged(bool en)
{
  _wnd->clock()->setBackgroundStretch(en);
}

void SettingsChangeListener::onBackgroundPerCharacterChanged(bool en)
{
  _wnd->clock()->setBackgroundPerChar(en);
}
