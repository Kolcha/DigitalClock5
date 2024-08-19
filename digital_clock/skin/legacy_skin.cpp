/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "legacy_skin.hpp"

#include <QDir>
#include <QSettings>

#include "skin/image_glyph.hpp"

namespace {

constexpr const auto required_keys = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "s1"};
constexpr const auto optional_keys = {"s2", "am", "pm"};

constexpr const auto skin_cfg_file = "skin.ini";
constexpr const auto geometry_file = "geometry.ini";

std::shared_ptr<ImageGlyphBase> probeImage(const QString& filename)
{
  // trust file extension, but still verify "reader's" state
  auto ext = QFileInfo(filename).suffix().toLower();
  if (ext == "svg") {
    if (auto r = std::make_unique<QSvgRenderer>(filename); r->isValid())
      return std::make_shared<VectorGlyph>(std::move(r));
  } else {
    if (auto p = QPixmap(filename); !p.isNull())
      return std::make_shared<RasterGlyph>(std::move(p));
  }
  return nullptr;
}

using SkinGlyphs = QHash<QLatin1StringView, std::shared_ptr<ImageGlyphBase>>;

void loadSkinFiles(const LegacySkinLoader::FilesMap& files, SkinGlyphs& glyphs)
{
  for (auto i = files.begin(); i != files.end(); ++i)
    if (auto g = probeImage(i.value()))
      glyphs[i.key()] = std::move(g);
}

void readSkinFiles(const QDir& d, LegacySkinLoader::FilesMap& files)
{
  auto proc_value = [&](QLatin1StringView key, const QVariant& val) {
    auto file = val.toString();

    if (file.isEmpty()) {
      files.remove(key);
      return;
    }

    if (d.exists(file)) {
      files[key] = d.absoluteFilePath(file);
    }
  };

  QSettings s(d.absoluteFilePath(skin_cfg_file), QSettings::IniFormat);
  s.beginGroup("files");
  for (const auto& k : required_keys)
    proc_value(QLatin1StringView(k), s.value(k));
  for (const auto& k : optional_keys)
    proc_value(QLatin1StringView(k), s.value(k));
  s.endGroup();
}

void detectSkinFiles(const QDir& d, LegacySkinLoader::FilesMap& files)
{
  auto detect_file = [&](QLatin1StringView key) {
    for (const auto& ext : {"svg", "png"}) {
      auto file = QString("%1.%2").arg(key, ext);
      if (d.exists(file)) {
        files[key] = d.absoluteFilePath(file);
        break;
      }
    }
  };

  for (const auto& k : required_keys)
    detect_file(QLatin1StringView(k));
  for (const auto& k : optional_keys)
    detect_file(QLatin1StringView(k));
}

bool validateSkinFiles(const LegacySkinLoader::FilesMap& files)
{
  bool valid = !files.empty();
  for (const auto& k : required_keys)
    valid = valid && files.contains(QLatin1StringView(k));
  return valid;
}

// finds skin image files and does validation, returns validation result
bool findSkinFiles(const QString& path, LegacySkinLoader::FilesMap& files)
{
  QDir d(path);

  if (d.exists(skin_cfg_file))
    readSkinFiles(d, files);
  else
    detectSkinFiles(d, files);

  return validateSkinFiles(files);
}

// returns skin title, should never return empty string
// directory name is used as fallback if no title defined
QString loadMetadata(const QString& path, LegacySkinLoader::Metadata& mdata)
{
  QDir d(path);
  QSettings s(d.absoluteFilePath(skin_cfg_file), QSettings::IniFormat);
  s.beginGroup("info");
  const auto keys = s.childKeys();
  for (const auto& k : keys) {
    auto v = s.value(k).toString();
    if (v.isEmpty())
      mdata.remove(k);
    else
      mdata[k] = std::move(v);
  }
  s.endGroup();
  return mdata.value("name", d.dirName());
}

struct GlyphGeometryRaw {
  std::optional<qreal> x;
  std::optional<qreal> y;
  std::optional<qreal> w;
  std::optional<qreal> h;
  std::optional<qreal> ax;
  std::optional<qreal> ay;
};

GlyphGeometryRaw parseGlyphGeometry(const QSettings& s)
{
  auto load_value = [&](const char* key) -> std::optional<qreal> {
    auto v = s.value(key);
    if (!v.toString().isEmpty() && v.canConvert<qreal>())
      return v.toReal();
    return std::nullopt;
  };

  GlyphGeometryRaw gargs;
  gargs.x = load_value("x");
  gargs.y = load_value("y");
  gargs.w = load_value("w");
  gargs.h = load_value("h");
  gargs.ax = load_value("ax");
  gargs.ay = load_value("ay");
  return gargs;
}

void updateGlyphGeometry(ImageGlyphBase& glyph, const GlyphGeometryRaw& gargs)
{
  const auto& r = glyph.rect();
  qreal x = gargs.x.value_or(r.x());
  qreal y = gargs.y.value_or(r.y());
  qreal w = gargs.w.value_or(r.width());
  qreal h = gargs.h.value_or(r.height());
  qreal ax = gargs.ax.value_or(w);
  qreal ay = gargs.ay.value_or(h);
  glyph.setRect({x, y, w, h});
  glyph.setAdvance({ax, -ay});
}

void loadGeometry(const QString& path, SkinGlyphs& glyphs)
{
  QDir d(path);

  if (!d.exists(geometry_file))
    return;

  QSettings s(d.absoluteFilePath(geometry_file), QSettings::IniFormat);
  for (auto i = glyphs.begin(); i != glyphs.end(); ++i) {
    s.beginGroup(i.key());
    updateGlyphGeometry(*i.value(), parseGlyphGeometry(s));
    s.endGroup();
  }
}

} // namespace

LegacySkinLoader::LegacySkinLoader(const QString& path)
{
  addOverlay(path);
}

void LegacySkinLoader::addOverlay(const QString& path)
{
  _overlays.append(path);
  _title = loadMetadata(path, _metadata);
  _is_valid = findSkinFiles(path, _filesmap);
}

std::unique_ptr<Skin> LegacySkinLoader::skin() const
{
  if (!_is_valid)
    return nullptr;

  SkinGlyphs glyphs;
  loadSkinFiles(_filesmap, glyphs);
  // set device/pixel ration for raster images
  for (auto i = glyphs.begin(); i != glyphs.end(); ++i) {
    if (auto g = std::dynamic_pointer_cast<RasterGlyph>(i.value()))
      g->setDevicePixelRatio(2.0);    // hardcoded for now
  }

  for (const auto& p : std::as_const(_overlays))
    loadGeometry(p, glyphs);

  auto skin = std::make_unique<LegacySkin>();

  auto add_glyph = [&](char c, QLatin1StringView key) {
    if (auto g = glyphs.value(key, nullptr))
      skin->addGlyph(c, g);
  };

  for (char c = '0'; c <= '9'; c++) {
    const std::array<char, 2> str_buf = {c, '\0'};
    add_glyph(c, QLatin1StringView(str_buf.data()));
  }

  add_glyph(':', QLatin1StringView("s1"));
  add_glyph(' ', QLatin1StringView("s2"));
  add_glyph('A', QLatin1StringView("am"));
  add_glyph('a', QLatin1StringView("am"));
  add_glyph('P', QLatin1StringView("pm"));
  add_glyph('p', QLatin1StringView("pm"));

  return skin;
}
