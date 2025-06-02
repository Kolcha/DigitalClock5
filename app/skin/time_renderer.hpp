// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "renderer/renderer.hpp"

#include <QtCore/QDateTime>
#include <QtCore/QHash>
#include <QtGui/QTransform>

class TimeRenderer : public LinesRenderer
{
public:
  std::unique_ptr<Renderable> draw(const QDateTime& dt) const;

  void setFormat(QString fmt) noexcept { _format = std::move(fmt); }
  void setOptions(Options opt) noexcept { _options = std::move(opt); }

  void setFlashingSeparator(bool en) noexcept;
  inline void enableFlashingSeparator() { setFlashingSeparator(true); }
  inline void disableFlashingSeparator() { setFlashingSeparator(false); }

  void animateSeparator() noexcept;

  void setCustomSeparators(QString seps) noexcept
  {
    _custom_seps = std::move(seps);
  }

  void setTokenTransform(const QString& token, const QTransform& tr)
  {
    _token_transforms[token] = tr;
  }

protected:
  void onSkinChange() override;

private:
  QString _format;
  Options _options;

  bool _flashing_separator = true;
  bool _separator_visible = true;

  // skin defines glyphs for ':' and ' '
  bool _skin_has_2_seps = false;

  QString _custom_seps;

  QHash<QString, QTransform> _token_transforms;

  friend class LinesBuilder;
};
