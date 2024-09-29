/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "skin_manager.hpp"

#include <QDir>
#include <QStandardPaths>

#include "skin/font_skin.hpp"
#include "skin/legacy_skin.hpp"

#include "skin/error_skin.hpp"

struct SkinManager::Impl {
  QHash<QString, LegacySkinLoader> loaders;
  QSet<QString> user_skins;

  void reload();

  const LegacySkinLoader* find(const QString& id) const;
};

void SkinManager::Impl::reload()
{
  auto data_paths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
  data_paths.append(":/");  // always look in app built-in resources

  std::reverse(data_paths.begin(), data_paths.end());

  for (const auto& p : std::as_const(data_paths)) {
    bool is_user = p == QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir d(p);

    if (!d.exists() || !d.cd("skins"))
      continue;

    const auto dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const auto& sd : std::as_const(dirs)) {
      if (auto iter = loaders.find(sd); iter != loaders.end())
        iter->addOverlay(d.absoluteFilePath(sd));
      else
        loaders.emplace(sd, d.absoluteFilePath(sd));

      if (is_user)
        user_skins.insert(sd);
    }
  }

  // at loading time we can't be sure that skin will remain valid,
  // any of overlays may brake it (but in general should not)
  // so, remove some invalid skins if any at the end of enumerating
  const auto ids = loaders.keys();
  for (const auto& id : ids)
    if (!loaders.value(id, LegacySkinLoader("")).valid())
      loaders.remove(id);
}

const LegacySkinLoader* SkinManager::Impl::find(const QString& id) const
{
  if (auto iter = loaders.find(id); iter != loaders.end())
    return &iter.value();
  // search skin by name as fallback
  for (auto iter = loaders.begin(); iter != loaders.end(); ++iter)
    if (iter->title() == id)
      return &iter.value();
  return nullptr;
}

SkinManager::SkinManager()
    : _impl(std::make_unique<Impl>())
{
  _impl->reload();
}

SkinManager::~SkinManager() = default;

std::unique_ptr<Skin> SkinManager::loadSkin(const QFont& font) const
{
  return std::make_unique<FontSkin>(font);
}

std::unique_ptr<Skin> SkinManager::loadSkin(const QString& id) const
{
  if (auto iter = _impl->find(id))
    return iter->skin();
  return std::make_unique<ErrorSkin>();
}

QStringList SkinManager::availableSkins() const
{
  return _impl->loaders.keys();
}

SkinManager::Metadata SkinManager::metadata(const QString& id) const
{
  if (auto iter = _impl->find(id))
    return iter->metadata();
  return {};
}

bool SkinManager::isUserSkin(const QString& id) const
{
  return _impl->user_skins.contains(id);
}

void SkinManager::setSkinBaseSize(int sz)
{
  for (auto& l : _impl->loaders)
    l.setGlyphBaseSize(sz);
}
