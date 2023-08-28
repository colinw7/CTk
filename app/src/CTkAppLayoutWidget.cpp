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
  if (widget_)
    return widget_->getQWidget()->geometry();
  else
    return layout_->geometry();
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
  if (widget_)
    return widget_->getQWidget()->maximumSize();
  else
    return layout_->maximumSize();
}

QSize
CTkAppLayoutWidget::
minimumSize() const
{
  if (widget_)
    return widget_->getQWidget()->minimumSize();
  else
    return layout_->minimumSize();
}

void
CTkAppLayoutWidget::
setGeometry(const QRect &rect)
{
  if (widget_)
    widget_->getQWidget()->setGeometry(rect);
  else
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
  if (widget_)
    return widget_->contentsMargins();
  else
    return layout_->contentsMargins();
}

QSize
CTkAppLayoutWidget::
sizeHint() const
{
  QSize s;

  if (widget_)
    s = widget_->sizeHint();
  else
    s = layout_->sizeHint();

  s = s.expandedTo(QApplication::globalStrut());

  return s;
}

QSize
CTkAppLayoutWidget::
minimumSizeHint() const
{
  QSize s;

  if (widget_)
    s = widget_->minimumSizeHint();
  else
    s = layout_->sizeHint();

  s = s.expandedTo(QApplication::globalStrut());

  return s;
}

QWidget *
CTkAppLayoutWidget::
widget()
{
  if (widget_)
    return widget_->getQWidget();
  else
    return layout_->widget();
}

QWidget *
CTkAppLayoutWidget::
getQWidget() const
{
  if (widget_)
    return widget_->getQWidget();
  else
    return layout_->widget();
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
