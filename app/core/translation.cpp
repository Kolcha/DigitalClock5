/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "translation.hpp"

#include <QLocale>

std::unique_ptr<QTranslator> findTranslation(QStringView module)
{
  for (QString locale : QLocale::system().uiLanguages()) {
    locale = QLocale(locale).name();
    if (locale == QLatin1String("C") ||               // overrideLanguage == "English"
        locale.startsWith(QLatin1String("en")))       // "English" is built-in
      break;                                          // use built-in

    if (locale.contains("ua", Qt::CaseInsensitive))   // Ukrainian,
      locale = QLatin1String("ru_RU");                // use Russian

    if (auto translator = loadTranslation(module, locale))
      return translator;
  }
  return nullptr;
}

std::unique_ptr<QTranslator> loadTranslation(QStringView module, QStringView locale)
{
  auto translator = std::make_unique<QTranslator>();
  if (translator->load(QString(":/i18n/i18n/%1_%2").arg(module, locale)))
    return translator;

  return nullptr;
}
