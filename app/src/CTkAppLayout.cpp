#include <CTkAppLayout.h>
#include <CTkAppWidget.h>

CTkAppLayout::
CTkAppLayout(CTkAppWidget *w, const Type &type) :
 QLayout(w->getQWidget()), w_(w), type_(type)
{
}

CTkAppLayout::
CTkAppLayout(const Type &type) :
 QLayout(nullptr), type_(type)
{
}
