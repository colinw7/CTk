#include <CTkAppEntry.h>

#include <CQUtil.h>
#include <CQStrParse.h>

class CTkAppEntryVarProc : public CTclTraceProc {
 public:
  CTkAppEntryVarProc(CTkApp *tk, CTkAppEntry *entry) :
   CTclTraceProc(tk), entry_(entry) {
  }

  void handleWrite(const char *) override {
    if (entry_)
      entry_->updateFromVar();
  }

 private:
  using EntryP = QPointer<CTkAppEntry>;

  EntryP entry_;
};

//---

class CTkAppEntryValidator : public QValidator {
 public:
  CTkAppEntryValidator(CTkApp *tk, CTkAppEntry *entry) :
   QValidator(entry), tk_(tk), entry_(entry) {
  }

  bool isEnabled() const { return enabled_; }
  void setEnabled(bool b) { enabled_ = b; }

  State validate(QString &input, int & /*pos*/) const override {
    if (! enabled_)
      return Acceptable;

    if (entry_->validate(input))
      return Acceptable;

    return Invalid;
  }

 private:
  using EntryP = QPointer<CTkAppEntry>;

  CTkApp* tk_ { nullptr };
  EntryP  entry_;
  bool    enabled_ { false };
};

//---

CTkAppEntry::
CTkAppEntry(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qedit_ = new CTkAppEntryWidget(this);

  qedit_->setFocusPolicy(Qt::StrongFocus);

  setQWidget(qedit_);

  connectSlots(true);
}

void
CTkAppEntry::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qedit_,
    SIGNAL(editingFinished()), this, SLOT(valueChangedSlot()));
}

