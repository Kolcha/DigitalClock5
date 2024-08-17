#include "graphics_widgets.hpp"

#include <QPainter>

GraphicsWidgetBase::GraphicsWidgetBase(std::shared_ptr<GraphicsBase> gr, QWidget* parent)
    : QWidget(parent)
    , _gt(std::move(gr))
{
  Q_ASSERT(_gt);
}

QSize GraphicsWidgetBase::sizeHint() const
{
  auto [w, h] = _k * _gt->rect().size();
  return QSize(_sx * w, _sy * h);
}

QPoint GraphicsWidgetBase::origin() const
{
  // keep in sync with paint transformations!
  const auto zp = QPoint(0, 0);
  const auto tl = -_gt->rect().topLeft();
  return QTransform().scale(_k*_sx, _k*_sy).translate(tl.x(), tl.y()).map(zp);
}

QPointF GraphicsWidgetBase::advance() const
{
  auto [dx, dy] = _k * _gt->advance();
  return {_sx * dx, _sy * dy};
}

void GraphicsWidgetBase::setSkin(std::shared_ptr<Skin> skin)
{
  _gt->setSkin(std::move(skin));
  updateGeometry();
  update();
}

void GraphicsWidgetBase::setAutoResizePolicy(AutoResizePolicy rp)
{
  switch (rp) {
    case None:
      setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
      break;
    case KeepWidth:
      setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
      break;
    case KeepHeight:
      setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
      break;
  }

  _rp = rp;
  _k = 1.0;
  update();
}

void GraphicsWidgetBase::setCharSpacing(qreal spacing)
{
  _gt->setCharSpacing(spacing);
  updateGeometry();
  update();
}

void GraphicsWidgetBase::setLineSpacing(qreal spacing)
{
  _gt->setLineSpacing(spacing);
  updateGeometry();
  update();
}

void GraphicsWidgetBase::setScaling(qreal sx, qreal sy)
{
  _sx = sx;
  _sy = sy;
  updateGeometry();
  update();
}

void GraphicsWidgetBase::setAlignment(Qt::Alignment a)
{
  _gt->setAlignment(a);
  update();
}

void GraphicsWidgetBase::setBackground(QBrush b, bool stretch, bool per_char)
{
  _gt->setBackground(std::move(b), stretch, per_char);
  update();
}

void GraphicsWidgetBase::setTexture(QBrush b, bool stretch, bool per_char)
{
  _gt->setTexture(std::move(b), stretch, per_char);
  update();
}

void GraphicsWidgetBase::paintEvent(QPaintEvent* event)
{
  const auto& sz = _gt->rect().size();
  const auto&  hint = sizeHint();

  bool geometry_changed = false;

  switch (_rp) {
    case None:
      _k = 1.0;
      break;

    case KeepWidth:
      if (qAbs(width() - hint.width()) > 1) {
        _k = width() / sz.width() / _sx;
        geometry_changed = true;
      }
      break;

    case KeepHeight:
      if (qAbs(height() - hint.height()) > 1) {
        _k = height() / sz.height() / _sy;
        geometry_changed = true;
      }
      break;
  }

  if (geometry_changed) {
    updateGeometry();
    update();
    return;
  }

  QWidget::paintEvent(event);

  QPainter p(this);
  p.scale(_k*_sx, _k*_sy);
  p.setRenderHint(QPainter::Antialiasing);
  p.translate(-_gt->rect().topLeft());
  _gt->draw(&p);
}


GraphicsTextWidget::GraphicsTextWidget(QWidget* parent)
    : GraphicsTextWidget(std::make_shared<GraphicsText>(), parent)
{
}

void GraphicsTextWidget::setText(QString text)
{
  _gt->setText(std::move(text));
  updateGeometry();
  update();
}

GraphicsTextWidget::GraphicsTextWidget(std::shared_ptr<GraphicsText> gt, QWidget* parent)
    : GraphicsWidgetBase(gt, parent)
    , _gt(std::move(gt))
{
  Q_ASSERT(_gt);
}


GraphicsDateTimeWidget::GraphicsDateTimeWidget(QWidget* parent)
    : GraphicsDateTimeWidget(std::make_shared<GraphicsDateTime>(), parent)
{
}

void GraphicsDateTimeWidget::setDateTime(QDateTime dt)
{
  _gt->setDateTime(std::move(dt));
  updateGeometry();
  update();
}

void GraphicsDateTimeWidget::setTimeZone(QTimeZone tz)
{
  _gt->setTimeZone(std::move(tz));
  updateGeometry();
  update();
}

void GraphicsDateTimeWidget::setFormat(QString fmt)
{
  _gt->setFormat(std::move(fmt));
  updateGeometry();
  update();
}

void GraphicsDateTimeWidget::setFlashSeparator(bool en)
{
  _gt->setFlashSeparator(en);
  updateGeometry();
  update();
}

void GraphicsDateTimeWidget::setUseAlternateSeparator(bool en)
{
  _gt->setUseAlternateSeparator(en);
  updateGeometry();
  update();
}

void GraphicsDateTimeWidget::setUseCustomSeparators(bool en)
{
  _gt->setUseCustomSeparators(en);
  updateGeometry();
  update();
}

void GraphicsDateTimeWidget::setCustomSeparators(QString seps)
{
  _gt->setCustomSeparators(std::move(seps));
  updateGeometry();
  update();
}

void GraphicsDateTimeWidget::updateSeparatorsState()
{
  _gt->updateSeparatorsState();
  update();
}

void GraphicsDateTimeWidget::setSecondsScaleFactor(qreal sf)
{
  setTokenTransform("ss", QTransform::fromScale(sf, sf));
}

void GraphicsDateTimeWidget::setTokenTransform(QString token, QTransform t)
{
  _gt->setTokenTransform(std::move(token), std::move(t));
  updateGeometry();
  update();
}

void GraphicsDateTimeWidget::removeTokenTransform(QString token)
{
  _gt->removeTokenTransform(std::move(token));
  updateGeometry();
  update();
}

void GraphicsDateTimeWidget::clearTokenTransform()
{
  _gt->clearTokenTransform();
  updateGeometry();
  update();
}

GraphicsDateTimeWidget::GraphicsDateTimeWidget(std::shared_ptr<GraphicsDateTime> gt, QWidget* parent)
    : GraphicsWidgetBase(gt, parent)
    , _gt(std::move(gt))
{
  Q_ASSERT(_gt);
}
