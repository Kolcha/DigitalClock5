// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "font_skin.hpp"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>

#include "item.hpp"

namespace {

class TextRenderableBase : public Renderable
{
public:
  TextRenderableBase(const QFont& font, const QString& str)
    : _font(font)
    , _str(str)
  {}

  void draw(QPainter* p) const override
  {
    p->setFont(_font);
    p->drawText(0, 0, _str);
  }

private:
  const QFont& _font;
  QString _str;
};


class FragmentRenderable : public TextRenderableBase
{
public:
  FragmentRenderable(const QFont& font, const QString& str)
    : TextRenderableBase(font, str)
  {
    QFontMetricsF fm(font);
    _r = fm.tightBoundingRect(str);
  }

  QRectF geometry() const noexcept override { return _r.marginsAdded(_m); }

  void setMargins(QMarginsF m) noexcept { _m = std::move(m); }

private:
  QRectF _r;
  QMarginsF _m;
};


class SymbolRenderable : public TextRenderableBase
{
public:
  SymbolRenderable(const QFont& font, char32_t c)
    : TextRenderableBase(font, QString::fromUcs4(&c, 1))
  {
    QFontMetricsF fm(font);
    if (c <= 0xFFFF) {
      _br = fm.boundingRect(QChar(c));
      _adv = fm.horizontalAdvance(QChar(c));
    } else {
      _br = fm.tightBoundingRect(QString::fromUcs4(&c, 1));
      _adv = (font.italic() ? 0.8 : 1.0) * _br.width();
    }
    _br.adjust(-0.5, -0.5, 0.5, 0.5);
  }

  QRectF geometry() const noexcept override { return _br; }

  qreal advance() const noexcept { return _adv; }

private:
  QRectF _br;
  qreal _adv = 0;
};

} // namespace


std::unique_ptr<Renderable> FontSkin::draw(const QString& str) const
{
  return perCharRendering() ? drawPerChar(str) : drawFragment(str);
}

Skin::Metrics FontSkin::metrics() const
{
  QFontMetricsF fm(_font);
  return {
    .ascent = fm.ascent(),
    .descent = fm.descent(),
    .line_spacing = fm.lineSpacing(),
  };
}

std::unique_ptr<Renderable> FontSkin::drawPerChar(const QString& str) const
{
  auto r = std::make_unique<CompositeRenderable>();

  const auto cps = str.toUcs4();

  qreal x = 0;
  for (const auto c : cps) {
    auto sym = std::make_unique<SymbolRenderable>(_font, static_cast<char32_t>(c));
    auto adv = sym->advance();
    auto ssr = std::make_unique<SkinRenderable>(std::move(sym));
    ssr->setTexture(texture(), textureRules());
    ssr->setBackground(background(), backgroundRules());

    auto item = std::make_unique<Item>(std::move(ssr));
    item->setMargins(charMargins());
    item->setPos({x, 0});
    x += adv + charSpacing();

    r->add(std::move(item));
  }

  return r;
}

std::unique_ptr<Renderable> FontSkin::drawFragment(const QString& str) const
{
  auto fr = std::make_unique<FragmentRenderable>(_font, str);
  fr->setMargins(charMargins());
  auto sr = std::make_unique<SkinRenderable>(std::move(fr));
  sr->setTexture(texture(), textureRules());
  sr->setBackground(background(), backgroundRules());
  return sr;
}
