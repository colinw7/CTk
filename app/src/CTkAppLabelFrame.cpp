#include <CTkAppLabelFrame.h>

#include <QGroupBox>

CTkAppLabelFrame::
CTkAppLabelFrame(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qframe_ = new QGroupBox(parent ? parent->getQWidget() : nullptr);
  qframe_->setObjectName("frame");

  setQWidget(qframe_);
}

bool
CTkAppLabelFrame::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-class") {
    tk_->TODO(name);
  }
  else if (name == "-labelanchor") {
    tk_->TODO(name);
  }
  else if (name == "-labelwidget") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;

}

bool
CTkAppLabelFrame::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  static auto optionNames = std::vector<QString>({ "cget", "configure" });

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  return CTkAppWidget::execDefaultOp(option, args);
}

void
CTkAppLabelFrame::
setText(const QString &text)
{
  CTkAppWidget::setText(text);

  qframe_->setTitle(text);
}
