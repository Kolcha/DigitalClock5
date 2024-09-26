/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "appearance_base.hpp"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>

static QString default_texture_path()
{
#ifdef Q_OS_MACOS
  return QCoreApplication::applicationDirPath() + "/../Resources/textures";
#else
  return QCoreApplication::applicationDirPath() + "/textures";
#endif
}

static QString last_texture_path(const QString& last_file)
{
  if (!last_file.isEmpty() && QFile::exists(last_file)) {
    return QFileInfo(last_file).filePath();
  }
  return default_texture_path();
}

QBrush AppearanceSectionBase::getTexture() const
{
  switch (getTextureType()) {
    case None:
      return Qt::NoBrush;
    case SolidColor:
      return getTextureColor();
    case Gradient:
      return getTextureGradient();
    case Pattern:
      return getTexturePattern();
  }
  Q_UNREACHABLE();
  return Qt::NoBrush;
}

QPixmap AppearanceSectionBase::getTexturePattern() const
{
  if (QFile::exists(getTexturePatternFile()))
    return QPixmap(getTexturePatternFile());
  return sample_pattern();
}

bool AppearanceSectionBase::shouldUseSystemForeground() const
{
  return getTextureType() == SolidColor && getUseSystemForeground();
}

QString AppearanceSectionBase::lastTexturePatternPath() const
{
  return last_texture_path(getTexturePatternFile());
}

QBrush AppearanceSectionBase::getBackground() const
{
  switch (getBackgroundType()) {
    case None:
      return Qt::NoBrush;
    case SolidColor:
      return getBackgroundColor();
    case Gradient:
      return getBackgroundGradient();
    case Pattern:
      return getBackgroundPattern();
  }
  Q_UNREACHABLE();
  return Qt::NoBrush;
}

QPixmap AppearanceSectionBase::getBackgroundPattern() const
{
  if (QFile::exists(getBackgroundPatternFile()))
    return QPixmap(getBackgroundPatternFile());
  return sample_pattern();
}

bool AppearanceSectionBase::shouldUseSystemBackground() const
{
  return getBackgroundType() == SolidColor && getUseSystemBackground();
}

QString AppearanceSectionBase::lastBackgroundPatternPath() const
{
  return last_texture_path(getBackgroundPatternFile());
}
