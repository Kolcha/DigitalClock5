// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "time_renderer.hpp"

#include "renderer/item.hpp"
#include "renderer/layout.hpp"

#include "utilities/datetime_formatter.hpp"

class LinesBuilder : public DateTimeStringBuilder
{
public:
  explicit LinesBuilder(const TimeRenderer& r)
    : _r(r)
    , _custom_seps(_r._custom_seps.toUcs4())
  {}

  void addCharacter(char32_t ch) override;
  void addSeparator(char32_t ch) override;

  void tokenStart(const QString& token) override;
  void tokenEnd(const QString& token) override;

  LinesRenderer::Lines lines();

private:
  char32_t actualSeparator(char32_t ch) const;

  void switchFragment();

  std::unique_ptr<Renderable> buildCurrentLine();
  void switchCurrentLine();

private:
  const TimeRenderer& _r;

  QList<uint> _custom_seps;   // type matches QString::toUcs4()
  uint _curr_sep_idx = 0;     // used for custom separators

  QString _curr_token;        // non-empty only for tokens
  QString _curr_fragment;     // token or arbitrary text
  LayoutItemsContainer _curr_line;

  LinesRenderer::Lines _lines;
};


void LinesBuilder::addCharacter(char32_t ch)
{
  if (ch == '\n')
    switchCurrentLine();
  else
    _curr_fragment.append(QString::fromUcs4(&ch, 1));
}

void LinesBuilder::addSeparator(char32_t ch)
{
  switchFragment();

  ch = actualSeparator(ch);

  if (auto s = _r.skin()) {
    auto sr = s->draw(QString::fromUcs4(&ch, 1));
    auto si = std::make_unique<Item>(std::move(sr));
    si->setVisible(_r._separator_visible || ch == ' ');
    _curr_line.push_back(std::move(si));
  }

  ++_curr_sep_idx;
}

void LinesBuilder::tokenStart(const QString& token)
{
  switchFragment();
  _curr_token = token;
}

void LinesBuilder::tokenEnd(const QString& token)
{
  Q_UNUSED(token);
  switchFragment();
  _curr_token.clear();
}

LinesRenderer::Lines LinesBuilder::lines()
{
  switchCurrentLine();
  return std::move(_lines);
}

char32_t LinesBuilder::actualSeparator(char32_t ch) const
{
  if (auto s = _r.skin()) {
    // custom separator
    if (_curr_sep_idx < _custom_seps.size() && s->supports(_custom_seps[_curr_sep_idx]))
      return _custom_seps[_curr_sep_idx];

    // alternate skin separator
    if (!_r._separator_visible && _r._skin_has_2_seps)
      return ' ';
  }
  // default separator
  return ch;
}

void LinesBuilder::switchFragment()
{
  if (_curr_fragment.isEmpty())
    return;

  if (auto s = _r.skin()) {
    auto tr = s->draw(_curr_fragment);
    auto ti = std::make_unique<Item>(std::move(tr));
    ti->setTransform(_r._token_transforms.value(_curr_token));
    _curr_line.push_back(std::move(ti));
  }
  _curr_fragment.clear();
}

std::unique_ptr<Renderable> LinesBuilder::buildCurrentLine()
{
  layout_horizontally(_curr_line, _r.charSpacing());
  return std::make_unique<CompositeRenderable>(std::move(_curr_line));
}

void LinesBuilder::switchCurrentLine()
{
  if (!_curr_fragment.isEmpty()) {
    switchFragment();
  }

  if (_curr_line.empty()) {
    auto p = _r.buildEmptyLinePlaceholder();
    _curr_line.push_back(std::make_unique<Item>(std::move(p)));
  }

  _lines.push_back(buildCurrentLine());
  _curr_line.clear();
}


std::unique_ptr<Renderable> TimeRenderer::draw(const QDateTime& dt) const
{
  LinesBuilder builder(*this);
  FormatDateTime(dt, _format, builder);
  return LinesRenderer::draw(builder.lines(), _options);
}

void TimeRenderer::setFlashingSeparator(bool en) noexcept
{
  if (!en)
    _separator_visible = true;
  _flashing_separator = en;
}

void TimeRenderer::animateSeparator() noexcept
{
  if (!_flashing_separator)
    return;

  _separator_visible = !_separator_visible;
}

void TimeRenderer::onSkinChange()
{
  if (auto s = skin()) {
    auto s1 = s->draw(":");
    auto s2 = s->draw(" ");
    _skin_has_2_seps =
      s1 && s2 &&
      qFuzzyCompare(s1->geometry().width(), s2->geometry().width());
  }
}
