/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "container.hpp"

ContainerImplBase::ContainerImplBase(std::shared_ptr<Algorithm> algo)
    : _algo(std::move(algo))
{
}

void ContainerImplBase::draw(QPainter* p) const
{
  for (const auto& g : _glyphs) g->draw(p);
}

void ContainerImplBase::addGlyph(std::shared_ptr<::Glyph> g)
{
  _glyphs.push_back(std::move(g));
}

void ContainerImplBase::updateGeometry()
{
  _rect = {};
  _adv = {};

  if (!_algo) return;
  if (_glyphs.empty()) return;

  _algo->process(_glyphs);
  _rect = _algo->rect();
  _adv = _algo->advance();
}


void LinearLayout::process(const GlyphList& glyphs)
{
  _rect = {};
  _adv = {};

  if (glyphs.empty()) return;

  glyphs.front()->setPos({0, 0});
  _rect = glyphs.front()->rect();
  _adv = glyphs.front()->advance();

  for (size_t i = 1; i < glyphs.size(); i++) {
    auto& curr = *glyphs[i];
    auto& prev = *glyphs[i-1];

    updatePos(curr, prev);
    updateAdv(_adv, curr);

    _rect |= curr.geometry();
  }

  applyAlignment(glyphs);
}


void HLayout::updatePos(Glyph& curr, const Glyph& prev) const
{
  curr.setPos(prev.pos() + QPointF(prev.advance().x() + spacing(), 0));
}

void HLayout::updateAdv(QPointF& adv, const Glyph& curr) const
{
  adv.rx() += curr.advance().x() + spacing();
  adv.ry() = std::max(adv.y(), curr.advance().y());
}

void HLayout::applyAlignment(const GlyphList& glyphs) const
{
  std::function<qreal(Glyph&)> ypos = [](auto& g) { return g.pos().y(); };

  switch (alignment() & Qt::AlignVertical_Mask) {
    case Qt::AlignTop:
      ypos = [](auto& g) { return 0; };
      break;

    case Qt::AlignVCenter:
      ypos = [this](auto& g) { return rect().center().y() - g.rect().center().y(); };
      break;

    case Qt::AlignBottom:
      ypos = [this](auto& g) { return rect().bottom() - g.rect().bottom(); };
      break;

    default:
      break;
  }

  for (auto& g : glyphs) { g->setPos({g->pos().x(), ypos(*g)}); }
}


void VLayout::updatePos(Glyph& curr, const Glyph& prev) const
{
  curr.setPos(prev.pos() + QPointF(0, -curr.advance().y() + spacing()));
}

void VLayout::updateAdv(QPointF& adv, const Glyph& curr) const
{
  adv.ry() += curr.advance().y() + spacing();
  adv.rx() = std::max(adv.x(), curr.advance().x());
}

void VLayout::applyAlignment(const GlyphList& glyphs) const
{
  std::function<qreal(Glyph&)> xpos = [](auto& g) { return g.pos().x(); };

  switch (alignment() & Qt::AlignHorizontal_Mask) {
    case Qt::AlignLeft:
      xpos = [](auto& g) { return 0; };
      break;

    case Qt::AlignHCenter:
      xpos = [this](auto& g) { return rect().center().x() - g.rect().center().x(); };
      break;

    case Qt::AlignRight:
      xpos = [this](auto& g) { return rect().right() - g.advance().x(); };
      break;

    default:
      break;
  }

  for (auto& g : glyphs) { g->setPos({xpos(*g), g->pos().y()}); }
}
