/*
 * SPDX-FileCopyrightText: 2013-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "plugin_info_dialog.hpp"
#include "ui_plugin_info_dialog.h"

#include <QFileInfo>

#include "core/plugin_manager_impl.hpp"

namespace {

QDateTime lastModified(const QString& filename)
{
  return QFileInfo(filename).lastModified();
}

QString copyrightString(const PluginHandle& ph)
{
  const auto& md = ph.metadata();

  if (auto iter = md.find("copyright"); iter != md.end())
    return iter.value().toString();

  if (auto iter = md.find("author"); iter != md.end())
    return QString("(c) %1 %2")
        .arg(lastModified(ph.fileName()).date().year())
        .arg(iter.value().toString());

  return {};
}

} // namespace

PluginInfoDialog::PluginInfoDialog(const PluginHandle& ph, QWidget* parent)
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
  ui->copyright_label->setFont(fnt);

  ui->icon_label->setPixmap(ph.icon().pixmap(96));
  ui->name_value->setText(ph.name());
  ui->version_value->setText(
      tr("version: %1")
          .arg(ph.metadata().value("version", tr("unknown")).toString()));
  ui->description_value->setText(ph.description());
  ui->copyright_label->setText(copyrightString(ph));
}

PluginInfoDialog::~PluginInfoDialog()
{
  delete ui;
}
