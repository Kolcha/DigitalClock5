/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "font_skin.hpp"

#include <QFontMetrics>

#include <QPainter>

FontSkin::FontSkin(QFont font)
    : _font(std::move(font))
{
}

FontSkin::CharGlyph::CharGlyph(char32_t c, const QFont& font)
    : _ch(c)
    , _font(font)
{
  QFontMetricsF fmf(font);
  if (c <= 0xFFFF) {
    _br = fmf.boundingRect(QChar(c));
    _adv.rx() = fmf.horizontalAdvance(QChar(c));
  } else {
    _br = fmf.tightBoundingRect(QString::fromUcs4(&c, 1));
    _adv.rx() = (_font.italic() ? 0.8 : 1.0) * _br.width();
  }
  _br.adjust(-0.5, -0.5, 0.5, 0.5);
  _adv.ry() = -fmf.lineSpacing();
}

void FontSkin::CharGlyph::draw(QPainter* p) const
{
  p->save();
  p->setFont(_font);
  p->drawText(0, 0, QString::fromUcs4(&_ch, 1));
  p->restore();
}

std::shared_ptr<Skin::Glyph> FontSkin::create(char32_t c) const
{
  return std::make_shared<FontSkin::CharGlyph>(c, _font);
}
