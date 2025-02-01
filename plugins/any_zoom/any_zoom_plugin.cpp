/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "any_zoom_plugin.hpp"

#include "gui/zoom_dialog.hpp"

AnyZoomPlugin::AnyZoomPlugin(const AnyZoomPluginInstanceConfig& cfg)
  : SettingsPluginInstance()
  , _cfg(cfg)
{
}

void AnyZoomPlugin::init(const InstanceOptionsHash& settings)
{
  _clock_scaling = settings.value(opt::Scaling, _clock_scaling).toInt();
}

void AnyZoomPlugin::startup()
{
  reloadConfig();
}

void AnyZoomPlugin::shutdown()
{
  emit optionChanged(opt::Scaling, _clock_scaling);
}

void AnyZoomPlugin::onOptionChanged(opt::InstanceOptions opt, const QVariant& val)
{
  Q_UNUSED(opt);
  Q_UNUSED(val);
}

void AnyZoomPlugin::onScaleFactorChanged(const ScaleFactor& f)
{
  emit optionChanged(opt::Scaling, QVariant::fromValue(f));
}

void AnyZoomPlugin::reloadConfig()
{
  auto sx = _cfg.getZoomX() / 100.;
  auto sy = _cfg.getZoomY() / 100.;
  emit optionChanged(opt::Scaling, QVariant::fromValue(ScaleFactor(sx, sy)));
}


void AnyZoomPluginFactory::init(Context&& ctx)
{
  _st = std::make_unique<PluginSettingsBackend>(std::move(ctx.settings));
}

QString AnyZoomPluginFactory::description() const
{
  return tr("Allows to set any clock zoom, even different values for X and Y axis.");
}

ClockPluginInstance* AnyZoomPluginFactory::instance(size_t idx)
{
  auto& inst = _insts[idx];
  if (!inst)
    inst = createInstance(idx);
  return inst.get();
}

void AnyZoomPluginFactory::configure(QWidget* parent, size_t idx)
{
  using any_zoom::ZoomDialog;
  auto d = new ZoomDialog(instanceConfig(idx), parent);
  d->setWindowTitle(name());

  connect(d, &ZoomDialog::accepted, _st->storage(), &SettingsStorage::commit);
  connect(d, &ZoomDialog::rejected, _st->storage(), &SettingsStorage::discard);

  connect(d, &ZoomDialog::finished, d, &QObject::deleteLater);

  disconnect(d, &ZoomDialog::zoomChanged, nullptr, nullptr);

  if (auto iter = _insts.find(idx); iter != _insts.end()) {
    auto inst = iter->second.get();
    Q_ASSERT(inst);
    connect(d, &ZoomDialog::zoomChanged, inst, &AnyZoomPlugin::onScaleFactorChanged);
    connect(d, &ZoomDialog::rejected, inst, &AnyZoomPlugin::reloadConfig);
  }

  d->show();
}

std::unique_ptr<AnyZoomPlugin> AnyZoomPluginFactory::createInstance(size_t idx)
{
  return std::make_unique<AnyZoomPlugin>(*instanceConfig(idx));
}

AnyZoomPluginInstanceConfig* AnyZoomPluginFactory::instanceConfig(size_t idx)
{
  auto& icfg = _cfgs[idx];
  if (!icfg) {
    auto st = std::make_unique<PluginSettingsStorage>(std::ref(*_st));
    auto pst = std::make_unique<PrefixedSettingsStorage>(std::move(st), instance_prefix(idx));
    icfg = std::make_unique<AnyZoomPluginInstanceConfig>(std::move(pst));
  }
  return icfg.get();
}
