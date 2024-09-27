/*
 * SPDX-FileCopyrightText: 2020-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <QThread>

class SystemThemeTracker : public QThread
{
  Q_OBJECT

public:
  explicit SystemThemeTracker(QObject* parent = nullptr);
  ~SystemThemeTracker();

  bool isLightTheme() const;
  bool isDarkTheme() const { return !isLightTheme(); }

public slots:
  void start();
  void stop();

protected:
  void run() override;

signals:
  void themeChanged(bool is_light);

private:
  struct Impl;
  std::unique_ptr<Impl> _impl;
};
