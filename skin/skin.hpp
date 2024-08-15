#pragma once

#include <memory>

#include <QHash>
#include <QPoint>
#include <QRect>

class QPainter;

class Skin {
public:
  virtual ~Skin() = default;

  class Glyph {
  public:
    virtual ~Glyph() = default;

    virtual QRectF rect() const = 0;
    // dy should be negative as Y points to down
    virtual QPointF advance() const = 0;

    virtual void draw(QPainter* p) const = 0;
  };

  virtual std::shared_ptr<Glyph> glyph(char32_t c) const = 0;
};


class SkinBase : public Skin {
public:
  std::shared_ptr<Glyph> glyph(char32_t c) const override
  {
    auto& cached_glyph = _cache[c];
    if (!cached_glyph) cached_glyph = create(c);
    return cached_glyph;
  }

protected:
  virtual std::shared_ptr<Glyph> create(char32_t c) const = 0;

private:
  mutable QHash<char32_t, std::shared_ptr<Glyph>> _cache;
};
