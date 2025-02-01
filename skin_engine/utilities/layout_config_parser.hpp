// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "skin_engine_global.hpp"

#include <QStringView>

#include "renderer/renderer.hpp"

SKIN_ENGINE_EXPORT
LinesRenderer::Options parse_layout_options(QStringView cfg);
