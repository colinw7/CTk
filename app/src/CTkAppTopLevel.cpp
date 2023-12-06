#include <CTkAppTopLevel.h>
#include <CTkAppX11.h>

CTkAppTopLevel::
CTkAppTopLevel(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qtoplevel_ = new CTkAppTopLevelWidget(this);

  qtoplevel_->setWindowTitle(name);

  setQWidget(qtoplevel_);

  tk_->addTopLevel(this);

  //---

  CTkAppX11::setWmClientMachine(qtoplevel_->winId(), "");
}

CTkAppTopLevel::
~CTkAppTopLevel()
{
  tk_->removeTopLevel(this);
}

void
CTkAppTopLevel::
setTopLevelWidget(CTkAppTopLevelWidget *qtoplevel)
{
  delete qtoplevel_;

  qtoplevel_ = qtoplevel;

  setQWidget(qtoplevel_);
}

bool
CTkAppTopLevel::
show()
{
  if (isIconWindow())
    return false;

  setNeedsShow(false);

  CTkAppWidget::show();

  qtoplevel_->resize(qtoplevel_->sizeHint());

  return true;
}

void
CTkAppTopLevel::
hide()
{
  setNeedsShow(false);

  CTkAppWidget::hide();
}

bool
CTkAppTopLevel::
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
      return tk_->throwError(tk_->msg() + "expected boolean value but got \"" + var + "\"");

    setContainer(b);
  }
  else if (name == "-menu") {
    auto value = tk_->variantToString(var);

    setMenuName(value);
  }
  else if (name == "-screen") {
    tk_->TODO(name);
  }
  else if (name == "-use") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppTopLevel::
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

CTkAppTopLevelWidget::
CTkAppTopLevelWidget(CTkAppTopLevel *toplevel) :
 QFrame(nullptr), toplevel_(toplevel)
{
  setObjectName("toplevel");
}

QSize
CTkAppTopLevelWidget::
sizeHint() const
{
  auto s = QFrame::sizeHint();

  return s;
}
