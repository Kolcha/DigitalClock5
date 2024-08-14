#include "app_config.hpp"

ConfigSlice::ConfigSlice(int id, SettingsStorage& st)
    : _id(id)
    , _appearance(title(u"appearance"), st)
    , _state(title(u"state"), st)
{
}

QString ConfigSlice::title(QStringView t) const
{
  return QString("%1/%2").arg(_id).arg(t);
}


AppConfig::AppConfig()
    : _st()
    , _global("app_global", _st)
{
  initSlices();
}

AppConfig::AppConfig(const QString& filename)
    : _st(filename)
    , _global("app_global", _st)
{
  initSlices();
}

void AppConfig::initSlices()
{
  for (int i = 0; i < _global.getNumInstances(); i++)
    _slices.push_back(ConfigSlice(i, _st));
}
