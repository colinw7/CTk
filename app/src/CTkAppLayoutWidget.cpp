#include <CTkAppLayout.h>
#include <CTkApp.h>
#include <CTkAppWidget.h>

#include <QApplication>
#include <QWidget>

Qt::Orientations
CTkLayoutWidget::
expandingDirections() const
{
  return Qt::Horizontal | Qt::Vertical;
}

QRect
CTkLayoutWidget::
geometry() const
{
  return getQWidget()->geometry();
}

bool
CTkLayoutWidget::
hasHeightForWidth() const
{
  // preferred height depends on width
  return false;
}

int
CTkLayoutWidget::
heightForWidth(int w) const
{
  return w;
}

bool
CTkLayoutWidget::
isEmpty() const
{
  return false;
}

QSize
CTkLayoutWidget::
maximumSize() const
{
  return getQWidget()->maximumSize();
}

QSize
CTkLayoutWidget::
minimumSize() const
{
  //return getQWidget()->minimumSize();
  return sizeHint();
}

void
CTkLayoutWidget::
setGeometry(const QRect &rect)
{
  return getQWidget()->setGeometry(rect);
}

QSize
CTkLayoutWidget::
sizeHint() const
{
  auto s = QSize(widget_->getWidth(), widget_->getHeight());

  s = s.expandedTo(QApplication::globalStrut());

  return s;
}

QWidget *
CTkLayoutWidget::
widget()
{
  return widget_->getQWidget();
}

QWidget *
CTkLayoutWidget::
getQWidget() const
{
  return widget_->getQWidget();
}
