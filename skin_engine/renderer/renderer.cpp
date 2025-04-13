// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "renderer.hpp"

#include <limits>

#include "layout.hpp"
#include "skin_base.hpp"

void LinesRenderer::Options::setLineScalingEnabled(size_t idx, bool enable)
{
  if (enable) {
    _scalable_lines.insert(idx);
  } else {
    _scalable_lines.remove(idx);
  }
}

Qt::Alignment LinesRenderer::Options::lineAlignment(size_t idx) const
{
  return _line_alignment.value(idx, _def_alignment);
}


namespace {

template<class Iter>
Iter find_first_non_scalable_item(Iter first, Iter last, const LinesRenderer::Options& opt)
{
  for (auto iter = first; iter != last; ++iter) {
    if (!opt.scalingEnabled(std::distance(first, iter))) {
      return iter;
    }
  }
  return last;
}

std::pair<qreal, qreal> find_min_max_x(const LayoutItemsContainer& items,
                                       const LinesRenderer::Options& opt,
                                       bool ignore_scalable)
{
  // this is ugly, but significantly simplifies initialization...
  qreal min_x = std::numeric_limits<qreal>::max();
  qreal max_x = std::numeric_limits<qreal>::lowest();

  for (size_t i = 0; i < items.size(); i++) {
    if (opt.scalingEnabled(i) && ignore_scalable)
      continue;

    const auto& r = items[i]->rect();
    if (r.left() < min_x)
      min_x = r.left();
    if (r.right() > max_x)
      max_x = r.right();
  }

  return {min_x, max_x};
}

bool isEmptyLine(const Item& item)
{
  const auto& r = item.rect();
  return r.isEmpty() || qFuzzyIsNull(r.width());
}

void align_items(LayoutItemsContainer& items, const LinesRenderer::Options& opt,
                 qreal min_x, qreal max_x)
{
  const qreal max_w = max_x - min_x;

  for (size_t i = 0; i < items.size(); i++) {
    if (opt.scalingEnabled(i) && !isEmptyLine(*items[i]))
      continue;

    auto& item = *items[i];
    const auto& r = item.rect();
    auto pos = item.pos();

    switch (opt.lineAlignment(i) & Qt::AlignHorizontal_Mask) {
      case Qt::AlignLeft:
        pos.setX(0);
        break;
      case Qt::AlignHCenter:
        pos.setX(min_x + (max_w - r.width()) / 2 - r.left());
        break;
      case Qt::AlignRight:
        pos.setX(max_x - r.right());
        break;
    }

    item.setPos(std::move(pos));

    const auto& g = item.geometry();
    item.setMargins({g.left() - min_x, 0, max_x - g.right(), 0});
  }
}

void scale_items(LayoutItemsContainer& items, const LinesRenderer::Options& opt,
                 qreal min_x, qreal max_x)
{
  const qreal max_w = max_x - min_x;

  for (size_t i = 0; i < items.size(); i++) {
    if (!opt.scalingEnabled(i) || isEmptyLine(*items[i]))
      continue;

    auto& item = *items[i];
    const auto& r = item.rect();
    auto pos = item.pos();

    const qreal k = max_w / r.width();
    item.setTransform(QTransform::fromScale(k, k));

    const auto& g = item.geometry();
    pos.setX(pos.x() + min_x - g.left());
    item.setPos(std::move(pos));
  }
}


class MarginsAddedRenderable : public RenderableDecorator
{
public:
  using RenderableDecorator::RenderableDecorator;

  QRectF geometry() const override { return inner()->geometry().marginsAdded(_m); }

  void setMargins(QMarginsF m) noexcept { _m = std::move(m); }

private:
  QMarginsF _m;
};

} // namespace


