/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "quick_note_plugin.hpp"

#include <QInputDialog>
#include <QMouseEvent>

#include "gui/settings_widget.hpp"

class DoubleClickWidget : public SkinnedTextWidget
{
  Q_OBJECT

public:
  using SkinnedTextWidget::SkinnedTextWidget;

signals:
  void doubleClicked();

protected:
  void mouseDoubleClickEvent(QMouseEvent* event) override;
};


void DoubleClickWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    emit doubleClicked();
  }
  SkinnedTextWidget::mouseDoubleClickEvent(event);
}


namespace {

QString escape(const QString& str)
{
  auto s = str.trimmed();
  return s.replace("\n", "\\n");
}

QString unescape(const QString& str)
{
  auto s = str;
  return s.replace("\\n", "\n").trimmed();
}

} // namespace


QuickNotePlugin::QuickNotePlugin(QuickNotePluginInstanceConfig* cfg, std::unique_ptr<SettingsStorage> st)
  : TextPluginInstanceBase(*cfg)
  , _cfg(cfg)
  , _st(std::move(st))
{
}

void QuickNotePlugin::startup()
{
  StateStorage st(*_st);
  if (!st.value("migration_complete", false).toBool()) {
    _last_text = unescape(st.value("last_text", tr("double click me!")).toString());
    _cfg->setLastText(escape(_last_text));
    _cfg->commit();
    st.setValue("migration_complete", true);
  }
  TextPluginInstanceBase::startup();
}

void QuickNotePlugin::onTextChanged(const QString& txt)
{
  if (txt.isEmpty()) return;
  _last_text = unescape(txt);
  repaintWidget();
}

SkinnedTextWidget* QuickNotePlugin::createWidget(QWidget* parent) const
{
  auto w = new DoubleClickWidget(parent);
  connect(w, &DoubleClickWidget::doubleClicked, this, &QuickNotePlugin::onWidgetClicked);
  return w;
}

void QuickNotePlugin::pluginReloadConfig()
{
  _last_text = unescape(_cfg->getLastText());
}

void QuickNotePlugin::onWidgetClicked()
{
  bool ok = false;
  auto str = QInputDialog::getText(nullptr, tr("Quick note"),
                                   tr("Enter the desired note"),
                                   QLineEdit::Normal, escape(_last_text), &ok);
  if (ok && !str.isEmpty()) {
    onTextChanged(str);
    _cfg->setLastText(escape(str));
    _cfg->commit();
  }
}


QString QuickNotePluginFactory::description() const
{
  return tr("Allows to display any short message under the clock.");
}

QVector<QWidget*> QuickNotePluginFactory::configPagesBeforeCommonPages()
{
  using plugin::quick_note::SettingsWidget;
  auto page = new SettingsWidget();
  connect(this, &QuickNotePluginFactory::instanceSwitched, page, [this, page](size_t idx) {
    page->initControls(qobject_cast<QuickNotePluginInstanceConfig*>(instanceConfig(idx)));
    if (!hasInstances()) return;
    disconnect(page, &SettingsWidget::textChanged, nullptr, nullptr);
    auto inst = qobject_cast<QuickNotePlugin*>(instance(idx));
    connect(page, &SettingsWidget::textChanged, inst, &QuickNotePlugin::onTextChanged);
  });
  return { page };
}

#include "quick_note_plugin.moc"
