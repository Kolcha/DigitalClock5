// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "skin_engine_global.hpp"

#include <memory>
#include <vector>

#include "item.hpp"

using LayoutItemsContainer = std::vector<std::unique_ptr<Item>>;

SKIN_ENGINE_EXPORT
// note: leaves Y unchanged
void layout_horizontally(LayoutItemsContainer& items, qreal spacing);

SKIN_ENGINE_EXPORT
// note: leaves X unchanged
void layout_vertically(LayoutItemsContainer& items, qreal spacing,
                       bool is_line_interval = false);
