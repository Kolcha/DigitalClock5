// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "clock_common_global.hpp"

#include <memory>

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>

class Skin;

struct SkinInfo {
  QString id;               // internal id, never empty
  QString name;             // human-readable name, should not be empty
  QHash<QString, QString> metadata;
  bool user_provided;       // skin is user-provided skin
};

using SkinInfoList = QVector<SkinInfo>;

class CLOCK_COMMON_EXPORT SkinManager
{
public:
  virtual ~SkinManager() = default;

  // returns list of available skin ids
  virtual QStringList list() const = 0;
  // returns list of available skins as list of SkinInfo
  virtual SkinInfoList infoList() const = 0;

  virtual SkinInfo info(const QString& id) const = 0;

  virtual std::unique_ptr<Skin> load(const QString& id) const = 0;
};