std::unique_ptr<Renderable> LinesRenderer::draw(Lines&& lines, const Options& opt) const
{
  if (lines.empty())
    return nullptr;

  LayoutItemsContainer items(lines.size());
  // *INDENT-OFF*
  std::transform(lines.begin(), lines.end(), items.begin(),
                 [](auto&& l) { return std::make_unique<Item>(std::move(l)); });
  // *INDENT-ON*

  auto first_non_scalable = find_first_non_scalable_item(items.begin(), items.end(), opt);
  auto [min_x, max_x] = find_min_max_x(items, opt, first_non_scalable != items.end());

  scale_items(items, opt, min_x, max_x);
  align_items(items, opt, min_x, max_x);

  qreal spacing = _line_spacing;
  if (_respect_line_spacing)
    spacing += _skin_metrics.line_spacing;
  layout_vertically(items, spacing, _respect_line_spacing);

  auto r = std::make_unique<CompositeRenderable>();
  for (auto&& i : items) {
    r->add(customize(std::move(i), _tx_ct == PerLine, _bg_ct == PerLine));
  }
  auto cr = customize(std::move(r), _tx_ct == AllText, _bg_ct == AllText);
  auto mr = std::make_unique<MarginsAddedRenderable>(std::move(cr));
  mr->setMargins(_text_margins);
  return mr;
}

void LinesRenderer::setTextureCustomization(CustomizationType c)
{
  _tx_ct = c;
  updateSkinTexture();
}

void LinesRenderer::setBackgroundCustomization(CustomizationType c)
{
  _bg_ct = c;
  updateSkinBackground();
}

void LinesRenderer::setTexture(QPixmap p, QTileRules t)
{
  _texture = std::move(p);
  _tx_rules = std::move(t);
  updateSkinTexture();
}

void LinesRenderer::setBackground(QPixmap p, QTileRules t)
{
  _background = std::move(p);
  _bg_rules = std::move(t);
  updateSkinBackground();
}

void LinesRenderer::setCharSpacing(qreal s)
{
  if (_skin)
    _skin->setCharSpacing(s);
  _char_spacing = s;
}

void LinesRenderer::setTruePerCharRendering(bool en)
{
  if (_skin)
    _skin->setPerCharRendering(en);
  _true_per_char = en;
}

void LinesRenderer::setCharMargins(QMarginsF m)
{
  if (_skin)
    _skin->setCharMargins(m);
  _char_margins = std::move(m);
}

void LinesRenderer::setTextMargins(QMarginsF m)
{
  _text_margins = std::move(m);
}

void LinesRenderer::setSkin(std::shared_ptr<Skin> skin)
{
  _skin = std::move(skin);
  _skin_metrics = {};
  updateSkinSettings();
  onSkinChange();
}

std::unique_ptr<Renderable> LinesRenderer::buildEmptyLinePlaceholder() const
{
  QPointF tl(0, -_skin_metrics.ascent);
  QPointF br(0, _skin_metrics.descent);
  return std::make_unique<NoopRenderable>(QRectF(tl, br));
}

void LinesRenderer::updateSkinTexture()
{
  if (!_skin)
    return;

  if (_tx_ct == PerChar)
    _skin->setTexture(_texture, _tx_rules);
  else
    _skin->setTexture({}, {});
}

void LinesRenderer::updateSkinBackground()
{
  if (!_skin)
    return;

  if (_bg_ct == PerChar)
    _skin->setBackground(_background, _bg_rules);
  else
    _skin->setBackground({}, {});
}

void LinesRenderer::updateSkinSettings()
{
  if (!_skin)
    return;

  updateSkinTexture();
  updateSkinBackground();

  _skin->setPerCharRendering(_true_per_char);
  _skin->setCharSpacing(_char_spacing);
  _skin->setCharMargins(_char_margins);

  _skin_metrics = _skin->metrics();
}

std::unique_ptr<Renderable> LinesRenderer::customize(std::unique_ptr<Renderable> r,
                                                     bool set_texture,
                                                     bool set_background) const
{
  if (!set_texture && !set_background)
    return std::move(r);

  auto sr = std::make_unique<SkinRenderable>(std::move(r));
  if (set_texture)
    sr->setTexture(_texture, _tx_rules);
  if (set_background)
    sr->setBackground(_background, _bg_rules);
  return sr;
}


std::unique_ptr<Renderable> TextRenderer::draw(const QString& txt, const Options& opt) const
{
  const auto s = skin();
  if (!s)
    return nullptr;

  const auto tlines = txt.split(QChar('\n'));

  Lines rlines;
  rlines.reserve(tlines.size());

  for (const auto& l : tlines) {
    if (auto rl = s->draw(l); rl && !l.isEmpty()) {
      rlines.push_back(std::move(rl));
    } else {
      rlines.push_back(buildEmptyLinePlaceholder());
    }
  }

  return LinesRenderer::draw(std::move(rlines), opt);
}