bool
CTkAppEntry::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-exportselection") {
    bool b;
    if (! tk_->variantToBool(var, b))
      return tk_->invalidBool(var);

    setExportSelection(b);
  }
  else if (name == "-invalidcommand") {
    auto value = tk_->variantToString(var);

    setInvalidCommand(value);
  }
  else if (name == "-placeholder") {
    auto value = tk_->variantToString(var);

    setPlaceHolder(value);
  }
  else if (name == "-placeholderforeground") {
    QColor c;
    if (! tk_->variantToQColor(var, c))
      return tk_->invalidQColor(var);

    setPlaceHolderForeground(c);
  }
  else if (name == "-readonlybackground") {
    QColor c;
    if (! tk_->variantToQColor(var, c))
      return tk_->invalidQColor(var);

    setReadOnlyBackground(c);
  }
  else if (name == "-show") {
    qedit_->setEchoMode(QLineEdit::Password);
  }
  else if (name == "-state") {
    auto value = tk_->variantToString(var);

    if (value == "readonly")
      qedit_->setReadOnly(true);
    else {
      if (! setWidgetStateFromString(value))
        return tk_->throwError(tk_->msg() + "bad state \"" + value + "\": must be "
                               "disabled, normal, or readonly");
    }
  }
  else if (name == "-textvariable") {
    auto value = tk_->variantToString(var);

    setVarName(value);

    if (! tk_->hasGlobalVar(varName()))
      tk_->setStringGlobalVar(varName(), qedit_->text());
    else
      setText(tk_->getStringGlobalVar(varName()));

    varProc_ = new CTkAppEntryVarProc(tk_, this);

    tk_->traceGlobalVar(varName(), varProc_);
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
      validateProc_ = new CTkAppEntryValidator(tk_, this);

      qedit_->setValidator(validateProc_);
    }
  }
  else if (name == "-validatecommand") {
    auto value = tk_->variantToString(var);

    validateCmd_ = value;
  }
  else if (name == "-width") {
    long w;
    if (! tk_->variantToInt(var, w))
      return tk_->invalidInteger(var);

    qedit_->setWidth(w);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppEntry::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  //---

  auto stringToIndex = [&](const QVariant &var, int &ind) {
    auto str = tk_->variantToString(var);

    CQStrParse parse(str);

    parse.skipSpace();

    if      (parse.isString("anchor")) {
      // TODO: cursor pos in selection ?
      ind = qedit_->selectionStart();
    }
    else if (parse.isString("end")) {
      ind = qedit_->text().length() - 1;
    }
    else if (parse.isString("insert")) {
      ind = qedit_->cursorPosition();
    }
    else if (parse.isString("sel.first")) {
      ind = qedit_->selectionStart();
    }
    else if (parse.isString("sel.last")) {
      ind = qedit_->selectionEnd();
    }
    else if (parse.isChar('@')) {
      parse.skipChar();

      int x;
      if (! parse.readInteger(&x))
        return false;

      ind = 0; // TODO
    }
    else {
      if (! parse.readInteger(&ind))
        return false;
    }

    return true;
  };

  //---

  static auto optionNames = std::vector<QString>({
    "bbox", "cget", "configure", "delete", "get", "icursor", "index",
    "insert", "scan", "selection", "validate", "xview"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "bbox") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " bbox index");

    int ind;
    if (! stringToIndex(args[1], ind))
      return tk_->throwError(tk_->msg() + "bad entry index \"" + args[1] + "\"");

    tk_->setStringResult("0 0 4 4");
  }
  else if (option == "delete") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " delete firstIndex ?lastIndex?");

    int startIndex, endIndex;
    if (! stringToIndex(args[1], startIndex))
      return tk_->throwError(tk_->msg() + "bad entry index \"" + args[1] + "\"");

    if (numArgs == 3) {
      if (! stringToIndex(args[2], endIndex))
        return tk_->throwError(tk_->msg() + "bad entry index \"" + args[2] + "\"");
    }
    else
      endIndex = startIndex;

    qedit_->setSelection(startIndex, endIndex - startIndex);

    qedit_->del();

    if (isExportSelection())
      tk_->setClipboardText(qedit_->selectedText(), CTkAppClipboard::Selection);
  }
  else if (option == "get") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getFullName() + " get");

    tk_->setStringResult(qedit_->text());
  }
  else if (option == "icursor") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " icursor pos");

    int ind;
    if (! stringToIndex(args[1], ind))
      return tk_->throwError(tk_->msg() + "bad entry index \"" + args[1] + "\"");

    qedit_->setCursorPosition(ind);
  }
  else if (option == "index") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " index string");

    int ind;
    if (! stringToIndex(args[1], ind))
      return tk_->throwError(tk_->msg() + "bad entry index \"" + args[1] + "\"");

    tk_->setIntegerResult(ind);
  }
  else if (option == "insert") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " insert index text");

    int ind;
    if (! stringToIndex(args[1], ind))
      return tk_->throwError(tk_->msg() + "bad entry index \"" + args[1] + "\"");

    qedit_->setCursorPosition(ind);

    qedit_->insert(tk_->variantToString(args[2]));
  }
  else if (option == "scan") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " scan mark|dragto x");

    static auto optionNames = std::vector<QString>({"dragto", "mark"});

    QString option;
    if (! tk_->lookupName("option", optionNames, args[1], option))
      return tk_->throwError(tk_->msg() + "bad scan option \"" + args[1] + "\": must be "
                             "dragto or mark");

    long x;
    if (! tk_->variantToInt(args[2], x))
      return tk_->invalidInteger(args[2]);

    tk_->TODO(args);
  }
  else if (option == "selection") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " selection option ?index?");

    static auto optionNames = std::vector<QString>(
      {"adjust", "clear", "from", "present", "range", "to"});

    QString option;
    if (! tk_->lookupName("option", optionNames, args[1], option))
      return tk_->throwError(tk_->msg() + "bad selection option \"" + args[1] + "\": must be "
                             "adjust, clear, from, present, range, or to");

    if      (option == "adjust") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " selection adjust index");

      int ind;
      if (! stringToIndex(args[2], ind))
        return tk_->throwError(tk_->msg() + "bad entry index \"" + args[2] + "\"");

      int start = qedit_->selectionStart();
      int len   = qedit_->selectionLength();

      if (start >= 0) {
        int end = start + len;

        if      (ind < start) start = ind;
        else if (ind > end  ) end   = ind;

        qedit_->setSelection(start, end - start);

        if (isExportSelection())
          tk_->setClipboardText(qedit_->selectedText(), CTkAppClipboard::Selection);
      }
    }
    else if (option == "clear") {
      if (numArgs != 2)
        return tk_->wrongNumArgs(getFullName() + " selection clear");

      qedit_->deselect();
    }
    else if (option == "from") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " selection from index");

      int ind;
      if (! stringToIndex(args[2], ind))
        return tk_->throwError(tk_->msg() + "bad entry index \"" + args[2] + "\"");

      qedit_->setSelection(ind, ind);
      qedit_->setCursorPosition(ind);

      if (isExportSelection())
        tk_->setClipboardText(qedit_->selectedText(), CTkAppClipboard::Selection);
    }
    else if (option == "present") {
      int start = qedit_->selectionStart();
      int end   = qedit_->selectionEnd  ();

      auto ind = (start >= 0 && end >= start ? 1 : 0);

      tk_->setIntegerResult(ind);
    }
    else if (option == "range") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " selection range start end");

      int startIndex, endIndex;
      if (! stringToIndex(args[2], startIndex))
        return tk_->throwError(tk_->msg() + "bad entry index \"" + args[2] + "\"");
      if (! stringToIndex(args[3], endIndex))
        return tk_->throwError(tk_->msg() + "bad entry index \"" + args[3] + "\"");

      qedit_->setSelection(startIndex, endIndex - startIndex);

      if (isExportSelection())
        tk_->setClipboardText(qedit_->selectedText(), CTkAppClipboard::Selection);
    }
    else if (option == "to") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " selection to index");

      int endIndex;
      if (! stringToIndex(args[2], endIndex))
        return tk_->throwError(tk_->msg() + "bad entry index \"" + args[2] + "\"");

      int startIndex = qedit_->cursorPosition();

      qedit_->setSelection(startIndex, endIndex - startIndex);

      if (isExportSelection())
        tk_->setClipboardText(qedit_->selectedText(), CTkAppClipboard::Selection);
    }
  }
  else if (option == "validate") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getFullName() + " validate");

    auto rc = validate(qedit_->text());

    tk_->setBoolResult(rc);
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

      int len = qedit_->text().length();
      int pos = std::min(std::max(int(len*f), 0), len);

      qedit_->setCursorPosition(pos);
    }
    else if (args[1] == "scroll") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " xview scroll number pages|units");

      double i;
      if (! tk_->variantToReal(args[2], i))
        return tk_->invalidReal(args[2]);

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
      if (! stringToIndex(args[1], ind))
        return tk_->throwError(tk_->msg() + "bad entry index \"" + args[1] + "\"");

      qedit_->setCursorPosition(ind);
    }
  }
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

