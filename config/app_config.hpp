#pragma once

#include "config_sections.hpp"

// represents per-instance settings
class ConfigSlice {
public:
  ConfigSlice(int id, SettingsStorage& st);

  auto& appearance() { return _appearance; }
  auto& appearance() const { return _appearance; }

  // TODO: add state

private:
  QString title(QStringView t) const;

private:
  int _id;

  SectionAppearance _appearance;
};


// config object
class AppConfig {
public:
  AppConfig();
  AppConfig(const QString& filename);

  auto& global() { return _global; }
  auto& global() const { return _global; }

  auto& slice(qsizetype i) { return _slices[i]; }
  auto& slice(qsizetype i) const { return _slices[i]; }

  auto& storage() { return _st; }

private:
  void initSlices();

private:
  SettingsStorage _st;
  SectionAppGlobal _global;
  std::vector<ConfigSlice> _slices;
};
