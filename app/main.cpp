/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QApplication>

#include <QLibraryInfo>
#include <QTranslator>

#include "core/application.hpp"
#include "core/translation.hpp"
#include "version.hpp"

int main(int argc, char *argv[])
{
  using namespace Qt::Literals::StringLiterals;
  // set system icon theme as fallback
  auto system_theme = QIcon::themeName();
  QIcon::setThemeName(u"unicons-line"_s);
  QIcon::setFallbackThemeName(system_theme);

  QApplication::setApplicationName(QString::fromLatin1(APP_PROJECT_NAME));
  QApplication::setApplicationDisplayName(u"Digital Clock 5"_s);
  QApplication::setApplicationVersion(QString::fromLatin1(APP_VERSION));
  QApplication::setOrganizationName(u"NickKorotysh"_s);
  QApplication::setOrganizationDomain(u"kolcha.github.com"_s);
  QApplication::setDesktopFileName(QString::fromLatin1(APP_IDENTIFIER));
  QApplication::setWindowIcon(QIcon::fromTheme("clock"));
#ifdef Q_OS_MACOS
  QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif
  QApplication::setQuitOnLastWindowClosed(false);
#ifdef Q_OS_WINDOWS
  QApplication::setStyle(u"windowsvista"_s);
#else
  QApplication::setStyle(u"fusion"_s);
#endif
  QApplication app(argc, argv);

  // install app translators
  auto app_translator = findTranslation(u"digital_clock");
  auto core_translator = findTranslation(u"plugin_core");
  QTranslator qt_translator;

  if (app_translator) {
    QApplication::installTranslator(app_translator.get());

    if (core_translator)
      QApplication::installTranslator(core_translator.get());

    if (qt_translator.load(QLatin1String("qt_") + app_translator->language(),
                           QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
      QApplication::installTranslator(&qt_translator);
  }

  Application clock_app(app);

  return app.exec();
}
