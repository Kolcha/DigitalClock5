// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtGui/QPainter>

class PainterGuard
{
public:
  inline explicit PainterGuard(QPainter& p) : _p(p) { _p.save(); }
  inline ~PainterGuard() { _p.restore(); }

private:
  QPainter& _p;
};
