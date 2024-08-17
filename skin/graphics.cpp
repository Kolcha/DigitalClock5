/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "graphics.hpp"

#include "datetime_formatter.hpp"

GraphicsBase::GraphicsBase()
    : _char_a(std::make_shared<Appearance>())
    , _text_a(std::make_shared<Appearance>())
{
  _tg.setAppearance(_text_a);
}

void GraphicsBase::setSkin(std::shared_ptr<Skin> skin)
{
  _skin = std::move(skin);
  rebuild();
}

void GraphicsBase::setCharSpacing(qreal spacing)
{
  _char_spacing = spacing;
  rebuild();
}

void GraphicsBase::setLineSpacing(qreal spacing)
{
  _line_spacing = spacing;
  _tg.algorithm()->setSpacing(_line_spacing);
  _tg.updateGeometry();
}

void GraphicsBase::setAlignment(Qt::Alignment a)
{
  _tg.algorithm()->setAlignment(a);
  _tg.updateGeometry();
}

void GraphicsBase::setBackground(QBrush b, bool stretch, bool per_char)
{
  _bg = std::move(b);
  _bg_s = stretch;
  _bg_pc = per_char;
  update();
}

void GraphicsBase::setTexture(QBrush b, bool stretch, bool per_char)
{
  _tx = std::move(b);
  _tx_s = stretch;
  _tx_pc = per_char;
  update();
}

void GraphicsBase::rebuild()
{
  if (!_skin) {
    _tg.clear();
    clearLayout();
    return;
  }

  buildLayout();

  _tg.updateGeometry();
}

void GraphicsBase::update()
{
  _char_a->setBackground(_bg_pc ? _bg : Qt::NoBrush);
  _char_a->setTexture(_tx_pc ? _tx : Qt::NoBrush);
  _char_a->setBackgroundStretch(_bg_pc && _bg_s);
  _char_a->setTextureStretch(_tx_pc && _tx_s);

  _text_a->setBackground(_bg_pc ? Qt::NoBrush : _bg);
  _text_a->setTexture(_tx_pc ? Qt::NoBrush : _tx);
  _text_a->setBackgroundStretch(!_bg_pc && _bg_s);
  _text_a->setTextureStretch(!_tx_pc && _tx_s);
}


GraphicsText::GraphicsText(QString text)
    : GraphicsBase()
    , _text(text)
{
  rebuild();
}

void GraphicsText::setText(QString text)
{
  if (_text == text) return;
  _text = std::move(text);
  rebuild();
}

void GraphicsText::clearLayout()
{
  _lines.clear();
}

void GraphicsText::buildLayout()
{
  vcontainer().clear();
  _lines.clear();

  const auto str_lines = _text.split('\n');
  _lines.reserve(str_lines.size());

  for (const auto& sl : str_lines) {
    const auto code_points = sl.toUcs4();
    QVector<std::shared_ptr<Glyph>> line_v;
    line_v.reserve(code_points.size());

    auto line_c = std::make_shared<HContainerGlyph>();
    line_c->algorithm()->setSpacing(charSpacing());

    for (const auto c : code_points) {
      auto sg = skin()->glyph(c);
      if (!sg) continue;

      auto g = std::make_shared<Glyph>(std::move(sg));
      g->setAppearance(charAppearance());
      line_c->addGlyph(g);
      line_v.push_back(g);
    }

    line_c->updateGeometry();

    vcontainer().addGlyph(std::move(line_c));
    _lines.push_back(std::move(line_v));
  }
}


class DateTimeTokenizer : public DateTimeStringBuilder {
public:
  struct Token {
    QString key;
    QVector<char32_t> val;
    bool sep = false;
  };
  using TokensList = QVector<Token>;

  void addCharacter(char32_t c) override;
  void addSeparator(char32_t c) override;

  void tokenStart(QStringView t) override;
  void tokenEnd(QStringView t) override;

  TokensList tokens() const { return _tokens; }

private:
  void addSingleCharToken(char32_t c, bool sep);

private:
  TokensList _tokens;
  std::optional<Token> _curr;
};


