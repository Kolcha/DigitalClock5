/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "update_checker.hpp"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "build_date.hpp"

UpdateChecker::UpdateChecker(QObject* parent)
  : QObject(parent)
  , _check_beta(false)
  , _is_running(false)
{
  connect(&_net_access_manager, &QNetworkAccessManager::finished, this, &UpdateChecker::readReply);
}

void UpdateChecker::checkForUpdates()
{
  runCheckForUpdates(true);
}

void UpdateChecker::setCheckForBeta(bool check) noexcept
{
  _check_beta = check;
}

void UpdateChecker::runCheckForUpdates(bool force)
{
  Q_UNUSED(force);
  if (_is_running)
    return;

  sendRequest(QUrl("https://digitalclock4.sourceforge.io/update/"));
}

void UpdateChecker::sendRequest(const QUrl& url)
{
  _is_running = true;
  auto ua = QString("%1/%2").arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion());
  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::UserAgentHeader, ua);
  _net_access_manager.get(req);
}

void UpdateChecker::readReply(QNetworkReply* reply)
{
  _is_running = false;
  reply->deleteLater();

  if (reply->error()) {
    emit errorMessage(reply->errorString());
    return;
  }

  QByteArray data = reply->readAll();

  QJsonParseError err;
  QJsonDocument js_doc = QJsonDocument::fromJson(data, &err);
  if (err.error != QJsonParseError::NoError) {
    emit errorMessage(err.errorString());
    return;
  }

  QJsonObject js_obj = js_doc.object().value("stable").toObject();
  QVersionNumber latest = QVersionNumber::fromString(js_obj.value("version").toString());
  QDate last_build = QDate::fromString(js_obj.value("timestamp").toString().simplified(), "dd-MM-yyyy");
  QString link = js_obj.value("download").toString();

  if (_check_beta) {
    js_obj = js_doc.object().value("testing").toObject();
    QString t_version = js_obj.value("version").toString("-");
    if (t_version != "-") {
      latest = QVersionNumber::fromString(t_version);
      last_build = QDate::fromString(js_obj.value("timestamp").toString().simplified(), "dd-MM-yyyy");
      link = js_obj.value("download").toString();
    }
  }

  if (latest > QVersionNumber::fromString(QCoreApplication::applicationVersion()) || build_date() < last_build) {
    emit newVersion(latest, last_build, {link});
  } else {
    emit upToDate();
  }
}
