// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "layout.hpp"

void layout_horizontally(LayoutItemsContainer& items, qreal spacing)
{
  if (items.empty())
    return;

  items.front()->setPos({0, items.front()->pos().y()});

  auto prev = items.begin();
  for (auto curr = std::next(prev); curr != items.end(); ++curr) {
    auto& prev_item = **prev;
    auto& curr_item = **curr;
    qreal x = prev_item.geometry().right() + spacing - curr_item.rect().left();
    curr_item.setPos({x, curr_item.pos().y()});
    prev = curr;
  }
}

static qreal current_y_pos(const Item& prev, const Item& curr,
                           qreal spacing,
                           bool is_line_interval)
{
  return is_line_interval ?
         prev.pos().y() + spacing :
         prev.geometry().bottom() + spacing - curr.rect().top();
}

void layout_vertically(LayoutItemsContainer& items, qreal spacing, bool is_line_interval)
{
  if (items.empty())
    return;

  items.front()->setPos({items.front()->pos().x(), 0});

  auto prev = items.begin();
  for (auto curr = std::next(prev); curr != items.end(); ++curr) {
    auto& prev_item = **prev;
    auto& curr_item = **curr;
    qreal y = current_y_pos(prev_item, curr_item, spacing, is_line_interval);
    curr_item.setPos({curr_item.pos().x(), y});
    prev = curr;
  }
}
