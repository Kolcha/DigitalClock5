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

struct SkinManager::Impl {
  QHash<QString, LegacySkinLoader> loaders;

  void reload();
};

void SkinManager::Impl::reload()
{
  auto data_paths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
  data_paths.append(":/");  // always look in app built-in resources

  std::reverse(data_paths.begin(), data_paths.end());

  for (const auto& p : std::as_const(data_paths)) {
    QDir d(p);

    if (!d.exists() || !d.cd("skins"))
      continue;

    const auto dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const auto& sd : std::as_const(dirs)) {
      if (auto iter = loaders.find(sd); iter != loaders.end())
        iter->addOverlay(d.absoluteFilePath(sd));
      else
        loaders.emplace(sd, d.absoluteFilePath(sd));
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
  if (auto iter = _impl->loaders.find(id); iter != _impl->loaders.end())
    return iter->skin();
  return nullptr;
}

QStringList SkinManager::availableSkins() const
{
  return _impl->loaders.keys();
}

SkinManager::Metadata SkinManager::metadata(const QString& id) const
{
  if (auto iter = _impl->loaders.find(id); iter != _impl->loaders.end())
    return iter->metadata();
  return {};
}