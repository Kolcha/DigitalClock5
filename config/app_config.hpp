#pragma once

#include "config_sections.hpp"

class AppConfig;

// represents per-instance settings
class WindowConfig {
public:
  WindowConfig(int id, AppConfig& cfg);

  const SectionAppearance& appearance() const;
  SectionAppearance& appearance();

  auto& state() { return _state; }
  auto& state() const { return _state; }

private:
  QString title(QStringView t) const;

private:
  int _id;
  AppConfig& _cfg;

  SectionAppearance _appearance;
  StateClient _state;
};


// config object
class AppConfig {
public:
  AppConfig();
  AppConfig(const QString& filename);

  auto& global() { return _global; }
  auto& global() const { return _global; }

  auto& window(size_t i) { return _windows[i]; }
  auto& window(size_t i) const { return _windows[i]; }

  auto& storage() { return _st; }

private:
  void initSlices();

private:
  SettingsStorage _st;
  SectionAppGlobal _global;
  std::vector<WindowConfig> _windows;
};
