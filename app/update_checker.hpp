/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QObject>

#include <QNetworkAccessManager>
#include <QVersionNumber>

class UpdateChecker : public QObject
{
  Q_OBJECT

public:
  explicit UpdateChecker(QObject* parent = nullptr);

signals:
  void newVersion(QVersionNumber version, QDate date, QUrl link);
  void upToDate();
  void errorMessage(QString msg);

public slots:
  void checkForUpdates();
  void setCheckForBeta(bool check) noexcept;

private:
  void runCheckForUpdates(bool force);
  void sendRequest(const QUrl& url);
  void readReply(QNetworkReply* reply);

private:
  bool _check_beta;
  bool _is_running;

  QNetworkAccessManager _net_access_manager;
};
