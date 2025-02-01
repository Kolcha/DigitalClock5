/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "quick_note_plugin.hpp"

#include <QInputDialog>
#include <QLabel>
#include <QMouseEvent>

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


QuickNotePlugin::QuickNotePlugin(const PluginInstanceConfig& cfg, std::unique_ptr<SettingsStorage> st)
  : TextPluginInstanceBase(cfg)
  , _st(std::move(st))
{
}

void QuickNotePlugin::startup()
{
  const StateStorage st(*_st);
  _last_text = st.value("last_text", tr("double click me!")).toString();
  TextPluginInstanceBase::startup();
}

SkinnedTextWidget* QuickNotePlugin::createWidget(QWidget* parent) const
{
  auto w = new DoubleClickWidget(parent);
  connect(w, &DoubleClickWidget::doubleClicked, this, &QuickNotePlugin::onWidgetClicked);
  return w;
}

void QuickNotePlugin::onWidgetClicked()
{
  bool ok = false;
  auto str = QInputDialog::getText(nullptr, tr("Quick note"),
                                   tr("Enter the desired note"),
                                   QLineEdit::Normal, _last_text, &ok);
  if (ok && !str.isEmpty()) {
    _last_text = str;
    StateStorage st(*_st);
    st.setValue("last_text", str);
    repaintWidget();
  }
}


QString QuickNotePluginFactory::description() const
{
  return tr("Allows to display any short message under the clock.");
}

QVector<QWidget*> QuickNotePluginFactory::configPagesBeforeCommonPages()
{
  auto l = new QLabel(tr("Nothing is here!\nDouble click the widget to change the text."));
  l->setAlignment(Qt::AlignCenter);
  l->setWindowTitle(tr("Note"));
  return {l};
}

#include "quick_note_plugin.moc"
