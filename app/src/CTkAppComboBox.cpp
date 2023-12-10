#include <CTkAppComboBox.h>

#include <CQUtil.h>

class CTkAppComboBoxVarProc : public CTclTraceProc {
 public:
  CTkAppComboBoxVarProc(CTkApp *tk, CTkAppComboBox *combo) :
   CTclTraceProc(tk), combo_(combo) {
  }

  void handleWrite(const char *) override {
    if (combo_)
      combo_->updateFromVar();
  }

 private:
  using ComboBoxP = QPointer<CTkAppComboBox>;

  ComboBoxP combo_;
};

//---

CTkAppComboBox::
CTkAppComboBox(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qcombo_ = new CTkAppComboBoxWidget(this);

  setQWidget(qcombo_);

  connectSlots(true);
}

void
CTkAppComboBox::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qcombo_,
    SIGNAL(currentIndexChanged(int)), this, SLOT(indexChangedSlot(int)));
}

bool
CTkAppComboBox::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-exportselection") {
    bool b;
    if (! tk_->variantToBool(var, b))
      return tk_->invalidBool(var);

    setExportSelection(b);
  }
  else if (name == "-height") {
    tk_->TODO(name);
  }
  else if (name == "-invalidcommand") {
    auto value = tk_->variantToString(var);

    setInvalidCommand(value);
  }
  else if (name == "-postcommand") {
    tk_->TODO(name);
  }
  else if (name == "-readonlybackground") {
    QColor c;
    if (! tk_->variantToQColor(var, c))
      return tk_->invalidQColor(var);

    setReadOnlyBackground(c);
  }
  else if (name == "-state") {
    auto value = tk_->variantToString(var);

    if (value == "readonly")
      qcombo_->setReadOnly(true);
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

    varProc_ = new CTkAppComboBoxVarProc(tk_, this);

    tk_->traceGlobalVar(varName(), varProc_);
  }
  else if (name == "-values") {
    auto value = tk_->variantToString(var);

    std::vector<QString> strs;
    if (! tk_->splitList(value, strs))
      return tk_->throwError("Invalid values \"" + value + "\"");

    QStringList qstrs;

    for (const auto &str : strs)
      qstrs << str;

    qcombo_->clear();
    qcombo_->addItems(qstrs);
  }
  else if (name == "-width") {
    long w;
    if (! tk_->variantToInt(var, w))
      return tk_->throwError(tk_->msg() + "Invalid integer value \"" + var + "\"");
    qcombo_->setWidth(w);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppComboBox::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  //--

  auto stringToIndex = [&](const QVariant &var, int &ind, bool rangeCheck=true) {
    auto str = tk_->variantToString(var);

    long l = -1;

    if (str == "end")
      l = qcombo_->count() - 1;
    else {
      if (! CTkAppUtil::stringToInt(str, l))
        return tk_->throwError("bad integer \"" + str + "\"");
    }

    if (l < 0 || l >= qcombo_->count()) {
      if (rangeCheck)
        return tk_->throwError("Index " + QString::number(l) + "out of range");
    }

    ind = l;

    return true;
  };

  auto stringToTextIndex = [&](const QVariant &var, int &ind) {
    auto str = tk_->variantToString(var);
    if (! CTkAppUtil::stringToIndex(tk_, var, ind))
      return false;
    if (ind == CTkAppUtil::END_INDEX)
      ind = qcombo_->lineEdit()->text().length() - 1;
    return true;
  };

  //--

  static auto optionNames = std::vector<QString>({
    "bbox", "cget", "configure", "current", "delete", "get", "icursor", "identify",
    "index", "insert", "instate", "selection", "set", "state", "style", "validate", "xview"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "bbox") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " bbox index");

    int i;
    if (! stringToIndex(args[1], i))
      return false;

    tk_->TODO(args);
  }
  else if (option == "current") {
    if (numArgs != 1 && numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " current ?newIndex?");

    if (numArgs == 1) {
      auto i = qcombo_->currentIndex();

      tk_->setIntegerResult(i);
    }
    else {
      int i;
      if (! stringToIndex(args[1], i))
        return false;
      qcombo_->setCurrentIndex(i);
    }
  }
  else if (option == "delete") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " delete firstIndex ?lastIndex?");

    int firstIndex;
    if (! stringToIndex(args[1], firstIndex, /*rangeCheck*/false))
      return false;

    int lastIndex = firstIndex;
    if (numArgs == 3) {
      if (! stringToIndex(args[2], lastIndex, /*rangeCheck*/false))
        return false;
    }

    std::set<int> inds;

    for (int i = firstIndex; i <= lastIndex; ++i) {
      if (i >= 0 && i < qcombo_->count())
        inds.insert(-i);
    }

    for (const auto &i : inds)
      qcombo_->removeItem(-i);
  }
  else if (option == "get") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getFullName() + " get");

    tk_->setStringResult(qcombo_->currentText());
  }
  else if (option == "icursor") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " icursor pos");

    tk_->TODO(args);
  }
  else if (option == "identify") {
    if (numArgs != 3 && numArgs != 4)
      return tk_->wrongNumArgs(getFullName() + " identify ?what? x y");

    tk_->TODO(args);
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

    qcombo_->insertItem(ind, tk_->variantToString(args[2]));
  }
  else if (option == "instate") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " instate state-spec ?script?");

    tk_->TODO(args);
  }
  else if (option == "selection") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " selection option ?arg ...?");

    if      (args[1] == "adjust") {
      tk_->TODO(option + " " + tk_->variantToString(args[1]));
    }
    else if (args[1] == "clear") {
      if (numArgs != 2)
        return tk_->wrongNumArgs(getFullName() + " selection clear");

      qcombo_->lineEdit()->deselect();
    }
    else if (args[1] == "from") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " selection from index");

      int ind;
      if (! stringToIndex(args[2], ind))
        return false;

      qcombo_->lineEdit()->setCursorPosition(ind);
    }
    else if (args[1] == "present") {
      tk_->TODO(option + " " + tk_->variantToString(args[1]));
    }
    else if (args[1] == "range") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " selection clear");

      int startIndex, endIndex;
      if (! stringToTextIndex(args[2], startIndex))
        return false;
      if (! stringToTextIndex(args[3], endIndex))
        return false;

      qcombo_->lineEdit()->setSelection(startIndex, endIndex - startIndex);
    }
    else if (args[1] == "to") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " selection to index");

      int endIndex;
      if (! stringToTextIndex(args[2], endIndex))
        return false;

      int startIndex = qcombo_->lineEdit()->cursorPosition();

      qcombo_->lineEdit()->setSelection(startIndex, endIndex - startIndex);
    }
    else
      return false;
  }
  else if (option == "set") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " set value");

    qcombo_->setEditText(tk_->variantToString(args[1]));
  }
  else if (option == "state") {
    if (numArgs > 2)
      return tk_->wrongNumArgs(getFullName() + " state state-spec");

    tk_->TODO(args);
  }
  else if (option == "style") {
    tk_->TODO(args);
  }
  else if (option == "validate") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getFullName() + " validate");

    tk_->TODO(args);
  }
  else if (option == "xview") {
    if      (numArgs == 1) {
      tk_->setStringResult("0.0 1.0");
    }
    else if (args[1] == "moveto") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " xview moveto fraction");

      double f;
      if (! tk_->variantToReal(args[2], f))
        return tk_->invalidReal(args[2]);

      int len = qcombo_->currentText().length();
      int pos = std::min(std::max(int(len*f), 0), len);

      qcombo_->lineEdit()->setCursorPosition(pos);
    }
    else if (args[1] == "scroll") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " xview scroll number what");

      long i;
      if (! tk_->variantToInt(args[2], i))
        return false;

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

      qcombo_->lineEdit()->setCursorPosition(ind);
    }
  }
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

void
CTkAppComboBox::
indexChangedSlot(int i)
{
  if (varName() != "") {
    if (varProc_)
      varProc_->setEnabled(false);

    tk_->setIntegerGlobalVar(varName(), i);

    if (varProc_)
      varProc_->setEnabled(true);
  }
}

void
CTkAppComboBox::
updateFromVar()
{
  if (varName() != "" && tk_->hasGlobalVar(varName())) {
    long i;
    if (tk_->getIntegerGlobalVar(varName(), i))
      qcombo_->setCurrentIndex(i);
  }
}

//---

CTkAppComboBoxWidget::
CTkAppComboBoxWidget(CTkAppComboBox *combo) :
 QComboBox(combo->getParent() ? combo->getParent()->getQWidget() : nullptr), combo_(combo)
{
  setObjectName("qwcombo");
}

QSize
CTkAppComboBoxWidget::
sizeHint() const
{
  auto s = QComboBox::sizeHint();

  QFontMetrics fm(font());

  if (width_ > 0)
    s.setWidth(width_*fm.horizontalAdvance("0"));

  return s;
}
