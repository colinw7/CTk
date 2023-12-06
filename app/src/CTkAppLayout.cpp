#include <CTkAppLayout.h>
#include <CTkAppWidget.h>

CTkAppLayout::
CTkAppLayout(CTkAppWidget *widget, const Type &type) :
 QLayout(widget->getQWidget()), widget_(widget), type_(type)
{
}

CTkAppLayout::
CTkAppLayout(const Type &type) :
 QLayout(nullptr), type_(type)
{
}

CTkAppWidget *
CTkAppLayout::
widget() const
{
  return widget_.data();
}
