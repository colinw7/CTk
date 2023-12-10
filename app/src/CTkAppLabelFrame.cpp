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
    if (! isInCreate())
      return tk_->throwError("can't modify -class option after widget is created");

    auto value = tk_->variantToString(var);

    setOptionClass(value);
  }
  else if (name == "-container") {
    if (! isInCreate())
      return tk_->throwError("can't modify -container option after widget is created");

    bool b;
    if (! tk_->variantToBool(var, b))
      return tk_->invalidBool(var);

    setContainer(b);
  }
  else if (name == "-labelanchor") {
    Qt::Alignment align;
    if (! tk_->variantToAlign(var, align))
      return tk_->throwError(tk_->msg() + "bad anchor \"" + var + "\": must be "
                             "n, ne, e, se, s, sw, w, nw, or center");

    setLabelAnchor(align);
  }
  else if (name == "-labelwidget") {
    auto *w = tk_->lookupWidgetByName(var, /*quiet*/true);
    if (! w)
      return tk_->throwError(tk_->msg() + "bad window path name \"" + var + "\"");

    setLabelWidget(w);
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
