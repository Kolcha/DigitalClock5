#pragma once

#include "config_sections.hpp"

class AppConfig;

// represents per-instance settings
class WindowConfig {
public:
  WindowConfig(int id, AppConfig& cfg);

  const SectionAppearance& appearance() const;
  SectionAppearance& appearance();

  auto& generic() { return _generic; }
  auto& generic() const { return _generic; }

  auto& state() { return _state; }
  auto& state() const { return _state; }

private:
  QString title(QStringView t) const;

private:
  int _id;
  AppConfig& _cfg;

  SectionAppearance _appearance;
  SectionGeneric _generic;
  StateClient _state;
};


// config object
class AppConfig {
public:
  AppConfig();
  AppConfig(const QString& filename);

  auto& global() { return _global; }
  auto& global() const { return _global; }

  auto& limits() { return _limits; }
  auto& limits() const { return _limits; }

  auto& window(size_t i) { return _windows[i]; }
  auto& window(size_t i) const { return _windows[i]; }

  auto& storage() { return *_st; }

  auto& state() { return _state; }
  auto& state() const { return _state; }

private:
  AppConfig(std::unique_ptr<SettingsStorage> st);

private:
  // SettingsStorage is non-movable because of QSettings :(
  std::unique_ptr<SettingsStorage> _st;
  SectionAppGlobal _global;
  SectionLimits _limits;
  std::vector<WindowConfig> _windows;
  StateClient _state;
};
