/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "skin/skin.hpp"

// TODO: convert it to "pure factory"
class LegacySkin : public Skin {
public:
  bool hasAlternateSeparator() const override
  {
    return _glyphs.contains(' ');
  }

  bool supportsCustomSeparator() const override { return false; }

protected:
  std::shared_ptr<Glyph> create(char32_t c) const override
  {
    return _glyphs.value(c, nullptr);
  }

public:
  void addGlyph(char32_t c, std::shared_ptr<Glyph> g)
  {
    _glyphs[c] = std::move(g);
  }

private:
  QHash<char32_t, std::shared_ptr<Glyph>> _glyphs;
};


// very lightweight object, stores only metadata
// files are loaded only on skin(), no files cached
class LegacySkinLoader
{
public:
  using Metadata = QHash<QString, QString>;
  using FilesMap = QHash<QLatin1StringView, QString>;

  explicit LegacySkinLoader(const QString& path);

  auto& overlays() const { return _overlays; }
  QString title() const { return _title; }
  auto& metadata() const { return _metadata; }
  bool valid() const { return _is_valid; }

  int glyphBaseSize() const { return _glyph_base_size; }

  void setGlyphBaseSize(int s) { _glyph_base_size = s; }

  // TODO: consider tracking file changes
  void addOverlay(const QString& path);

  std::unique_ptr<Skin> skin() const;

private:
  QStringList _overlays;
  QString _title;
  Metadata _metadata;
  FilesMap _filesmap;
  bool _is_valid = false;
  int _glyph_base_size = 96;
};
