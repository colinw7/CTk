#include <CTkAppSpinBox.h>
#include <CTkAppSpinBoxWidget.h>

#include <CQUtil.h>

class CTkAppSpinBoxVarProc : public CTclTraceProc {
 public:
  CTkAppSpinBoxVarProc(CTkApp *tk, CTkAppSpinBox *spin) :
   CTclTraceProc(tk), spin_(spin) {
  }

  void handleWrite(const char *) override {
    if (spin_)
      spin_->updateFromVar();
  }

 private:
  using SpinBoxP = QPointer<CTkAppSpinBox>;

  SpinBoxP spin_;
};

//---

class CTkAppSpinBoxValidator : public QValidator {
 public:
  CTkAppSpinBoxValidator(CTkApp *tk, CTkAppSpinBox *spin) :
   QValidator(spin), tk_(tk), spin_(spin) {
  }

  State validate(QString &input, int & /*pos*/) const override {
    if (spin_->validate(input))
      return Acceptable;

    return Invalid;
  }

 private:
  CTkApp*        tk_   { nullptr };
  CTkAppSpinBox* spin_ { nullptr };
};

//---

CTkAppSpinBox::
CTkAppSpinBox(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qspin_ = new CTkAppSpinBoxWidget(this);

  qspin_->setFocusPolicy(Qt::StrongFocus);

  setQWidget(qspin_);

  connectSlots(true);
}

void
CTkAppSpinBox::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qspin_,
    SIGNAL(editingFinished()), this, SLOT(valueChangedSlot()));
}

