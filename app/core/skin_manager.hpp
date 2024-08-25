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
  SkinManager();
  ~SkinManager();

  std::unique_ptr<Skin> loadSkin(const QFont& font) const;
  std::unique_ptr<Skin> loadSkin(const QString& id) const;

  // returns list of ids, not titles
  QStringList availableSkins() const;

  using Metadata = QHash<QString, QString>;
  Metadata metadata(const QString& id) const;

  // skin base size (affects not all skins)
  void setSkinBaseSize(int sz);

private:
  struct Impl;
  std::unique_ptr<Impl> _impl;
};
