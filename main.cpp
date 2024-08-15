#include <QApplication>

#include <QGridLayout>
#include <QTimer>
#include <QWidget>
#include <QPainter>
#include <QResizeEvent>

#include "config/app_config.hpp"
#include "skin/font_skin.hpp"
#include "skin/graphics_widgets.hpp"


class State {
public:
  virtual ~State() = default;

  virtual void setValue(QString key, QVariant val) = 0;
  virtual QVariant value(QString key, QVariant def) const = 0;
};


// this is maybe overkill, but window should not know
// about any config implementation details
class StateImpl : public State {
public:
  explicit StateImpl(StateClient& scl) : _scl(scl) {}

  void setValue(QString key, QVariant val) override
  {
    _scl.setValue(std::move(key), std::move(val));
  }

  QVariant value(QString key, QVariant def) const override
  {
    return _scl.value(std::move(key), std::move(def));
  }

public:
  StateClient& _scl;
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

  bool snapToEdge() const { return _snap_to_edge; }
  int snapThreshold() const { return _snap_threshold; }

  void addPluginWidget(GraphicsWidgetBase* w, int row, int column,
                       int row_span = 1, int column_span = 1);

  void saveState(State& s) const;
  void loadState(const State& s);

public slots:
  void setAnchorPoint(AnchorPoint ap);

  void enableSnapToEdge() { setSnapToEdge(true, snapThreshold()); }
  void disableSnapToEdge() { setSnapToEdge(false, snapThreshold()); }
  void setSnapToEdge(bool enable, int threshold);
  void setSnapThreshold(int thr) { setSnapToEdge(snapToEdge(), thr); }

signals:
  void saveStateRequested();

protected:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

private:
  AnchorPoint _anchor_point = AnchorLeft;
  QPoint _last_origin = {400, 400};

  QGridLayout* _layout = nullptr;
  GraphicsDateTimeWidget* _clock = nullptr;

  bool _is_dragging = false;
  QPoint _drag_pos;

  bool _snap_to_edge = true;
  int _snap_threshold = 15;
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

void MainWindow::setSnapToEdge(bool enable, int threshold)
{
  _snap_to_edge = enable;
  _snap_threshold = threshold;
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    _drag_pos = event->globalPosition().toPoint() - frameGeometry().topLeft();
    _is_dragging = true;
    event->accept();
  }
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
  if (event->buttons() & Qt::LeftButton) {
    QPoint target_pos = event->globalPosition().toPoint() - _drag_pos;
    if (_snap_to_edge) {
      QRect screen = window()->screen()->availableGeometry();
      QRect widget = frameGeometry();
      if (qAbs(target_pos.x() - screen.left()) <= _snap_threshold)
        target_pos.setX(screen.left());
      if (qAbs(target_pos.y() - screen.top()) <= _snap_threshold)
        target_pos.setY(screen.top());
      if (qAbs(screen.right() - (target_pos.x() + widget.width())) <= _snap_threshold)
        target_pos.setX(screen.right() - widget.width());
      if (qAbs(screen.bottom() - (target_pos.y() + widget.height())) <= _snap_threshold)
        target_pos.setY(screen.bottom() - widget.height());
    }
    move(target_pos);
    event->accept();
  }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    auto curr_origin = _clock->mapToGlobal(_clock->origin());

    switch (_anchor_point) {
      case AnchorLeft:
        _last_origin = curr_origin;
        break;
      case AnchorCenter:
        _last_origin = curr_origin + QPoint(_clock->advance().x(), 0)/2;
        break;
      case AnchorRight:
        _last_origin = curr_origin + QPoint(_clock->advance().x(), 0);
        break;
    }

    emit saveStateRequested();

    _is_dragging = false;
    event->accept();
  }
}

void MainWindow::paintEvent(QPaintEvent* event)
{
  // TODO: draw frame when setting dialog is open
  QWidget::paintEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
  QWidget::resizeEvent(event);

  // do not apply anhoring logic during dragging
  if (_is_dragging) return;

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
  AppConfig cfg("test.ini");
  StateImpl wnd_state(cfg.slice(0).state());

  MainWindow w;
  w.loadState(wnd_state);

  w.setAnchorPoint(MainWindow::AnchorRight);

  auto dtw = w.clock();

  QFont fnt("SignPainter");
  fnt.setPointSize(96);

  dtw->setSkin(std::make_shared<FontSkin>(fnt));
  // dtw->setBackground(Qt::blue);
  dtw->setTexture(QColor(128, 0, 255, 224));
  // dtw->setTimeZone(QTimeZone::utc());
  dtw->setFormat("hh:mm:ss");

  auto stw = new GraphicsTextWidget(&w);

  stw->setAutoResizePolicy(GraphicsWidgetBase::KeepWidth);
  stw->setText("Very long text string");
  stw->setSkin(std::make_shared<FontSkin>(w.font()));

  w.addPluginWidget(stw, 1, 0);
  // w.layout()->setAlignment(stw, Qt::AlignCenter);

  QTimer timer;
  QObject::connect(&timer, &QTimer::timeout, dtw, [&]() { dtw->setDateTime(QDateTime::currentDateTime()); });
  QObject::connect(&timer, &QTimer::timeout, dtw, &GraphicsDateTimeWidget::updateSeparatorsState);
  timer.start(500);

  QObject::connect(&w, &MainWindow::saveStateRequested, &w, [&]() { w.saveState(wnd_state); });

  w.show();
  return a.exec();
}

#include "main.moc"
