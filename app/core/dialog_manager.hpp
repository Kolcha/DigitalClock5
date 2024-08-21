/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <concepts>

#include <QtCore/QHash>
#include <QtWidgets/QDialog>

/**
 * Helper class to avoid dialogs re-creations
 *
 * Acts as factory + cache for any dialogs.
 */
template<typename Tag>
class DialogManager
{
public:
  template<typename Dialog, typename... Args>
  requires std::derived_from<Dialog, QDialog>
  Dialog* maybeCreateAndShowDialog(const Tag& tag, Args&&... args)
  {
    auto& dlg = _cache[tag];
    if (!dlg) {
      dlg = new Dialog(std::forward<Args>(args)...);
      QObject::connect(dlg, &QDialog::finished, dlg, &QObject::deleteLater);
      QObject::connect(dlg, &QObject::destroyed, [this, tag]() { _cache.remove(tag); });
    }
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
    return qobject_cast<Dialog*>(dlg);
  }

private:
  QHash<Tag, QDialog*> _cache;
};
