/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "skin_manager.hpp"

#include "skin/font_skin.hpp"

std::unique_ptr<Skin> SkinManager::loadSkin(const QFont& font) const
{
  return std::make_unique<FontSkin>(font);
}

std::unique_ptr<Skin> SkinManager::loadSkin(const QString& id) const
{
  // TODO: implement
  return nullptr;
}
