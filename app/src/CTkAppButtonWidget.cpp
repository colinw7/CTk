#include <CTkAppButtonWidget.h>
#include <CTkAppButton.h>

CTkAppButtonWidget::
CTkAppButtonWidget(CTkAppButton *button) :
 CQButtonImage(button->getParent() ? button->getParent()->getQWidget() : nullptr), button_(button)
{
  setObjectName("qbutton");
}

void
CTkAppButtonWidget::
updateText()
{
  if (wrapLength_ > 0) {
    auto text = CTkAppUtil::formatStringInWidth(text_, wrapLength_, font());

    CQButtonImage::setLabel(text);
  }
  else
    CQButtonImage::setLabel(text_);
}
