/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "skin_manager_impl.hpp"

#include <algorithm>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

#include "skin/error_skin.hpp"
#include "skin/legacy_skin.hpp"

struct SkinManagerImpl::Impl {
  QHash<QString, LegacySkinLoader> loaders;
  QSet<QString> user_skins;

  void reload();

  const LegacySkinLoader* find(const QString& id) const;
};

void SkinManagerImpl::Impl::reload()
{
  auto data_paths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
  data_paths.append(QCoreApplication::applicationDirPath());
  data_paths.append(":/");  // always look in app built-in resources

  std::ranges::reverse(data_paths);

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

const LegacySkinLoader* SkinManagerImpl::Impl::find(const QString& id) const
{
  if (auto iter = loaders.find(id); iter != loaders.end())
    return &iter.value();
  // search skin by name as fallback
  for (auto iter = loaders.begin(); iter != loaders.end(); ++iter)
    if (iter->title() == id)
      return &iter.value();
  return nullptr;
}

SkinManagerImpl::SkinManagerImpl()
  : _impl(std::make_unique<Impl>())
{
  _impl->reload();
}

SkinManagerImpl::~SkinManagerImpl() = default;

QStringList SkinManagerImpl::list() const
{
  return _impl->loaders.keys();
}

SkinInfoList SkinManagerImpl::infoList() const
{
  const auto lst = list();

  SkinInfoList info_list(lst.size());
  // *INDENT-OFF*
  std::ranges::transform(lst, info_list.begin(),
                         [this](const auto& i) { return info(i); });
  // *INDENT-ON*
  return info_list;
}

SkinInfo SkinManagerImpl::info(const QString& id) const
{
  if (auto iter = _impl->find(id))
    return {
    .id = id,
    .name = iter->title(),
    .metadata = iter->metadata(),
    .user_provided = _impl->user_skins.contains(id),
  };
  return {};
}

std::unique_ptr<Skin> SkinManagerImpl::load(const QString& id) const
{
  if (auto iter = _impl->find(id))
    return iter->skin();
  return std::make_unique<ErrorSkin>();
}

void SkinManagerImpl::setSkinBaseSize(int sz)
{
  for (auto& l : _impl->loaders)
    l.setGlyphBaseSize(sz);
}
