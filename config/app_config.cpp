#include "app_config.hpp"

WindowConfig::WindowConfig(int id, AppConfig& cfg)
    : _id(id)
    , _cfg(cfg)
    , _appearance(title(u"appearance"), cfg.storage())
    , _state(title(u"state"), cfg.storage())
{
}

const SectionAppearance& WindowConfig::appearance() const
{
  if (!_cfg.global().getAppearancePerInstance())
    return _cfg.window(0).appearance();
  return _appearance;
}

SectionAppearance& WindowConfig::appearance()
{
  return const_cast<SectionAppearance&>(std::as_const(*this).appearance());
}

QString WindowConfig::title(QStringView t) const
{
  return QString("Window%1/%2").arg(_id).arg(t);
}


AppConfig::AppConfig()
    : AppConfig(std::make_unique<SettingsStorage>())
{
}

AppConfig::AppConfig(const QString& filename)
    : AppConfig(std::make_unique<SettingsStorage>(filename))
{
}

AppConfig::AppConfig(std::unique_ptr<SettingsStorage> st)
    : _st(std::move(st))
    , _global("app_global", *_st)
    , _limits("limits", *_st)
{
  for (int i = 0; i < _global.getNumInstances(); i++)
    _windows.push_back(WindowConfig(i, *this));
}
