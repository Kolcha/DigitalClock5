/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QApplication>

#include "core/application.hpp"
#include "version.hpp"

int main(int argc, char *argv[])
{
  using namespace Qt::Literals::StringLiterals;

  QApplication::setApplicationName(QString::fromLatin1(APP_PROJECT_NAME));
  QApplication::setApplicationDisplayName(u"Digital Clock 5"_s);
  QApplication::setApplicationVersion(QString::fromLatin1(APP_VERSION));
  QApplication::setOrganizationName(u"NickKorotysh"_s);
  QApplication::setOrganizationDomain(u"kolcha.github.com"_s);
  QApplication::setDesktopFileName(QString::fromLatin1(APP_IDENTIFIER));
  QApplication::setWindowIcon(QIcon(":/icons/clock.svg"));
#ifdef Q_OS_MACOS
  QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif
  QApplication::setQuitOnLastWindowClosed(false);
  QApplication::setStyle(u"fusion"_s);

  Application app(argc, argv);
  return app.exec();
}
