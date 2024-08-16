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


class SectionLimits : public SettingsStorageClient {
public:
  using SettingsStorageClient::SettingsStorageClient;

  CONFIG_OPTION_Q(int, MinScaling, 10)
  CONFIG_OPTION_Q(int, MaxScaling, 250)

  CONFIG_OPTION_Q(int, BaseSize, 120)

  CONFIG_OPTION_Q(int, MinSpacing, -8)
  CONFIG_OPTION_Q(int, MaxSpacing, 16)
};

// per-window settings

class SectionAppearance : public SettingsStorageClient {
public:
  using SettingsStorageClient::SettingsStorageClient;

  CONFIG_OPTION_Q(int, ScalingH, 100)
  CONFIG_OPTION_Q(int, ScalingV, 100)

  CONFIG_OPTION_Q(int, SpacingH, 4)
  CONFIG_OPTION_Q(int, SpacingV, 4)
};
