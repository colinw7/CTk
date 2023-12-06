#include <CTkAppCheckButton.h>

#include <CQUtil.h>

class CTkAppCheckButtonVarProc : public CTclTraceProc {
 public:
  CTkAppCheckButtonVarProc(CTkApp *tk, CTkAppCheckButton *check) :
   CTclTraceProc(tk), check_(check) {
  }

  void handleWrite(const char *) override {
    if (check_)
      check_->updateFromVar();
  }

 private:
  using CheckButtonP = QPointer<CTkAppCheckButton>;

  CheckButtonP check_;
};

//---

CTkAppCheckButton::
CTkAppCheckButton(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qcheck_ = new CTkAppCheckButtonWidget(this);

  setQWidget(qcheck_);

  connectSlots(true);
}

void
CTkAppCheckButton::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qcheck_,
    SIGNAL(stateChanged(int)), this, SLOT(stateChangedSlot(int)));
}

bool
CTkAppCheckButton::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-command") {
    auto value = tk_->variantToString(var);

    setCommand(value);
  }
#if 0
  else if (name == "-height") {
    // height of image (pixels) or text (chars)
    CTkAppDistance h;
    if (! tk_->variantToDistance(var, h))
      return tk_->invalidDistance(var);

    qcheck_->setHeight(h.rvalue);
  }
#endif
  else if (name == "-indicatoron") {
    bool b;
    if (! tk_->variantToBool(var, b))
      return tk_->throwError(tk_->msg() + "expected boolean value but got \"" + var + "\"");

    setShowIndicator(b);
  }
  else if (name == "-offrelief") {
    Relief relief { Relief::NONE };
    if (! CTkAppUtil::variantToRelief(tk_, var, relief))
      return tk_->throwError(tk_->msg() + "bad relief \"" + var + "\": must be "
                             "flat, groove, raised, ridge, solid, or sunken");

    tk_->TODO(name);
  }
  else if (name == "-offvalue") {
    auto value = tk_->variantToString(var);

    setOffValue(value);

    updateToVar();
  }
  else if (name == "-onrelief") {
    Relief relief { Relief::NONE };
    if (! CTkAppUtil::variantToRelief(tk_, var, relief))
      return tk_->throwError(tk_->msg() + "bad relief \"" + var + "\": must be "
                             "flat, groove, raised, ridge, solid, or sunken");

    tk_->TODO(name);
  }
  else if (name == "-onvalue") {
    auto value = tk_->variantToString(var);

    setOnValue(value);

    updateToVar();
  }
  else if (name == "-overrelief") {
    Relief relief { Relief::NONE };
    if (tk_->variantIsValid(var)) {
      if (! CTkAppUtil::variantToRelief(tk_, var, relief))
        return tk_->throwError(tk_->msg() + "bad relief \"" + var + "\": must be "
                               "flat, groove, raised, ridge, solid, or sunken");
    }

    setOverRaised(relief == Relief::RAISED);
    appearanceChanged();
  }
  else if (name == "-selectcolor") {
    QColor c;
    if (tk_->variantIsValid(var)) {
      if (! CTkAppUtil::variantToQColor(tk_, var, c))
        return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");
    }

    setSelectColor(c);
  }
  else if (name == "-selectimage") {
    auto value = tk_->variantToString(var);

    auto appImage = tk_->getImage(value);
    if (! appImage)
      return tk_->throwError(tk_->msg() + "image \"" + var + "\" doesn't exist");

    tk_->TODO(name);
  }
  else if (name == "-state") {
    auto value = tk_->variantToString(var);

    if (value == "readonly")
      qcheck_->setReadOnly(true);
    else {
      if (! setWidgetStateFromString(value))
        return tk_->throwError(tk_->msg() + "bad state \"" + value + "\": must be "
                               "active, disabled, or normal");
    }
  }
  else if (name == "-textvariable") {
    tk_->TODO(name);
  }
  else if (name == "-tristateimage") {
    auto value = tk_->variantToString(var);

    auto appImage = tk_->getImage(value);
    if (! appImage)
      return tk_->throwError(tk_->msg() + "image \"" + var + "\" doesn't exist");

    tk_->TODO(name);
  }
  else if (name == "-tristatevalue") {
    tk_->TODO(name);
  }
  else if (name == "-variable") {
    auto value = tk_->variantToString(var);

    setVarName(value);

    if (! tk_->hasGlobalVar(varName())) {
      tk_->setBoolGlobalVar(varName(), false);

      updateToVar();
    }
    else
      updateFromVar();

    varProc_ = new CTkAppCheckButtonVarProc(tk_, this);

    tk_->traceGlobalVar(varName(), varProc_);
  }
