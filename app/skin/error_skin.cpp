/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "error_skin.hpp"

#include <QFontDatabase>
#include <QPainter>

namespace {

class ErrorGlyph : public Skin::Glyph
{
public:
  explicit ErrorGlyph(bool vis) noexcept
      : _r(0, 0, 240, 100)
      , _vis(vis)
  {}

  QRectF rect() const override { return _r; }
  QPointF advance() const override { return {_r.width(), -_r.height()}; }

  void draw(QPainter* p) const override;

private:
  QRectF _r;
  bool _vis = true;
};

void ErrorGlyph::draw(QPainter* p) const
{
  p->save();
  p->drawRoundedRect(_r.marginsRemoved({5, 5, 5, 5}), 8, 8);
  auto fnt = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  p->setPen(QColor(0x8C, 0x8A, 0xFF));
  fnt.setPointSize(14);
  p->setFont(fnt);
  p->drawText(_r.adjusted(0, 10, 0, -(_r.height() - 10 - 20)), "Digital Clock 5", {Qt::AlignCenter});
  if (_vis) {
    fnt.setPointSize(36);
    p->setPen(QColor(0xF0, 0x40, 0x58));
    p->setFont(fnt);
    p->drawText(_r, "ERROR", {Qt::AlignCenter});
  }
  fnt.setPointSize(14);
  p->setFont(fnt);
  p->drawText(_r.adjusted(0, _r.height() - 10 - 20, 0, -10), "Skin Not Loaded", {Qt::AlignCenter});
  p->restore();
}

} // namespace

std::shared_ptr<Skin::Glyph> ErrorSkin::create(char32_t c) const
{
  if (c == ':' || c == ' ') {
    return std::make_shared<ErrorGlyph>(c == ':');
  }
  return nullptr;
}
