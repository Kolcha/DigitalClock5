/*
 * SPDX-FileCopyrightText: 2026 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "autostart.h"

#import <ServiceManagement/ServiceManagement.h>

bool IsAutoStartEnabled()
{
  SMAppService* service = [SMAppService mainAppService];
  return service.status == SMAppServiceStatusEnabled;
}

void SetAutoStart(bool enable)
{
  NSError** error = nil;
  SMAppService* service = [SMAppService mainAppService];
  if (enable) {
    [service registerAndReturnError:error];
  } else {
    [service unregisterAndReturnError:error];
  }
}
