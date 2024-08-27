/*
 * SPDX-FileCopyrightText: 2015-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "autostart.h"

#include <QCoreApplication>
#include <QDir>
#include <QSettings>

static const char* const REG_RUN_KEY = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";

static bool isPortable()
{
  using namespace Qt::Literals::StringLiterals;
  QDir app_dir(QCoreApplication::applicationDirPath());
  return app_dir.exists(u"portable.txt"_s) || app_dir.exists(u".portable"_s);
}

static QString GetAppKey()
{
  QString key = "Digital Clock Next";   // unfortunately, installer uses spaces :(
  if (isPortable()) {
    key = QCoreApplication::applicationName();
    quint16 checksum = qChecksum(QCoreApplication::applicationFilePath().toUtf8());
    key.remove(QLatin1Char(' '));
    key += QStringLiteral("-") + QString::number(checksum);
  }
  return key;
}

bool IsAutoStartEnabled()
{
  QSettings settings(REG_RUN_KEY, QSettings::NativeFormat);
  return settings.value(GetAppKey()).toString() == QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
}

void SetAutoStart(bool enable)
{
  QSettings settings(REG_RUN_KEY, QSettings::NativeFormat);
  if (enable) {
    settings.setValue(GetAppKey(), QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
  } else {
    settings.remove(GetAppKey());
  }
}
