// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "skin_engine_global.hpp"

#include <memory>
#include <vector>

#include <QtCore/qnamespace.h>
#include <QtCore/qtmetamacros.h>
#include <QtCore/QMargins>
#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtGui/QPixmap>
#include <QtWidgets/QTileRules>

#include "renderable.hpp"
#include "skin.hpp"

class SKIN_ENGINE_EXPORT LinesRenderer
{
  Q_GADGET

public:
  virtual ~LinesRenderer() = default;

  using Lines = std::vector<std::unique_ptr<Renderable>>;

  class SKIN_ENGINE_EXPORT Options {
  public:
    Options() {}  // compiler bug, see https://stackoverflow.com/questions/53408962

    bool scalingEnabled(size_t idx) const { return _scalable_lines.contains(idx); }

    void setLineScalingEnabled(size_t idx, bool enable);
    inline void enableLineScaling(size_t idx) { setLineScalingEnabled(idx, true); }
    inline void disableLineScaling(size_t idx) { setLineScalingEnabled(idx, false); }

    Qt::Alignment lineAlignment(size_t idx) const;

    void setAlignment(Qt::Alignment a) noexcept { _def_alignment = a; }
    void setLineAlignment(size_t idx, Qt::Alignment a) { _line_alignment[idx] = a; }

  private:
    QSet<size_t> _scalable_lines;
    Qt::Alignment _def_alignment = Qt::AlignLeft;
    QHash<size_t, Qt::Alignment> _line_alignment;
  };

  std::unique_ptr<Renderable> draw(Lines&& lines, const Options& opt = {}) const;

  enum CustomizationType {
    None,
    AllText,
    PerLine,
    PerChar,
  };
  Q_ENUM(CustomizationType)

  void setTextureCustomization(CustomizationType c);
  void setBackgroundCustomization(CustomizationType c);

  void setTexture(QPixmap p, QTileRules t = {});
  void setBackground(QPixmap p, QTileRules t = {});

  void setCharSpacing(qreal s);
  void setLineSpacing(qreal s) noexcept { _line_spacing = s; }
  void setRespectLineSpacing(bool en) noexcept { _respect_line_spacing = en; }

  // not recommended for arbitrary text
  void setTruePerCharRendering(bool en);

  void setCharMargins(QMarginsF m);
  void setTextMargins(QMarginsF m);

  void setSkin(std::shared_ptr<Skin> skin);

  qreal charSpacing() const noexcept { return _char_spacing; }
  qreal lineSpacing() const noexcept { return _line_spacing; }

  QMarginsF charMargins() const noexcept { return _char_margins; }
  QMarginsF textMargins() const noexcept { return _text_margins; }

  std::shared_ptr<Skin> skin() const noexcept { return _skin; }

protected:
  // Skin::metrics() may be expensive to call on "hot path",
  // prefer using this method instead
  // cached value is updated on skin change
  const auto& cachedSkinMetrics() const noexcept { return _skin_metrics; }

  std::unique_ptr<Renderable> buildEmptyLinePlaceholder() const;

  // convenient skin change event handler
  // called after storing new skin (skin() will return the new skin)
  // default implementation does nothing
  virtual void onSkinChange() {}

private:
  void updateSkinTexture();
  void updateSkinBackground();

  void updateSkinSettings();

  std::unique_ptr<Renderable> customize(std::unique_ptr<Renderable> r,
                                        bool set_texture,
                                        bool set_background) const;

private:
  QPixmap _texture;
  QPixmap _background;
  QTileRules _tx_rules;
  QTileRules _bg_rules;
  CustomizationType _tx_ct = None;
  CustomizationType _bg_ct = None;

  qreal _char_spacing = 0;
  qreal _line_spacing = 0;
  bool _respect_line_spacing = false;

  bool _true_per_char = false;

  QMarginsF _char_margins;
  QMarginsF _text_margins;

  std::shared_ptr<Skin> _skin;
  Skin::Metrics _skin_metrics;
};


class SKIN_ENGINE_EXPORT TextRenderer : public LinesRenderer
{
public:
  std::unique_ptr<Renderable> draw(const QString& txt, const Options& opt = {}) const;
};