bool
CTkAppSpinBox::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-buttonbackground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");

    tk_->TODO(name);
  }
  else if (name == "-buttoncursor") {
    tk_->TODO(name);
  }
  else if (name == "-buttondownrelief") {
    Relief relief { Relief::NONE };
    if (! CTkAppUtil::variantToRelief(tk_, var, relief))
      return tk_->throwError(tk_->msg() + "bad relief \"" + var + "\": must be "
                             "flat, groove, raised, ridge, solid, or sunken");

    tk_->TODO(name);
  }
  else if (name == "-buttonuprelief") {
    Relief relief { Relief::NONE };
    if (! CTkAppUtil::variantToRelief(tk_, var, relief))
      return tk_->throwError(tk_->msg() + "bad relief \"" + var + "\": must be "
                             "flat, groove, raised, ridge, solid, or sunken");

    tk_->TODO(name);
  }
  else if (name == "-command") {
    auto value = tk_->variantToString(var);

    setCommand(value);
  }
  else if (name == "-disabledbackground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");

    tk_->TODO(name);
  }
  else if (name == "-disabledforeground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");
    tk_->TODO(name);
  }
  else if (name == "-format") {
    tk_->TODO(name);
  }
  else if (name == "-from") {
    long from;
    if (! tk_->getOptionInt(name, var, from))
      return tk_->invalidInteger(var);

    qspin_->setMinimum(from);
  }
  else if (name == "-invalidcommand") {
    auto value = tk_->variantToString(var);

    setInvalidCommand(value);
  }
  else if (name == "-increment") {
    long step;
    if (! tk_->getOptionInt(name, var, step))
      return tk_->invalidInteger(var);

    qspin_->setSingleStep(step);
  }
  else if (name == "-readonlybackground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");

    tk_->TODO(name);
  }
  else if (name == "-state") {
    auto value = tk_->variantToString(var);

    if (value == "readonly")
      qspin_->setReadOnly(true);
    else {
      if (! setWidgetStateFromString(value))
        return tk_->throwError(tk_->msg() + "bad state \"" + value + "\": must be "
                               "active, disabled, or normal");
    }
  }
  else if (name == "-textvariable") {
    auto value = tk_->variantToString(var);

    setVarName(value);

    if (! isInitNotify() && ! tk_->hasGlobalVar(varName()))
      tk_->setStringGlobalVar(varName(), QString());

    if (tk_->hasGlobalVar(varName()))
      setText(tk_->getStringGlobalVar(varName()));

    varProc_ = new CTkAppSpinBoxVarProc(tk_, this);

    tk_->traceGlobalVar(varName(), varProc_);
  }
  else if (name == "-to") {
    long to;
    if (! tk_->getOptionInt(name, var, to))
      return tk_->invalidInteger(var);

    qspin_->setMaximum(to);
  }
  else if (name == "-validate") {
    auto value = tk_->variantToString(var);

    static auto names =
      std::vector<QString>({"none", "focus", "focusin", "focusout", "key", "all"});

    QString value1;
    if (! tk_->lookupName("validate", names, value, value1))
      return tk_->throwError("Invalid validate mode '" + value + "'");

    if      (value1 == "none"    ) validateMode_ = ValidateMode::NONE;
    else if (value1 == "focus"   ) validateMode_ = ValidateMode::FOCUS;
    else if (value1 == "focusin" ) validateMode_ = ValidateMode::FOCUSIN;
    else if (value1 == "focusout") validateMode_ = ValidateMode::FOCUSOUT;
    else if (value1 == "key"     ) validateMode_ = ValidateMode::KEY;
    else if (value1 == "all"     ) validateMode_ = ValidateMode::ALL;

    if (! validateProc_) {
      validateProc_ = new CTkAppSpinBoxValidator(tk_, this);

      qspin_->setValidator(validateProc_);
    }
  }
  else if (name == "-validatecommand") {
    auto value = tk_->variantToString(var);

    validateCmd_ = value;
  }
  else if (name == "-values") {
    auto value = tk_->variantToString(var);

    std::vector<QString> strs;
    if (! tk_->splitList(value, strs))
      return tk_->throwError("Invalid values \"" + value + "\"");

    QStringList qstrs;

    for (const auto &str : strs)
      qstrs << str;

    qspin_->setStrings(qstrs);
  }
  else if (name == "-width") {
    long w;
    if (! tk_->variantToInt(var, w))
      return tk_->throwError(tk_->msg() + "bad integer \"" + var + "\"");
    qspin_->setWidth(w);
  }
  else if (name == "-wrap") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppSpinBox::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  //---

  auto stringToIndex = [&](const QVariant &var, int &ind) {
    if (! CTkAppUtil::stringToIndex(tk_, var, ind))
      return false;
    //if (ind == CTkAppUtil::END_INDEX)
    //  ind = qspin_->text().length() - 1;
    return true;
  };

  //---

  static auto optionNames = std::vector<QString>({
    "bbox", "cget", "configure", "delete", "get", "icursor", "identify", "index",
    "insert", "invoke", "scan", "selection", "set", "style", "validate", "xview"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "bbox") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " bbox index");

    int index;
    if (! stringToIndex(args[1], index))
      return false;
  }
  else if (option == "delete") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " delete firstIndex ?lastIndex?");

    int startIndex, endIndex;
    if (! stringToIndex(args[1], startIndex))
      return false;
    if (! stringToIndex(args[2], endIndex))
      return false;

    qspin_->deleteChars(startIndex, endIndex - startIndex);
  }
  else if (option == "get") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getFullName() + " get");

    tk_->setStringResult(qspin_->text());
  }
  else if (option == "icursor") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " icursor pos");

    tk_->TODO(args);
  }
  else if (option == "identify") {
    if (numArgs == 3) {
      long x, y;
      if (! tk_->variantToInt(args[1], x))
        return tk_->invalidInteger(args[1]);
      if (! tk_->variantToInt(args[2], y))
        return tk_->invalidInteger(args[2]);

      tk_->TODO(args);
    }
    else
      return tk_->wrongNumArgs(getFullName() + " identify x y");
  }
  else if (option == "index") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " index string");

    tk_->TODO(args);
  }
  else if (option == "insert") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " insert index text");

    int ind;
    if (! stringToIndex(args[1], ind))
      return false;

    qspin_->insertChars(ind, tk_->variantToString(args[2]));
  }
  else if (option == "instate") {
    tk_->TODO(args);
  }
  else if (option == "invoke") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " invoke elemName");

    tk_->TODO(args);
  }
  else if (option == "scan") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " scan mark|dragto x");

    tk_->TODO(args);
  }
  else if (option == "selection") {
    if (numArgs != 2 && numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " selection option ?index?");

    if      (args[1] == "adjust") {
      tk_->TODO(option + " " + tk_->variantToString(args[1]));
    }
    else if (args[1] == "clear") {
      if (numArgs != 2)
        return tk_->wrongNumArgs(getFullName() + " selection clear");

      qspin_->deselectAll();
    }
    else if (args[1] == "from") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " selection from index");

      int ind;
      if (! stringToIndex(args[2], ind))
        return false;

      qspin_->setCursorPos(ind);
    }
    else if (args[1] == "present") {
      tk_->TODO(option + " " + tk_->variantToString(args[1]));
    }
    else if (args[1] == "range") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " selection clear");

      int startIndex, endIndex;
      if (! stringToIndex(args[2], startIndex))
        return false;
      if (! stringToIndex(args[3], endIndex))
        return false;

      //qspin_->setSelection(startIndex, endIndex - startIndex);
    }
    else if (args[1] == "to") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " selection to index");

      int endIndex;
      if (! stringToIndex(args[2], endIndex))
        return false;

      int startIndex = qspin_->cursorPos();

      qspin_->selectChars(startIndex, endIndex - startIndex);
    }
    else
      return false;

    tk_->TODO(args);
  }
  else if (option == "set") {
    if (numArgs != 1 && numArgs != 1)
      return tk_->wrongNumArgs(getFullName() + " set ?string?");

    tk_->TODO(args);
  }
  else if (option == "validate") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getFullName() + " validate");

    tk_->TODO(args);
  }
  else if (option == "xview") {
    if (numArgs > 4)
      return tk_->wrongNumArgs(getFullName() + "xview ?moveto|scroll index?");

    if      (numArgs == 1) {
      tk_->setStringResult("0.0 1.0");
    }
    else if (args[1] == "moveto") {
      if (numArgs != 2)
        return tk_->wrongNumArgs(getFullName() + " xview moveto fraction");

      double f;
      if (! tk_->variantToReal(args[2], f))
        return tk_->invalidReal(args[2]);

      //int len = qspin_->text().length();
      //int pos = std::min(std::max(int(len*f), 0), len);

      //qspin_->setCursorPosition(pos);

      tk_->TODO(args);
    }
    else if (args[1] == "scroll") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " xview scroll number what");

      long i;
      if (! tk_->variantToInt(args[2], i))
        return tk_->invalidInteger(args[2]);

      static auto names = std::vector<QString>({"pages", "units"});

      QString name;
      if (! tk_->lookupName("what", names, args[3], name))
        return tk_->throwError(tk_->msg() + "bad argument \"" + args[3] +
                               "\": must be pages or units");

      int value = 1;
      if (name == "pages")
        value = i*1000;

      tk_->TODO("xview scroll " + QString::number(value));
      tk_->TODO(args);
    }
    else {
      if (numArgs != 2)
        return tk_->wrongNumArgs(getFullName() + " xview index");

      int ind;
      if (! stringToIndex(args[2], ind))
        return false;

      //qspin_->setCursorPosition(ind);

      tk_->TODO(args);
    }
  }
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