#if 0
  else if (name == "-width") {
    // width of image (pixels) or text (chars)
    CTkAppDistance w;
    if (! tk_->variantToDistance(var, w))
      return tk_->invalidDistance(var);

    qcheck_->setWidth(w.rvalue);
  }
#endif
  else if (name == "-value") {
    bool b;
    if (! tk_->variantToBool(var, b))
      return tk_->throwError(tk_->msg() + "expected boolean value but got \"" + var + "\"");

    setChecked(b);
  }
  else if (name == "-tristatevalue") {
    tk_->TODO(name);
  }
  else if (name == "-underline") {
    long pos;
    if (! tk_->getOptionInt(name, var, pos))
      return tk_->invalidInteger(var);

    setUnderlinePos(pos);
  }
  else if (name == "-wraplength") {
    CTkAppDistance length;
    if (! tk_->variantToDistance(var, length))
      return tk_->invalidDistance(var);

    qcheck_->setWrapLength(length.rvalue);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppCheckButton::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  static auto optionNames = std::vector<QString>({
    "cget", "configure", "deselect", "flash", "invoke", "select", "toggle"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "deselect") {
    if (numArgs == 1)
      setChecked(false);
    else
      return tk_->wrongNumArgs(getFullName() + " deselect");
  }
  else if (option == "flash") {
    if (numArgs == 1)
      tk_->TODO(args);
    else
      return tk_->wrongNumArgs(getFullName() + " flash");
  }
  else if (option == "invoke") {
    if (numArgs == 1) {
      if (getQWidget()->isEnabled())
        runCommand();
    }
    else
      return tk_->wrongNumArgs(getFullName() + " invoke");
  }
  else if (option == "select") {
    if (numArgs == 1)
      setChecked(true);
    else
      return tk_->wrongNumArgs(getFullName() + " select");
  }
  else if (option == "toggle") {
    if (numArgs == 1)
      setChecked(! qcheck_->isChecked());
    else
      return tk_->wrongNumArgs(getFullName() + " toggle");
  }
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

void
CTkAppCheckButton::
setText(const QString &text)
{
  CTkAppWidget::setText(text);

  qcheck_->setLabel(text);
}

void
CTkAppCheckButton::
setChecked(bool b)
{
  if (b != qcheck_->isChecked()) {
    connectSlots(false);

    qcheck_->setChecked(b);

    updateToVar();

    connectSlots(true);
  }
}

void
CTkAppCheckButton::
stateChangedSlot(int)
{
  updateToVar();

  runCommand();
}

void
CTkAppCheckButton::
updateFromVar()
{
  if (varName() != "" && tk_->hasGlobalVar(varName())) {
    bool checked = false;

    if      (onValue() && tk_->getStringGlobalVar(varName()) == *onValue())
      checked = true;
    else if (offValue() && tk_->getStringGlobalVar(varName()) == *offValue())
      checked = true;
    else {
      bool checked1 = tk_->getBoolGlobalVar(varName());

      if ((checked1 && ! onValue()) || (! checked && ! offValue()))
        checked = checked1;
    }

    setChecked(checked);
  }
}

void
CTkAppCheckButton::
updateToVar()
{
  if (varName() != "") {
    if (varProc_)
      varProc_->setEnabled(false);

    if (qcheck_->isChecked()) {
      if (onValue())
        tk_->setStringGlobalVar(varName(), *onValue());
      else
        tk_->setBoolGlobalVar(varName(), true);
    }
    else {
      if (offValue())
        tk_->setStringGlobalVar(varName(), *offValue());
      else
        tk_->setBoolGlobalVar(varName(), false);
    }

    if (varProc_)
      varProc_->setEnabled(true);
  }
}

//---

CTkAppCheckButtonWidget::
CTkAppCheckButtonWidget(CTkAppCheckButton *check) :
 QCheckBox(check->getParent() ? check->getParent()->getQWidget() : nullptr)
{
  setObjectName("qcheck");
}

void
CTkAppCheckButtonWidget::
updateText()
{
  if (wrapLength_ > 0) {
    auto text = CTkAppUtil::formatStringInWidth(text_, wrapLength_, font());

    QCheckBox::setText(text);
  }
  else
    QCheckBox::setText(text_);
}
