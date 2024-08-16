#pragma once

#include "settings_storage.hpp"

// even it may look as unnecessary and unused header,
// it is requeired due to some templates behind macro
#include "custom_converters.hpp"

class SectionAppGlobal : public SettingsStorageClient {
public:
  using SettingsStorageClient::SettingsStorageClient;

  CONFIG_OPTION_Q(int, NumInstances, 1)
  CONFIG_OPTION_Q(bool, AppearancePerInstance, false)
};

// per-window settings

class SectionAppearance : public SettingsStorageClient {
public:
  using SettingsStorageClient::SettingsStorageClient;
};
