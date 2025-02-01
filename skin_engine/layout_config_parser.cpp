// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "layout_config_parser.hpp"

LinesRenderer::Options parse_layout_options(QStringView cfg)
{
  LinesRenderer::Options opt;

  for (qsizetype i = 0; i < cfg.length(); i++) {
    switch (cfg[i].unicode()) {
      case '1':
        opt.enableLineScaling(i);
        break;
      case '<':
        opt.setLineAlignment(i, Qt::AlignLeft);
        break;
      case '>':
        opt.setLineAlignment(i, Qt::AlignRight);
        break;
      case 'x':
      case 'X':
        opt.setLineAlignment(i, Qt::AlignHCenter);
        break;
      case '0':
        break;
      default:
        break;
    }
  }

  return opt;
}
