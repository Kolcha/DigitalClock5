/*
 * SPDX-FileCopyrightText: 2015-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "autostart.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QRegularExpression>

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

static QString GetAutoStartCmd()
{
  return QCoreApplication::applicationFilePath();
}

static bool IsAutoStartFileHasCorrectCmd(const QString& path, const QString& cmd)
{
  QFile f(path);

  if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString data(f.readAll());
    f.close();

    QRegularExpression re(QString("^\\s*<string>%1<\\/string>$").arg(QRegularExpression::escape(cmd)),
                          QRegularExpression::MultilineOption);
    QRegularExpressionMatch m = re.match(data);
    return m.hasMatch();
  }

  return false;
}


bool IsAutoStartEnabled()
{
  return IsAutoStartFileHasCorrectCmd(GetPlistFile(), GetAutoStartCmd());
}

void SetAutoStart(bool enable)
{
  QString plist_file = GetPlistFile();
  if (enable) {
    QString autostart_cmd = GetAutoStartCmd();
    if (IsAutoStartFileHasCorrectCmd(plist_file, autostart_cmd)) return;
    QString startup_dir = GetAutoStartDir();
    if (!QFile::exists(startup_dir)) QDir::home().mkpath(startup_dir);
    // *INDENT-OFF*
    QString plist_data = QString(
          "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
          "<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
          "<plist version=\"1.0\">\n"
          "<dict>\n"
          "    <key>Label</key>\n"
          "    <string>%1</string>\n"
          "    <key>Program</key>\n"
          "    <string>%2</string>\n"
          "    <key>RunAtLoad</key>\n"
          "    <true/>\n"
          "</dict>\n"
          "</plist>\n")
        .arg(GetPackageName(), autostart_cmd);
    // *INDENT-ON*
    QFile file(plist_file);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(plist_data.toLocal8Bit());
    file.close();
  } else {
    QFile::remove(plist_file);
  }
}
