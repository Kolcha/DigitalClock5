// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>

namespace plugin::ip {

struct ExtIPDetector {
  QString name;
  QString ipv4;
  QString ipv6;
};

int detectors_count() noexcept;
ExtIPDetector ext_ip_detector(int idx);

} // namespace plugin::ip
