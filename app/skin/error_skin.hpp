// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "renderer/skin_base.hpp"

class ErrorSkin : public SkinBase
{
public:
  std::unique_ptr<Renderable> draw(const QString& str) const override;

  bool supports(char32_t ch) const noexcept override { return true; }

  Metrics metrics() const override;
};