void DateTimeTokenizer::addCharacter(char32_t c)
{
  if (_curr.has_value()) {
    _curr->val.push_back(c);
  } else {
    addSingleCharToken(c, false);
  }
}

void DateTimeTokenizer::addSeparator(char32_t c)
{
  Q_ASSERT(!_curr.has_value());
  addSingleCharToken(c, true);
}

void DateTimeTokenizer::tokenStart(QStringView t)
{
  Q_ASSERT(!_curr.has_value());
  _curr.emplace();
  _curr->key = t.toString();
}

void DateTimeTokenizer::tokenEnd(QStringView t)
{
  Q_ASSERT(_curr.has_value());
  Q_ASSERT(_curr->key == t);
  Q_ASSERT(!_curr->val.empty());
  _tokens.push_back(*_curr);
  _curr.reset();
}

void DateTimeTokenizer::addSingleCharToken(char32_t c, bool s)
{
  _tokens.push_back({ .key = QString::fromUcs4(&c, 1), .val = {c}, .sep = s });
}


GraphicsDateTime::GraphicsDateTime(QDateTime dt)
    : GraphicsBase()
    , _dt(std::move(dt))
    , _tz(_dt.timeZone())
    , _fmt(QLocale::system().timeFormat(QLocale::ShortFormat))
{
  rebuild();
}

QTransform GraphicsDateTime::tokenTransform(QString token) const
{
  auto iter = _tt.find(token);
  return iter != _tt.end() ? *iter : QTransform();
}

void GraphicsDateTime::setDateTime(QDateTime dt)
{
  if (_dt == dt) return;
  _dt = std::move(dt);
  rebuild();
}

void GraphicsDateTime::setTimeZone(QTimeZone tz)
{
  if (_tz == tz) return;
  _tz = std::move(tz);
  rebuild();
}

void GraphicsDateTime::setFormat(QString fmt)
{
  if (_fmt == fmt) return;
  _fmt = std::move(fmt);
  rebuild();
}

void GraphicsDateTime::setFlashSeparator(bool en)
{
  _flash_seps = en;
  if (!en) _sep_visible = true;
  rebuild();
}

void GraphicsDateTime::setUseAlternateSeparator(bool en)
{
  _use_alt_sep = en;
  rebuild();
}

void GraphicsDateTime::setUseCustomSeparators(bool en)
{
  _use_cust_seps = en;
  rebuild();
}

void GraphicsDateTime::setCustomSeparators(QString seps)
{
  _custom_seps = std::move(seps);
  rebuild();
}

void GraphicsDateTime::updateSeparatorsState()
{
  if (_flash_seps)
    _sep_visible = !_sep_visible;

  if (!_use_alt_sep)
    for (const auto& s : _seps)
      s->setVisible(_sep_visible);
}

void GraphicsDateTime::setTokenTransform(QString token, QTransform t)
{
  _tt[token] = std::move(t);
  rebuild();
}

void GraphicsDateTime::removeTokenTransform(QString token)
{
  _tt.remove(token);
  rebuild();
}

void GraphicsDateTime::clearTokenTransform()
{
  _tt.clear();
  rebuild();
}

void GraphicsDateTime::clearLayout()
{
  _lines.clear();
  _seps.clear();
}

