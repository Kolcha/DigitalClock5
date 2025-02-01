// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "skin_engine_global.hpp"

#include "skin_base.hpp"

#include <QtGui/QFont>

class SKIN_ENGINE_EXPORT FontSkin : public SkinBase
{
public:
  explicit FontSkin(const QFont& font) noexcept
    : _font(std::move(font))
  {}

  std::unique_ptr<Renderable> draw(const QString &str) const override;

  // don't do any checks, assume any character can be rendered
  bool supports(char32_t ch) const noexcept override { return true; }

  Metrics metrics() const override;

private:
  std::unique_ptr<Renderable> drawPerChar(const QString& str) const;
  std::unique_ptr<Renderable> drawFragment(const QString& str) const;

private:
  QFont _font;
};
