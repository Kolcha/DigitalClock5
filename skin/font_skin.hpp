#pragma once

#include "skin.hpp"

#include <QFont>

class FontSkin : public SkinBase {
public:
  explicit FontSkin(QFont font);

  class CharGlyph : public Glyph {
  public:
    CharGlyph(char32_t c, const QFont& font);

    QRectF rect() const override { return _br; }
    QPointF advance() const override { return _adv; }

    void draw(QPainter* p) const override;

  private:
    char32_t _ch;
    QRectF _br;
    QPointF _adv;
    const QFont& _font;
  };

protected:
  std::shared_ptr<Glyph> create(char32_t c) const override;

private:
  QFont _font;
};
