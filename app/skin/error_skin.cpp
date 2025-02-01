// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "error_skin.hpp"

#include <QtGui/QFontDatabase>
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>

namespace {

constexpr const int base_w = 40;
constexpr const int base_h = 80;

class ErrorRenderable : public Renderable
{
public:
  explicit ErrorRenderable(int n) noexcept
    : _r(0, -base_h, n * base_w, base_h)
  {}

  QRectF geometry() const noexcept override { return _r; }

  void draw(QPainter* p) const override;

private:
  QRectF _r;
};

void ErrorRenderable::draw(QPainter* p) const
{
  auto pen = p->pen();
  pen.setWidth(2);
  pen.setCapStyle(Qt::RoundCap);
  pen.setJoinStyle(Qt::RoundJoin);
  p->setPen(pen);

  auto r = _r.adjusted(2, 2, -2, -2);
  p->drawRect(r);

  auto fnt = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  fnt.setPointSize(12);
  p->setFont(fnt);

  QString txt = "Error";
  QFontMetricsF fm(fnt);
  if (r.width() > 1.2*fm.boundingRect(txt).width()) {
    p->drawText(r, txt, {Qt::AlignCenter});
  } else {
    p->drawLine(r.topLeft(), r.bottomRight());
    p->drawLine(r.bottomLeft(), r.topRight());
  }
}

} // namespace

std::unique_ptr<Renderable> ErrorSkin::draw(const QString& str) const
{
  auto er = std::make_unique<ErrorRenderable>(str.toUcs4().size());
  auto sr = std::make_unique<SkinRenderable>(std::move(er));
  sr->setTexture(texture(), textureRules());
  sr->setBackground(background(), backgroundRules());
  return sr;
}

Skin::Metrics ErrorSkin::metrics() const
{
  return {
    .ascent = base_h,
    .descent = 0,
    .line_spacing = base_h,
  };
}
