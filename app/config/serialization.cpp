/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "serialization.hpp"

#include <QDataStream>
#include <QFile>
#include <QVariantHash>

namespace dc5 {

constexpr const quint32 file_type_magic = 0x44435335;   // DCS5, BE
constexpr const quint16 serialization_format = 0x515A;  // QZ, BE

void exportToFile(const QVariantHash& settings, const QString& filename)
{
  QByteArray buffer;
  QDataStream sout(&buffer, QIODevice::WriteOnly);
  sout.setVersion(QDataStream::Qt_6_5);
  sout << settings;

  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) return;

  QByteArray sdata = qCompress(buffer, 9);

  QDataStream fout(&file);
  fout.setVersion(QDataStream::Qt_6_5);
  fout << file_type_magic;
  fout << serialization_format;
  fout << qChecksum(sdata);
  fout << static_cast<quint32>(sdata.size());
  fout.writeRawData(sdata.data(), sdata.size());
}

void importFromFile(QVariantHash& settings, const QString& filename)
{
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) return;

  QDataStream fin(&file);
  fin.setVersion(QDataStream::Qt_6_5);

  quint32 magic;
  quint16 dsfmt;
  quint16 dcsum;
  quint32 dsize;

  fin >> magic >> dsfmt >> dcsum >> dsize;

  if (magic != file_type_magic) return;
  if (dsfmt != serialization_format) return;

  QByteArray sdata(dsize, Qt::Initialization{});
  if (fin.readRawData(sdata.data(), sdata.size()) != dsize) return;
  if (qChecksum(sdata) != dcsum) return;

  QByteArray buffer = qUncompress(sdata);
  QDataStream sin(&buffer, QIODevice::ReadOnly);
  sin.setVersion(QDataStream::Qt_6_5);

  sin >> settings;
}

} // namespace dc5
