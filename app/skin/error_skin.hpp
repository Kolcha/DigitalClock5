/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "skin/skin.hpp"

class ErrorSkin : public Skin
{
public:
  bool hasAlternateSeparator() const override { return true; }
  bool supportsCustomSeparator() const override { return false; }

protected:
  std::shared_ptr<Glyph> create(char32_t c) const override;
};
