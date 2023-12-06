#include <CTkAppFrame.h>
#include <CTkAppLayout.h>

CTkAppFrame::
CTkAppFrame(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qframe_ = new CTkAppFrameWidget(this);

  setQWidget(qframe_);
}

bool
CTkAppFrame::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-backgroundimage") {
    auto image = tk_->getImage(var);
    if (! image) return false;

    setBackgroundImage(image);
  }
  else if (name == "-class") {
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
      return tk_->throwError(tk_->msg() + "expected boolean value but got \"" + var + "\"");

    setContainer(b);
  }
  else if (name == "-tile") {
    bool b;
    if (! tk_->variantToBool(var, b))
      return tk_->throwError(tk_->msg() + "expected boolean value but got \"" + var + "\"");

    setTile(b);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppFrame::
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

//---

CTkAppFrameWidget::
CTkAppFrameWidget(CTkAppFrame *frame) :
 QFrame(frame->getParent() ? frame->getParent()->getQWidget() : nullptr), frame_(frame)
{
  setObjectName("frame");
}

QSize
CTkAppFrameWidget::
sizeHint() const
{
  auto *layout = qobject_cast<CTkAppLayout *>(this->layout());

  auto s = (layout ? layout->sizeHint() : QFrame::sizeHint());

  return s;
}
