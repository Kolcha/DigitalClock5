/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "appearance_base.hpp"

#include <QFile>

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
