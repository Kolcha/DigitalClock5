/*
 * SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "autostart_mac_migration.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>

#include "autostart.h"

static QString GetInstanceSuffix()
{
  quint16 checksum = qChecksum(QCoreApplication::applicationFilePath().toUtf8());
  return QStringLiteral("-") + QString::number(checksum);
}

static QString GetAutoStartDir()
{
  return QDir::home().absoluteFilePath("Library/LaunchAgents");
}

static QString GetPackageName()
{
  QStringList domain_parts = QCoreApplication::organizationDomain().split('.', Qt::SkipEmptyParts);
  std::reverse(domain_parts.begin(), domain_parts.end());
  return domain_parts.join('.');
}

static QString GetPlistFile()
{
  return QDir(GetAutoStartDir()).absoluteFilePath(GetPackageName() + GetInstanceSuffix() + ".plist");
}

namespace compat {

void migrate_mac_autostart()
{
  if (auto plist_file = GetPlistFile(); QFile::exists(plist_file)) {
    QFile::remove(plist_file);
    SetAutoStart(true);
  }
}

} // namespace compat