void GraphicsDateTime::buildLayout()
{
  DateTimeTokenizer dtkz;
  FormatDateTime(_dt.toTimeZone(_tz), _fmt, dtkz);

  auto dt_tokens = dtkz.tokens();
  QStringList fmt_tokens(dt_tokens.size());
  std::transform(dt_tokens.begin(), dt_tokens.end(),
                 fmt_tokens.begin(), [](const auto& t) { return t.key; });

  // format changed
  if (fmt_tokens != _last_tokens) {
    vcontainer().clear();
    _lines.clear();
    _seps.clear();
    _last_tokens = fmt_tokens;
  }

  // update separators
  if (_use_alt_sep && skin()->hasAlternateSeparator()) {
    for (auto& t : dt_tokens) {
      if (t.sep) {
        t.val.clear();
        t.val.push_back(_sep_visible ? ':' : ' ');
      }
    }
  }

  if (_use_cust_seps && skin()->supportsCustomSeparator()) {
    const auto custom_seps = _custom_seps.toUcs4();
    qsizetype sep_idx = 0;
    for (auto& t : dt_tokens) {
      if (t.sep && sep_idx < custom_seps.size()) {
        t.val.clear();
        t.val.push_back(custom_seps[sep_idx]);
        sep_idx++;
      }
    }
  }

  // build layout
  _lines.empty() ? rebuildLayout(dt_tokens) : updateLayout(dt_tokens);
}

void GraphicsDateTime::rebuildLayout(const auto& dt_tokens)
{
  auto line_c = std::make_shared<HContainerGlyph>();
  Line line_v;

  for (const auto& t : dt_tokens) {
    if (t.key.front() == '\n') {
      line_c->updateGeometry();
      vcontainer().addGlyph(std::move(line_c));

      _lines.push_back(std::move(line_v));

      line_c = std::make_shared<HContainerGlyph>();
      line_v = {};

      continue;
    }

    auto token_c = std::make_shared<HContainerGlyph>();
    LineToken token_v;
    token_v.reserve(t.val.size());

    for (const auto c : t.val) {
      auto g = std::make_shared<Glyph>(skin()->glyph(c));
      g->setAppearance(charAppearance());

      if (t.sep) _seps.push_back(g);

      token_c->addGlyph(g);
      token_v.push_back(std::move(g));
    }

    if (auto iter = _tt.find(t.key); iter != _tt.end())
      token_c->transform()->setTransform(*iter);

    token_c->algorithm()->setSpacing(charSpacing());
    token_c->updateGeometry();
    line_c->addGlyph(std::move(token_c));
    line_v.push_back(std::move(token_v));
  }

  line_c->algorithm()->setSpacing(charSpacing());
  line_c->updateGeometry();
  vcontainer().addGlyph(std::move(line_c));

  _lines.push_back(std::move(line_v));
}

void GraphicsDateTime::updateLayout(const auto& dt_tokens)
{
  qsizetype cl = 0;
  qsizetype ct = 0;

  for (const auto& t : dt_tokens) {
    auto& curr_line = static_cast<HContainerGlyph&>(*vcontainer()[cl]);
    curr_line.algorithm()->setSpacing(charSpacing());

    if (t.key.front() == '\n') {
      curr_line.updateGeometry();
      cl++;
      ct = 0;
      continue;
    }

    if (_lines[cl][ct].size() != t.val.size()) {
      // replace token
      auto new_token = std::make_shared<HContainerGlyph>();
      _lines[cl][ct].clear();
      for (const auto c : t.val) {
        auto g = std::make_shared<Glyph>(skin()->glyph(c));
        g->setAppearance(charAppearance());
        new_token->addGlyph(g);
        _lines[cl][ct].push_back(std::move(g));
      }
      if (auto iter = _tt.find(t.key); iter != _tt.end())
        new_token->transform()->setTransform(*iter);
      new_token->algorithm()->setSpacing(charSpacing());
      new_token->updateGeometry();
      curr_line[ct] = std::move(new_token);
    } else {
      // update token
      auto& curr_token = static_cast<HContainerGlyph&>(*curr_line[ct]);
      for (qsizetype i = 0; i < t.val.size(); i++) {
        _lines[cl][ct][i]->setGlyph(skin()->glyph(t.val[i]));
      }
      if (auto iter = _tt.find(t.key); iter != _tt.end())
        curr_token.transform()->setTransform(*iter);
      else
        curr_token.transform()->setTransform(QTransform());
      curr_token.algorithm()->setSpacing(charSpacing());
      curr_token.updateGeometry();
    }

    ct++;
  }

  static_cast<HContainerGlyph&>(*vcontainer()[cl]).updateGeometry();
}
