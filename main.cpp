/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QApplication>

#include "app/application.hpp"

int main(int argc, char *argv[])
{
  Application app(argc, argv);

  QApplication::setQuitOnLastWindowClosed(false);

  return app.exec();
}
