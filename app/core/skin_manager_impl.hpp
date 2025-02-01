// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "common/skin_manager.hpp"

class SkinManagerImpl : public SkinManager
{
public:
  SkinManagerImpl();
  ~SkinManagerImpl();

  QStringList list() const override;
  SkinInfoList infoList() const override;

  SkinInfo info(const QString& id) const override;

  std::unique_ptr<Skin> load(const QString& id) const override;

  // skin base size (affects not all skins)
  void setSkinBaseSize(int sz);

private:
  struct Impl;
  std::unique_ptr<Impl> _impl;
};
