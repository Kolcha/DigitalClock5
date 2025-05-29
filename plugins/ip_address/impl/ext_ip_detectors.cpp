// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ext_ip_detectors.hpp"

namespace plugin::ip {

namespace impl {

struct detector_def {
  const char* name;
  const char* ipv4;
  const char* ipv6;
};

static constexpr const detector_def detectors[] = {
  { .name = "ipify.org", .ipv4 = "https://api.ipify.org/", .ipv6 = "https://api6.ipify.org/" },
  { .name = "icanhazip.com", .ipv4 = "https://ipv4.icanhazip.com/", .ipv6 = "https://ipv6.icanhazip.com/" },
};

static constexpr const int detectors_count = sizeof(detectors) / sizeof(detector_def);

} // namespace impl

int detectors_count() noexcept
{
  return impl::detectors_count;
}

ExtIPDetector ext_ip_detector(int idx)
{
  idx = std::clamp(idx, 0, impl::detectors_count);
  const auto& dimpl = impl::detectors[idx];
  return {
    .name = QString::fromLatin1(dimpl.name),
    .ipv4 = QString::fromLatin1(dimpl.ipv4),
    .ipv6 = QString::fromLatin1(dimpl.ipv6),
  };
}

} // namespace plugin::ip
