/*
 * SPDX-FileCopyrightText: 2013-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "plugin_info_dialog.hpp"
#include "ui_plugin_info_dialog.h"

#include <QFileInfo>

namespace {

QDateTime lastModified(const QString& filename)
{
  return QFileInfo(filename).lastModified();
}

QString copyrightString(const PluginInfo& info)
{
  const auto& md = info.metadata;

  if (auto iter = md.find("copyright"); iter != md.end())
    return iter.value().toString();

  if (auto iter = md.find("author"); iter != md.end())
    return QString("(c) %1 %2")
        .arg(lastModified(info.path).date().year())
        .arg(iter.value().toString());

  return {};
}

QIcon pluginIcon(const PluginInfo& info)
{
  const auto known_names = {
      QString(":/icons/%1").arg(info.id),
      QString(":/%1/icon").arg(info.id),
      QString(":/%1/logo").arg(info.id),
  };

  const auto known_exts = {"svg", "png"};

  for (const auto& p : known_names) {
    for (const auto& e : known_exts) {
      auto fn = QString("%1.%2").arg(p, e);
      if (QFile::exists(fn)) {
        return QIcon(fn);
      }
    }
  }

  return QIcon::fromTheme("plugins");
}

} // namespace

PluginInfoDialog::PluginInfoDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::PluginInfoDialog)
{
  ui->setupUi(this);

  auto fnt = font();
  fnt.setBold(true);
  ui->version_value->setFont(fnt);
  fnt.setPointSizeF(fnt.pointSizeF() * 4/3);
  ui->name_value->setFont(fnt);

  fnt = font();
  fnt.setPointSizeF(fnt.pointSizeF() * 5/6);
  ui->description_value->setFont(fnt);
}

PluginInfoDialog::~PluginInfoDialog()
{
  delete ui;
}

void PluginInfoDialog::SetInfo(const PluginInfo& info)
{
  ui->icon_label->setPixmap(pluginIcon(info).pixmap(96));
  ui->name_value->setText(info.title);
  ui->version_value->setText(
      tr("version: %1")
          .arg(info.metadata.value("version", tr("unknown")).toString()));
  ui->description_value->setText(info.description);
  ui->copyright_label->setText(copyrightString(info));
}