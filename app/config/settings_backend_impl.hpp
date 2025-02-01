// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "common/settings.hpp"

#include <QtCore/QSettings>

// reset config:
// - drop runtime values
// - set flag to block reading from storage
// - just return default values
// - drop flag on commit() or discard()

class SettingsBackendImpl : public SettingsBackend
{
public:
  SettingsBackendImpl();
  explicit SettingsBackendImpl(const QString& filename);

  void setValue(const QString& k, const QVariant& v) override;
  QVariant value(const QString& k, const QVariant& d) const override;

  void commitValue(const QString& k) override;
  void discardValue(const QString& k) override;

  void exportSettings(QVariantHash& s) const;
  void importSettings(const QVariantHash& s);

  void commitAll();
  void discardAll();

private:
  QSettings _st;
  mutable QVariantHash _runtime_values;
};
