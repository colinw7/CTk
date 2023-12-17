#include <CTkAppButton.h>
#include <CTkAppButtonWidget.h>
#include <CTkAppCommands.h>
#include <CTkAppWidgetCommand.h>
#include <CTkAppImage.h>

#include <CQUtil.h>

class CTkAppButtonVarProc : public CTclTraceProc {
 public:
  CTkAppButtonVarProc(CTkApp *tk, CTkAppButton *button_) :
   CTclTraceProc(tk), button_(button_) {
  }

  void handleWrite(const char *) override {
    if (button_)
      button_->updateFromVar();
  }

  void handleUnset(const char *) override {
    if (button_)
      app_->setStringGlobalVar(button_->varName(), button_->getText());
  }

 private:
  using ButtonP = QPointer<CTkAppButton>;

  ButtonP button_;
};

//---

CTkAppButton::
CTkAppButton(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qbutton_ = new CTkAppButtonWidget(this);

  setQWidget(qbutton_);

  connectSlots(true);
}

CTkAppButton::
~CTkAppButton()
{
  if (varProc_)
    tk_->untraceGlobalVar(varName(), varProc_);
}

void
CTkAppButton::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qbutton_,
    SIGNAL(clicked()), this, SLOT(clickSlot()));
}

bool
CTkAppButton::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-command") {
    auto value = tk_->variantToString(var);

    setCommand(value);
  }
  else if (name == "-default") {
    CTkAppWidgetState state;
    if (! CTkAppUtil::variantToState(tk_, var, state))
      return tk_->throwError(tk_->msg() + "bad default \"" + var + "\": must be "
                             "active, disabled, or normal");

    if      (state == CTkAppWidgetState::ACTIVE  ) qbutton_->setDown(true);
    else if (state == CTkAppWidgetState::DISABLED) qbutton_->setEnabled(false);
    else if (state == CTkAppWidgetState::NORMAL  ) qbutton_->setEnabled(true);
  }
  else if (name == "-height") {
    // height of image (pixels) or text (chars)
    CTkAppDistance h;
    if (! tk_->variantToDistanceI(var, h))
      return tk_->invalidDistance(var);

    qbutton_->setUserHeight(h.ivalue);
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
  else if (name == "-state") {
    auto value = tk_->variantToString(var);

    if (value == "readonly")
      qbutton_->setReadOnly(true);
    else {
      if (! setWidgetStateFromString(value))
        return tk_->throwError(tk_->msg() + "bad state \"" + value + "\": must be "
                               "active, disabled, or normal");
    }
  }
  else if (name == "-underline") {
    long underlinePos = -1;
    if (! tk_->getOptionInt(name, var, underlinePos))
      return tk_->invalidInteger(var);
  }
  else if (name == "-textvariable") {
    auto value = tk_->variantToString(var);

    setVarName(value);

    if (! tk_->hasGlobalVar(varName()))
      tk_->setStringGlobalVar(varName(), getText());
    else
      updateFromVar();

    varProc_ = new CTkAppButtonVarProc(tk_, this);

    tk_->traceGlobalVar(varName(), varProc_);
  }
  else if (name == "-width") {
    // width of image (pixels) or text (chars)
    CTkAppDistance w;
    if (! tk_->variantToDistance(var, w))
      return tk_->invalidDistance(var);

    qbutton_->setUserWidth(w.rvalue);
  }
  else if (name == "-wraplength") {
    CTkAppDistance length;
    if (! tk_->variantToDistance(var, length))
      return tk_->invalidDistance(var);

    qbutton_->setWrapLength(length.rvalue);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

void
CTkAppButton::
updateFromVar()
{
  if (varName() != "" && tk_->hasGlobalVar(varName())) {
    setText(tk_->getStringGlobalVar(varName()));
  }
}

bool
CTkAppButton::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  static auto optionNames = std::vector<QString>({
    "cget", "configure", "flash", "invoke"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "flash") {
    if (numArgs == 1)
      flash();
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
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

void
CTkAppButton::
setText(const QString &text)
{
  CTkAppWidget::setText(text);

  qbutton_->setText(text);

  widgetCommand_->setNameValue("-text", text);
}

void
CTkAppButton::
setImageRef(const CTkAppImageRef &image)
{
  CTkAppWidget::setImageRef(image);

  if (image)
    qbutton_->setImage(image->getQImage());
  else
    qbutton_->setImage(QImage());
}

void
CTkAppButton::
setJustify(const Qt::Alignment &a)
{
  CTkAppWidget::setAnchor(a);

  qbutton_->setJustify(a);
}

void
CTkAppButton::
clickSlot()
{
  runCommand();
}

void
CTkAppButton::
flash()
{
  tk_->TODO();
}

void
CTkAppButton::
setPadX(int i)
{
  CTkAppWidget::setPadX(i);

  qbutton_->setBorderX(i);
}

void
CTkAppButton::
setPadY(int i)
{
  CTkAppWidget::setPadY(i);

  qbutton_->setBorderY(i);
}

void
CTkAppButton::
appearanceChanged()
{
  bool autoRaise = false;

  if (relief() == Relief::FLAT && overRaised_)
    autoRaise = true;

  qbutton_->setAutoRaise(autoRaise);
}