void
CTkAppEntry::
setText(const QString &text)
{
  CTkAppWidget::setText(text);

  qedit_->setText(text);
}

void
CTkAppEntry::
valueChangedSlot()
{
  if (varName() != "") {
    if (varProc_)
      varProc_->setEnabled(false);

    if (! tk_->hasGlobalVar(varName()))
      tk_->setStringGlobalVar(varName(), QString());

    tk_->setStringGlobalVar(varName(), qedit_->text());

    if (varProc_)
      varProc_->setEnabled(true);
  }
}

bool
CTkAppEntry::
validate(const QString &s) const
{
  if (validateProc_)
    validateProc_->setEnabled(false);

  bool b = true;

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

    auto replaceStr = [&](const QString &from, const QString &to) {
      auto pos = cmd.indexOf(from);
      if (pos >= 0)
        cmd = cmd.mid(0, pos) + "{" + to + "}" + cmd.mid(pos + from.length());
    };

    replaceStr("%d", "-1");
    replaceStr("%i", "-1");
    replaceStr("%P", s);
    replaceStr("%s", s);
    replaceStr("%S", "");
    replaceStr("%v", "all");
    replaceStr("%V", "forced");
    replaceStr("%W", getFullName());

    if (! tk_->eval(cmd))
      return true;

    QVariant res;
    if (tk_->getResult(res)) {
      if (! tk_->variantToBool(res, b))
        return true;
    }
  }

  if (validateProc_)
    validateProc_->setEnabled(true);

  return b;
}

void
CTkAppEntry::
updateFromVar()
{
  if (varName() != "" && tk_->hasGlobalVar(varName()))
    setText(tk_->getStringGlobalVar(varName()));
}

//---

CTkAppEntryWidget::
CTkAppEntryWidget(CTkAppEntry *entry) :
 QLineEdit(entry->getParent() ? entry->getParent()->getQWidget() : nullptr), entry_(entry)
{
  setObjectName("qentry");
}

QSize
CTkAppEntryWidget::
sizeHint() const
{
  auto s = QLineEdit::sizeHint();

  QFontMetrics fm(font());

  if (width_ > 0)
    s.setWidth(width_*fm.horizontalAdvance("0"));

  return s;
}