void
CTkAppSpinBox::
setText(const QString &)
{
  if (qspin_->mode() == CQSpinList::Mode::INTEGER) {
    long i;
    if (tk_->getIntegerGlobalVar(varName(), i))
      qspin_->setValue(i);
  }
  else
    qspin_->setText(tk_->getStringGlobalVar(varName()));
}

void
CTkAppSpinBox::
valueChangedSlot()
{
  if (varName() != "") {
    if (varProc_)
      varProc_->setEnabled(false);

    if (! tk_->hasGlobalVar(varName()))
      tk_->setStringGlobalVar(varName(), QString());

    //tk_->setStringGlobalVar(varName(), qedit_->text());

    if (varProc_)
      varProc_->setEnabled(true);
  }
}

bool
CTkAppSpinBox::
validate(const QString &s) const
{
  if (validateCmd_ != "") {
    // %d Type of action: 1 for insert, 0 for delete, or -1 for focus,
    //    forced or textvariable validation.
    // %i Index of char string to be inserted/deleted, if any, otherwise -1.
    // %P Value of the entry if the edit is allowed.
    //    If you are configuring the entry widget to have a new textvariable,
    //    this will be the value of that textvariable.
    // %s Current value of entry prior to editing.
    // %S Text string being inserted/deleted, if any, {} otherwise.
    // %v Type of validation currently set.
    // %V Type of validation that triggered the callback (key, focusin, focusout, forced).
    // %W Name of the entry widget.

    auto cmd = validateCmd_;

    auto pos = cmd.indexOf("%P");
    if (pos >= 0)
      cmd = cmd.mid(0, pos) + "{" + s + "}" + cmd.mid(pos + 2);

    if (! tk_->eval(cmd))
      return false;

    bool b = false;
    QVariant res;
    if (tk_->getResult(res)) {
      if (! tk_->variantToBool(res, b))
        return false;
    }
    return b;
  }

  return true;
}

void
CTkAppSpinBox::
updateFromVar()
{
  if (varName() != "" && tk_->hasGlobalVar(varName())) {
    long i;
    if (tk_->getIntegerGlobalVar(varName(), i))
      qspin_->setValue(i);
  }
}

//---

CTkAppSpinBoxWidget::
CTkAppSpinBoxWidget(CTkAppSpinBox *spin) :
 CQSpinList(spin->getParent() ? spin->getParent()->getQWidget() : nullptr), spin_(spin)
{
  setObjectName("qspin");
}

QSize
CTkAppSpinBoxWidget::
sizeHint() const
{
  auto s = CQSpinList::sizeHint();

  if (width_ > 0) {
    QFontMetrics fm(font());

    s.setWidth(width_*fm.horizontalAdvance("0"));
  }

  return s;
}
