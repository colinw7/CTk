#include <CTkAppLayoutWidget.h>
#include <CTkAppWidget.h>
#include <CTkAppLayout.h>

#include <QApplication>
#include <QWidget>

CTkAppLayoutWidget::
CTkAppLayoutWidget(CTkAppLayout *l, TkWidget *widget) :
 QObject(l), l_(l), widget_(widget)
{
  init();
}

CTkAppLayoutWidget::
CTkAppLayoutWidget(CTkAppLayout *l, QLayout *layout) :
 QObject(l), l_(l), layout_(layout)
{
  init();
}

void
CTkAppLayoutWidget::
init()
{
  static int ind = 1;

  auto name = QString("lw%1").arg(ind++);

  setObjectName(name);
}

CTkAppWidget *
CTkAppLayoutWidget::
getTkWidget() const
{
  return widget_.data();
}

Qt::Orientations
CTkAppLayoutWidget::
expandingDirections() const
{
  return Qt::Horizontal | Qt::Vertical;
}

QRect
CTkAppLayoutWidget::
geometry() const
{
  QRect rect;

  if (widget_)
    rect = widget_->getQWidget()->geometry();
  else if (layout_)
    rect = layout_->geometry();

  return rect;
}

bool
CTkAppLayoutWidget::
hasHeightForWidth() const
{
  // preferred height depends on width
  return false;
}

int
CTkAppLayoutWidget::
heightForWidth(int w) const
{
  return w;
}

bool
CTkAppLayoutWidget::
isEmpty() const
{
  return false;
}

QSize
CTkAppLayoutWidget::
maximumSize() const
{
  QSize s;

  if      (widget_)
    s = widget_->getQWidget()->maximumSize();
  else if (layout_)
    s = layout_->maximumSize();

  return s;
}

QSize
CTkAppLayoutWidget::
minimumSize() const
{
  QSize s;

  if      (widget_)
    s = widget_->getQWidget()->minimumSize();
  else if (layout_)
    s = layout_->minimumSize();

  return s;
}

void
CTkAppLayoutWidget::
setGeometry(const QRect &rect)
{
  if      (widget_)
    widget_->getQWidget()->setGeometry(rect);
  else if (layout_)
    layout_->setGeometry(rect);
}

void
CTkAppLayoutWidget::
show()
{
  if (widget_)
    widget_->show();
}

QMargins
CTkAppLayoutWidget::
contentsMargins() const
{
  QMargins m;

  if      (widget_)
    m = widget_->contentsMargins();
  else if (layout_)
    m = layout_->contentsMargins();

  return m;
}

QSize
CTkAppLayoutWidget::
sizeHint() const
{
  QSize s;

  if      (widget_)
    s = widget_->sizeHint();
  else if (layout_)
    s = layout_->sizeHint();

  s = s.expandedTo(QApplication::globalStrut());

  return s;
}

QSize
CTkAppLayoutWidget::
minimumSizeHint() const
{
  QSize s;

  if      (widget_)
    s = widget_->minimumSizeHint();
  else if (layout_)
    s = layout_->sizeHint();

  s = s.expandedTo(QApplication::globalStrut());

  return s;
}

QWidget *
CTkAppLayoutWidget::
widget()
{
  if      (widget_)
    return widget_->getQWidget();
  else if (layout_)
    return layout_->widget();
  else
    return nullptr;
}

QWidget *
CTkAppLayoutWidget::
getQWidget() const
{
  if      (widget_)
    return widget_->getQWidget();
  else if (layout_)
    return layout_->widget();
  else
    return nullptr;
}

QString
CTkAppLayoutWidget::
name() const
{
  if (widget_)
    return widget_->getName();
  else
    return "layout";
}
