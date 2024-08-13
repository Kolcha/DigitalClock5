#include <QApplication>

#include <QGridLayout>
#include <QTimer>
#include <QWidget>
#include <QPainter>
#include <QResizeEvent>

#include "font_skin.hpp"
#include "graphics_widgets.hpp"


class State {
public:
  void setValue(QString key, QVariant val) {}
  QVariant value(QString key, QVariant def) const { return {}; }
};


// =================== widgets ===================

class MainWindow : public QWidget
{
  Q_OBJECT

public:
  enum AnchorPoint { AnchorLeft, AnchorCenter, AnchorRight };
  Q_ENUM(AnchorPoint)

  explicit MainWindow(QWidget* parent = nullptr);

  AnchorPoint anchorPoint() const { return _anchor_point; }

  GraphicsDateTimeWidget* clock() const { return _clock; }

  void addPluginWidget(GraphicsWidgetBase* w, int row, int column,
                       int row_span = 1, int column_span = 1);

  void saveState(State& s) const;
  void loadState(const State& s);

public slots:
  void setAnchorPoint(AnchorPoint ap);

signals:
  void saveStateRequested();

protected:
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

private:
  AnchorPoint _anchor_point = AnchorLeft;
  QPoint _last_origin = {400, 400};

  QGridLayout* _layout = nullptr;
  GraphicsDateTimeWidget* _clock = nullptr;
};


MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
{
  _layout = new QGridLayout(this);
  _layout->setSizeConstraint(QLayout::SetFixedSize);

  _clock = new GraphicsDateTimeWidget(this);
  _layout->addWidget(_clock);

  setLayout(_layout);
}

void MainWindow::addPluginWidget(GraphicsWidgetBase* w, int row, int column,
                                 int row_span, int column_span)
{
  _layout->addWidget(w, row, column, row_span, column_span);
}

void MainWindow::saveState(State& s) const
{
  s.setValue("state/origin", _last_origin);
  s.setValue("state/anchor", (int)_anchor_point);
}

void MainWindow::loadState(const State& s)
{
  // TODO: maybe set default point based on graphics size
  // as practice shown, it is better to avoid negative Y values :(
  _last_origin = s.value("state/origin", QPoint(300, 300)).toPoint();
  _anchor_point = (AnchorPoint)s.value("state/anchor", (int)AnchorLeft).toInt();
}

void MainWindow::setAnchorPoint(AnchorPoint ap)
{
  _anchor_point = ap;
  emit saveStateRequested();
}

void MainWindow::paintEvent(QPaintEvent* event)
{
  // TODO: draw frame when setting dialog is open
  QWidget::paintEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
  QWidget::resizeEvent(event);

  // state restore must happen before the first resize event!
  // previous origin and correct anchor point must be known here
  auto curr_origin = _clock->mapToGlobal(_clock->origin());

  switch (_anchor_point) {
    case AnchorLeft:
      move(pos() + _last_origin - curr_origin);
      break;
    case AnchorCenter:
      move(pos() + _last_origin - (curr_origin + QPoint(_clock->advance().x(), 0)/2));
      break;
    case AnchorRight:
      move(pos() + _last_origin - (curr_origin + QPoint(_clock->advance().x(), 0)));
      break;
  }
}

// test code
// ===========================

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
/*
  // GraphicsTextWidget w;
  GraphicsDateTimeWidget w;

  QFont fnt("Monaco");
  fnt.setPointSize(96);
  w.setSkin(std::make_shared<FontSkin>(fnt));
  w.setCharSpacing(-4);
  // w.setText("Test\nt x t\n😎🤓");
  // w.setFormat("s");
  // w.setUseAlternateSeparator(true);
  // w.setUseCustomSeparators(true);
  // w.setCustomSeparators("😎");
  w.setBackground(Qt::blue);
  // w.setTexture(Qt::yellow);
  // w.setDateTime(QDateTime::currentDateTime());

  QTimer timer;
  QObject::connect(&timer, &QTimer::timeout, &w, [&]() { w.setDateTime(QDateTime::currentDateTime()); });
  QObject::connect(&timer, &QTimer::timeout, &w, &GraphicsDateTimeWidget::updateSeparatorsState);
  timer.start(500);
*/
  MainWindow w;

  w.setAnchorPoint(MainWindow::AnchorRight);

  auto dtw = w.clock();

  QFont fnt("SignPainter");
  fnt.setPointSize(96);

  dtw->setSkin(std::make_shared<FontSkin>(fnt));
  // dtw->setBackground(Qt::blue);
  dtw->setTexture(QColor(128, 0, 255, 224));
  dtw->setFormat("hh:mm:ss");

  auto stw = new GraphicsTextWidget(&w);

  stw->setAutoResizePolicy(GraphicsWidgetBase::KeepWidth);
  stw->setText("Very long text string");
  stw->setSkin(std::make_shared<FontSkin>(w.font()));

  w.addPluginWidget(stw, 1, 0);

  QTimer timer;
  QObject::connect(&timer, &QTimer::timeout, dtw, [&]() { dtw->setDateTime(QDateTime::currentDateTime()); });
  QObject::connect(&timer, &QTimer::timeout, dtw, &GraphicsDateTimeWidget::updateSeparatorsState);
  timer.start(500);

  w.show();
  return a.exec();
}

#include "main.moc"
