/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "quick_note_plugin.hpp"

#include <QInputDialog>
#include <QLabel>
#include <QMouseEvent>
#include <QTimer>

class DoubleClickWidget : public GraphicsTextWidget
{
  Q_OBJECT

public:
  explicit DoubleClickWidget(QWidget* parent = nullptr);

signals:
  void doubleClicked();

protected:
  void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
  void onTimeout();

private:
  QTimer _timer;
  int _clicks_count = 0;
};


DoubleClickWidget::DoubleClickWidget(QWidget* parent)
    : GraphicsTextWidget(parent)
{
  connect(&_timer, &QTimer::timeout, this, &DoubleClickWidget::onTimeout);
}

void DoubleClickWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    if (!_timer.isActive()) {
      _timer.setSingleShot(true);
      _timer.start(250);
    }
    if (++_clicks_count == 2) {
      emit doubleClicked();
    }
  }
  GraphicsTextWidget::mouseReleaseEvent(event);
}

void DoubleClickWidget::onTimeout()
{
  _clicks_count = 0;
}


void QuickNotePlugin::saveState(StateClient& st) const
{
  st.setValue("last_text", _last_text);
}

void QuickNotePlugin::loadState(const StateClient& st)
{
  _last_text = st.value("last_text", tr("double click me!")).toString();
}

QWidget* QuickNotePlugin::customConfigure(SettingsClient& s, StateClient& t)
{
  Q_UNUSED(s);
  Q_UNUSED(t);
  auto l = new QLabel(tr("Nothing is here!\nDouble click the widget to change the text."));
  l->setAlignment(Qt::AlignCenter);
  return l;
}

std::shared_ptr<GraphicsWidgetBase> QuickNotePlugin::createWidget()
{
  auto w = std::make_shared<DoubleClickWidget>();
  w->setText(_last_text);
  connect(w.get(), &DoubleClickWidget::doubleClicked, this, &QuickNotePlugin::onWidgetClicked);
  _widget = std::move(w);
  return _widget;
}

void QuickNotePlugin::destroyWidget()
{
  _widget.reset();
}

void QuickNotePlugin::onWidgetClicked()
{
  bool ok = false;
  auto str = QInputDialog::getText(nullptr, tr("Quick note"),
                                   tr("Enter the desired note"),
                                   QLineEdit::Normal, _last_text, &ok);
  if (ok && !str.isEmpty()) {
    _last_text = str;
    _widget->setText(std::move(str));
  }
}


std::unique_ptr<ClockPluginBase> QuickNotePluginFactory::create() const
{
  return std::make_unique<QuickNotePlugin>();
}

QString QuickNotePluginFactory::description() const
{
  return tr("Allows to display any short message under the clock.");
}

#include "quick_note_plugin.moc"
