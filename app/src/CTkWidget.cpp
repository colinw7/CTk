#include <CTkWidget.h>
#include <CTkApp.h>

CTkWidget::
CTkWidget(CTkApp *app, const QString &id) :
 QFrame(nullptr), app_(app), id_(id)
{
  setObjectName("tkWidget");

  app_->installToplevel(id, this);
}

void
CTkWidget::
loadFile(const QString &filename)
{
  app_->evalFile(filename);
}
