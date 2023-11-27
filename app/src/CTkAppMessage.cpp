#include <CTkAppMessage.h>

#include <QTextEdit>

class CTkAppMessageVarProc : public CTclTraceProc {
 public:
  CTkAppMessageVarProc(CTkApp *tk, CTkAppMessage *message) :
   CTclTraceProc(tk), message_(message) {
  }

  void handleWrite(const char *) override {
    if (message_)
      message_->updateFromVar();
  }

 private:
  using MessageP = QPointer<CTkAppMessage>;

  MessageP message_;
};

//---

CTkAppMessage::
CTkAppMessage(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qedit_ = new QTextEdit(parent ? parent->getQWidget() : nullptr);
  qedit_->setObjectName("edit");

  qedit_->setReadOnly(true);

  setQWidget(qedit_);
}

bool
CTkAppMessage::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-textvariable") {
    auto value = tk_->variantToString(var);

    setVarName(value);

    if (! isInitNotify() && ! tk_->hasGlobalVar(varName()))
      tk_->setStringGlobalVar(varName(), QString());

    if (tk_->hasGlobalVar(varName()))
      setText(tk_->getStringGlobalVar(varName()));

    varProc_ = new CTkAppMessageVarProc(tk_, this);

    tk_->traceGlobalVar(varName(), varProc_);
  }
  else if (name == "-aspect") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppMessage::
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
CTkAppMessage::
setText(const QString &text)
{
  CTkAppWidget::setText(text);

  qedit_->setText(text);
}

void
CTkAppMessage::
updateFromVar()
{
  if (varName() != "" && tk_->hasGlobalVar(varName()))
    setText(tk_->getStringGlobalVar(varName()));
}
