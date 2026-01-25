/*
 * SPDX-FileCopyrightText: 2024-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QApplication>
#include <QIcon>

#include "core/clock_application.hpp"
#include "core/compat.hpp"
#include "version.hpp"

int main(int argc, char* argv[])
{
  QApplication::setApplicationName(QString::fromLatin1(APP_PROJECT_NAME));
  QApplication::setApplicationDisplayName("Digital Clock 5");
  QApplication::setApplicationVersion(QString::fromLatin1(APP_VERSION));
  QApplication::setOrganizationName("NickKorotysh");
  QApplication::setOrganizationDomain("kolcha.github.com");
  QApplication::setDesktopFileName(QString::fromLatin1(APP_IDENTIFIER));
  QApplication::setWindowIcon(QIcon(":/icons/clock.svg"));
#ifdef Q_OS_MACOS
  QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif
  QApplication::setQuitOnLastWindowClosed(false);
  QApplication::setStyle("fusion");

  QApplication app(argc, argv);
  compat::migrate();

  ClockApplication clock_app;
  app.installEventFilter(&clock_app);

  QObject::connect(&app, &QGuiApplication::screenAdded,
                   &clock_app, &ClockApplication::onScreenAdded);
  QObject::connect(&app, &QGuiApplication::screenRemoved, &clock_app,
                   &ClockApplication::onScreenRemoved);

  return app.exec();
}
