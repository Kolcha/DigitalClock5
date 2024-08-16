/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <memory>

#include <QFont>

#include "skin/skin.hpp"

class SkinManager {
public:
  std::unique_ptr<Skin> loadSkin(const QFont& font) const;
  std::unique_ptr<Skin> loadSkin(const QString& id) const;
};
