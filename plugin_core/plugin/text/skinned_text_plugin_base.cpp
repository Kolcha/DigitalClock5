// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "skinned_text_plugin_base.hpp"

#include <QColor>
#include <QDateTime>
#include <QEvent>
#include <QGridLayout>

#include "common/skin_manager.hpp"
#include "renderer/font_skin.hpp"
#include "renderer/texture.hpp"
#include "layout_config_parser.hpp"

#include "impl/settings_dialog.hpp"

namespace plugin::text {

TextPluginInstanceBase::TextPluginInstanceBase(const PluginInstanceConfig& cfg)
  : _cfg(cfg)
  , _opt_router(cfg)
{
}

void TextPluginInstanceBase::init(const InstanceOptionsHash& settings)
{
  Q_ASSERT(!settings.isEmpty());
  for (auto i = settings.begin(); i != settings.end(); ++i)
    _opt_router.onClockOptionChanged(i.key(), i.value());
}

void TextPluginInstanceBase::init(QWidget* w)
{
  Q_ASSERT(w);
  _clock_wnd = w;
}

void TextPluginInstanceBase::init(const SkinManager* sm)
{
  Q_ASSERT(sm);
  _sm = sm;
}

void TextPluginInstanceBase::startup()
{
  _widget = createWidget(_clock_wnd);
  _widget->installEventFilter(this);

  _widget_text = text();

  reloadConfig();
}

void TextPluginInstanceBase::shutdown()
{
  auto l = qobject_cast<QGridLayout*>(_clock_wnd->layout());
  Q_ASSERT(l);
  l->removeWidget(_widget);
  _widget->deleteLater();
  _widget = nullptr;
}

void TextPluginInstanceBase::update(const QDateTime& dt)
{
  Q_UNUSED(dt);
  repaintWidget();
}

void TextPluginInstanceBase::onOptionChanged(opt::InstanceOptions opt, const QVariant& val)
{
  Q_ASSERT(_widget);
  _opt_router.onClockOptionChanged(opt, val);

  if (_cfg.getAutoscaleEnabled())
    applyPluginOption(wpo::PercentOfClockSize, _cfg.getPercentOfClockSize());

  if (_opt_router.should_route(opt))
    applySharedOption(opt, val);
}

void TextPluginInstanceBase::applyPluginOption(wpo::InstanceOptions opt, const QVariant& val)
{
  if (!_widget) return;

  switch (opt) {
    case wpo::WidgetPosition: {
      auto l = qobject_cast<QGridLayout*>(_clock_wnd->layout());
      Q_ASSERT(l);
      l->removeWidget(_widget);
      switch (val.value<WidgetPosition>()) {
        case Bottom:
          _row = l->rowCount();
          _col = 0;
          _row_span = 1;
          _col_span = l->columnCount();
          break;
        case Right:
          _row = 0;
          _col = l->columnCount();
          _row_span = l->rowCount();
          _col_span = 1;
          break;
      }
      applyPluginOption(wpo::AutoscaleEnabled, _cfg.getAutoscaleEnabled());
      l->addWidget(_widget, _row, _col, _row_span, _col_span, _cfg.getWidgetAlignment());
      break;
    }

    case wpo::AutoscaleEnabled:
      if (val.toBool())
        applyPluginOption(wpo::PercentOfClockSize, _cfg.getPercentOfClockSize());
      else
        applySharedOption(opt::Scaling, _opt_router.value<int>(opt::Scaling));
      break;
    case wpo::PercentOfClockSize: {
      qreal k = val.toInt() / 100.;
      QSize avail_size = availSize();
      switch (_cfg.getWidgetPosition()) {
        case Bottom:
          k *= static_cast<qreal>(avail_size.width()) / _widget->sizeHint().width();
          break;
        case Right:
          k *= static_cast<qreal>(avail_size.height()) / _widget->sizeHint().height();
          break;
      }
      _widget->setScaling(_widget->scaling() * k);
      break;
    }
    case wpo::WidgetAlignment: {
      auto l = qobject_cast<QGridLayout*>(_clock_wnd->layout());
      Q_ASSERT(l);
      l->setAlignment(_widget, val.value<Qt::Alignment>());
      break;
    }

    case wpo::FollowClockAppearance:
      loadAppearanceSettings();
      break;
    case wpo::UseClockSkin:
      loadSkinSettings();
      break;
  }
}

void TextPluginInstanceBase::applySharedOption(opt::InstanceOptions opt, const QVariant& val)
{
  if (!_widget) return;

  switch (opt) {
    case opt::UseSkin:
      val.toBool() ?
         applySharedOption(opt::Skin, _opt_router.value<QString>(opt::Skin)) :
         applySharedOption(opt::Font, _opt_router.value<QFont>(opt::Font));
      break;
    case opt::Skin:
      _widget->setSkin(_sm->load(val.toString()));
      break;
    case opt::Font:
      _widget->setSkin(std::make_unique<FontSkin>(val.value<QFont>()));
      break;

    case opt::Scaling:
      if (!_cfg.getAutoscaleEnabled())
        _widget->setScaling(val.toInt() / 100.);
      break;

    case opt::TextureType:
      switch (val.value<tx::TextureType>()) {
        case tx::None:
          _widget->setTextureCustomization(LinesRenderer::None);
          break;
        case tx::Color:
          applySharedOption(opt::TextureColorUseTheme, _opt_router.value<bool>(opt::TextureColorUseTheme));
          break;
        case tx::Gradient:
          applySharedOption(opt::TextureGradient, QVariant::fromValue(_opt_router.value<QGradient>(opt::TextureGradient)));
          break;
        case tx::Pattern:
          applySharedOption(opt::TexturePattern, _opt_router.value<QString>(opt::TexturePattern));
          break;
      }
      if (val.value<tx::TextureType>() != tx::None)
        _widget->setTextureCustomization(_opt_router.value<LinesRenderer::CustomizationType>(opt::TextureCustomization));
      break;
    case opt::TextureCustomization:
      _widget->setTextureCustomization(val.value<LinesRenderer::CustomizationType>());
      break;
    case opt::TextureColor:
      _widget->setTexture(solid_color_texture(val.value<QColor>()));
      break;
    case opt::TextureColorUseTheme:
      applySharedOption(opt::TextureColor, val.toBool() ?
                        _clock_wnd->palette().color(QPalette::WindowText) :
                        _opt_router.value<QColor>(opt::TextureColor));
      break;
    case opt::TextureGradient:
      _widget->setTexture(gradient_texture(val.value<QGradient>()));
      break;
    case opt::TexturePattern:
      _widget->setTexture(pattern_from_file(val.toString()),
                          _opt_router.value<bool>(opt::TexturePatternTile) ? Qt::RepeatTile : Qt::StretchTile);
      break;
    case opt::TexturePatternTile:
      _widget->setTexture(pattern_from_file(_opt_router.value<QString>(opt::TexturePattern)),
                          val.toBool() ? Qt::RepeatTile : Qt::StretchTile);
      break;

    case opt::BackgroundType:
      switch (val.value<tx::TextureType>()) {
        case tx::None:
          _widget->setBackgroundCustomization(LinesRenderer::None);
          break;
        case tx::Color:
          applySharedOption(opt::BackgroundColorUseTheme, _opt_router.value<bool>(opt::BackgroundColorUseTheme));
          break;
        case tx::Gradient:
          applySharedOption(opt::BackgroundGradient, QVariant::fromValue(_opt_router.value<QGradient>(opt::BackgroundGradient)));
          break;
        case tx::Pattern:
          applySharedOption(opt::BackgroundPattern, _opt_router.value<QString>(opt::BackgroundPattern));
          break;
      }
      if (val.value<tx::TextureType>() != tx::None)
        _widget->setBackgroundCustomization(_opt_router.value<LinesRenderer::CustomizationType>(opt::BackgroundCustomization));
      break;
    case opt::BackgroundCustomization:
      _widget->setBackgroundCustomization(val.value<LinesRenderer::CustomizationType>());
      break;
    case opt::BackgroundColor:
      _widget->setBackground(solid_color_texture(val.value<QColor>()));
      break;
    case opt::BackgroundColorUseTheme:
      applySharedOption(opt::BackgroundColor, val.toBool() ?
                        _clock_wnd->palette().color(QPalette::Window) :
                        _opt_router.value<QColor>(opt::BackgroundColor));
      break;
    case opt::BackgroundGradient:
      _widget->setBackground(gradient_texture(val.value<QGradient>()));
      break;
    case opt::BackgroundPattern:
      _widget->setBackground(pattern_from_file(val.toString()),
                             _opt_router.value<bool>(opt::BackgroundPatternTile) ? Qt::RepeatTile : Qt::StretchTile);
      break;
    case opt::BackgroundPatternTile:
      _widget->setBackground(pattern_from_file(_opt_router.value<QString>(opt::BackgroundPattern)),
                             val.toBool() ? Qt::RepeatTile : Qt::StretchTile);
      break;

    case opt::TruePerCharRendering:
      _widget->setTruePerCharRendering(val.toBool());
      break;

    case opt::CharSpacing:
      _widget->setCharSpacing(val.toInt());
      break;
    case opt::LineSpacing:
      _widget->setLineSpacing(val.toInt());
      break;
    case opt::RespectLineSpacing:
      _widget->setRespectLineSpacing(val.toBool());
      break;

    case opt::CharMargins:
      _widget->setCharMargins(val.value<QMarginsF>());
      break;
    case opt::WidgetMargins:
      _widget->setTextMargins(val.value<QMargins>());
      break;

    case opt::LayoutConfig:
      _text_opts = parse_layout_options(val.toString());
      _widget->drawText(_widget_text, _text_opts);
      applyPluginOption(wpo::AutoscaleEnabled, _cfg.getAutoscaleEnabled());
      break;

    default:  // just to make clang happy...
      break;
  }
}

void TextPluginInstanceBase::reloadConfig()
{
  loadSkinSettings();
  loadAppearanceSettings();
  // widget must be fully configured at this point
  loadPluginSettings();
}

SkinnedTextWidget* TextPluginInstanceBase::createWidget(QWidget* parent) const
{
  return new SkinnedTextWidget(_clock_wnd);
}

void TextPluginInstanceBase::repaintWidget()
{
  auto new_text = text();
  if (_widget_text != new_text) {
    _widget_text = new_text;
    _widget->drawText(_widget_text, _text_opts);
  }

  if (_cfg.getAutoscaleEnabled())
    applyPluginOption(wpo::PercentOfClockSize, _cfg.getPercentOfClockSize());
}

bool TextPluginInstanceBase::eventFilter(QObject* obj, QEvent* e)
{
  if (e->type() == QEvent::PaletteChange) {
    if (_opt_router.value<tx::TextureType>(opt::TextureType) == tx::Color &&
        _opt_router.value<bool>(opt::TextureColorUseTheme))
      applySharedOption(opt::TextureColorUseTheme, true);
    if (_opt_router.value<tx::TextureType>(opt::BackgroundType) == tx::Color &&
        _opt_router.value<bool>(opt::BackgroundColorUseTheme))
      applySharedOption(opt::BackgroundColorUseTheme, true);
  }
  return QObject::eventFilter(obj, e);
}

void TextPluginInstanceBase::loadPluginSettings()
{
  applyPluginOption(wpo::WidgetPosition, _cfg.getWidgetPosition());
}

void TextPluginInstanceBase::loadSkinSettings()
{
  applySharedOption(opt::UseSkin, _opt_router.value<bool>(opt::UseSkin));
}

void TextPluginInstanceBase::loadAppearanceSettings()
{
  applySharedOption(opt::TextureType, _opt_router.value<tx::TextureType>(opt::TextureType));
  applySharedOption(opt::BackgroundType, _opt_router.value<tx::TextureType>(opt::BackgroundType));

  applySharedOption(opt::TruePerCharRendering, _opt_router.value<bool>(opt::TruePerCharRendering));

  applySharedOption(opt::CharSpacing, _opt_router.value<int>(opt::CharSpacing));
  applySharedOption(opt::LineSpacing, _opt_router.value<int>(opt::LineSpacing));
  applySharedOption(opt::RespectLineSpacing, _opt_router.value<bool>(opt::RespectLineSpacing));

  applySharedOption(opt::CharMargins, QVariant::fromValue(_opt_router.value<QMarginsF>(opt::CharMargins)));
  applySharedOption(opt::WidgetMargins, QVariant::fromValue(_opt_router.value<QMargins>(opt::WidgetMargins)));

  applySharedOption(opt::LayoutConfig, _opt_router.value<QString>(opt::LayoutConfig));
}

QSize TextPluginInstanceBase::availSize() const
{
  auto l = qobject_cast<QGridLayout*>(_clock_wnd->layout());
  Q_ASSERT(l);
  auto sz = l->itemAtPosition(0, 0)->widget()->sizeHint();  // clock
  for (int i = 1; i < _col_span; i++)
    if (auto item = l->itemAtPosition(0, i))
      if (auto w = item->widget())
        sz.rwidth() += w->sizeHint().width();
  for (int i = 1; i < _row_span; i++)
    if (auto item = l->itemAtPosition(i, 0))
      if (auto w = item->widget())
        sz.rheight() += w->sizeHint().height();
  return sz;
}


void TextPluginBase::init(Context&& ctx)
{
  Q_ASSERT(!_cfg);
  auto st_back = std::make_unique<PluginSettingsBackend>(std::move(ctx.settings));
  _cfg = createConfig(std::move(st_back));
  _state = std::make_unique<PluginSettingsBackend>(std::move(ctx.state));
  _clock_insts = std::move(ctx.active_instances);
}

ClockPluginInstance* TextPluginBase::instance(size_t idx)
{
  Q_ASSERT(_cfg);
  auto& inst = _insts[idx];
  if (!inst)
    inst = createInstance(idx);
  return inst.get();
}

void TextPluginBase::configure(QWidget* parent, size_t idx)
{
  Q_ASSERT(!_clock_insts.isEmpty());

  using plugin::text::impl::SettingsDialog;
  auto d = new SettingsDialog(_cfg.get(), parent);
  d->setWindowTitle(name());
  d->insertPagesBeforeCommon(configPagesBeforeCommonPages());
  d->insertPagesAfterCommon(configPagesAfterCommonPages());

  connect(d, &SettingsDialog::accepted, _cfg.get(), &PluginConfig::commit);
  connect(d, &SettingsDialog::rejected, _cfg.get(), &PluginConfig::discard);

  connect(d, &SettingsDialog::instanceSwitched, this, [this, d, prev = idx](size_t curr) mutable {
    if (_insts.empty()) return;
    auto prev_inst = qobject_cast<TextPluginInstanceBase*>(instance(prev));
    if (auto w = prev_inst->widget()) w->disableFrame();
    disconnect(d, &SettingsDialog::pluginOptionChanged, prev_inst, &TextPluginInstanceBase::applyPluginOption);
    disconnect(d, &SettingsDialog::sharedOptionChanged, prev_inst, &TextPluginInstanceBase::applySharedOption);
    auto curr_inst = qobject_cast<TextPluginInstanceBase*>(instance(curr));
    if (auto w = curr_inst->widget()) w->enableFrame();
    connect(d, &SettingsDialog::pluginOptionChanged, curr_inst, &TextPluginInstanceBase::applyPluginOption);
    connect(d, &SettingsDialog::sharedOptionChanged, curr_inst, &TextPluginInstanceBase::applySharedOption);
    prev = curr;
  });
  connect(d, &SettingsDialog::instanceSwitched, this, &TextPluginBase::instanceSwitched);

  connect(d, &SettingsDialog::rejected, this, [this]() {
    for (const auto& [_, i] : _insts)
      i->reloadConfig();
  });

  connect(d, &SettingsDialog::finished, this, [this]() {
    for (const auto& [_, i] : _insts)
      if (auto w = i->widget())
        w->disableFrame();
  });
  connect(d, &SettingsDialog::finished, d, &QObject::deleteLater);

  for (auto i : _clock_insts)
    d->addInstance(i);
  d->setInstance(idx);

  d->show();
}

std::unique_ptr<PluginConfig> TextPluginBase::createConfig(std::unique_ptr<PluginSettingsBackend> b) const
{
  return std::make_unique<PluginConfig>(std::move(b));
}

PluginInstanceConfig* TextPluginBase::instanceConfig(size_t idx) const
{
  return _cfg->instance(idx);
}

std::unique_ptr<SettingsStorage> TextPluginBase::instanceState(size_t idx) const
{
  auto st = std::make_unique<PluginSettingsStorage>(std::ref(*_state));
  return std::make_unique<PrefixedSettingsStorage>(std::move(st), instance_prefix(idx));
}

} // namespace plugin::text
