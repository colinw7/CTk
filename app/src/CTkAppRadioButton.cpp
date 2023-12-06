#include <CTkAppRadioButton.h>
#include <CTkAppImage.h>

#include <CQUtil.h>

#include <QPainter>

class CTkAppRadioButtonVarProc : public CTclTraceProc {
 public:
  CTkAppRadioButtonVarProc(CTkApp *tk, CTkAppRadioButton *radio) :
   CTclTraceProc(tk), radio_(radio) {
  }

  void handleWrite(const char *) override {
    if (radio_)
      radio_->updateFromVar();
  }

 private:
  using RadioButtonP = QPointer<CTkAppRadioButton>;

  RadioButtonP radio_;
};

//---

CTkAppRadioButton::RadioGroups CTkAppRadioButton::s_radioGroups;

CTkAppRadioButton::
CTkAppRadioButton(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qradio_ = new CTkAppRadioButtonWidget(this);

  setQWidget(qradio_);

  connectSlots(true);
}

CTkAppRadioButton::
~CTkAppRadioButton()
{
  if (varProc_)
    tk_->untraceGlobalVar(varName(), varProc_);

  if (varName_ != "")
    removeRadioGroup(this, varName());
}

void
CTkAppRadioButton::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qradio_,
    SIGNAL(toggled(bool)), this, SLOT(toggleSlot(bool)));
}

bool
CTkAppRadioButton::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-command") {
    auto value = tk_->variantToString(var);

    setCommand(value);
  }
  else if (name == "-height") {
    // height of image (pixels) or text (chars)
    CTkAppDistance h;
    if (! tk_->variantToDistanceI(var, h))
      return tk_->invalidDistance(var);

    setUserHeight(h.ivalue);
  }
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
  else if (name == "-onrelief") {
    Relief relief { Relief::NONE };
    if (! CTkAppUtil::variantToRelief(tk_, var, relief))
      return tk_->throwError(tk_->msg() + "bad relief \"" + var + "\": must be "
                             "flat, groove, raised, ridge, solid, or sunken");

    tk_->TODO(name);
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
      qradio_->setReadOnly(true);
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
    auto value = tk_->variantToString(var);

    setTriStateValue(value);
  }
  else if (name == "-underline") {
    long pos;
    if (! tk_->getOptionInt(name, var, pos))
      return tk_->invalidInteger(var);
  }
  else if (name == "-value") {
    auto value = tk_->variantToString(var);

    setValue(value);
  }
  else if (name == "-variable") {
    auto value = tk_->variantToString(var);

    setVarName(value);
  }
  else if (name == "-width") {
    // width of image (pixels) or text (chars)
    CTkAppDistance w;
    if (! tk_->variantToDistance(var, w))
      return tk_->invalidDistance(var);

    setUserWidth(w.rvalue);
  }
  else if (name == "-wraplength") {
    CTkAppDistance length;
    if (! tk_->variantToDistance(var, length))
      return tk_->invalidDistance(var);

    qradio_->setWrapLength(length.rvalue);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppRadioButton::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  static auto optionNames = std::vector<QString>({
    "cget", "configure", "deselect", "flash", "invoke", "select"});

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
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

void
CTkAppRadioButton::
setText(const QString &text)
{
  CTkAppWidget::setText(text);

  qradio_->setText(text);
}

void
CTkAppRadioButton::
setImageRef(const CTkAppImageRef &image)
{
  CTkAppWidget::setImageRef(image);

  auto qimage = image->getQImage();

  qradio_->setImage(qimage);
}

void
CTkAppRadioButton::
setValue(const QString &value)
{
  value_ = value;

  updateToVar();
}

void
CTkAppRadioButton::
setVarName(const QString &varName)
{
  if (varName_ != "")
    removeRadioGroup(this, varName_);

  varName_ = varName;

  if (varName_ != "")
    addRadioGroup(this, varName_);

  //---

  updateToVar();

  delete varProc_;

  if (varName_ != "") {
    varProc_ = new CTkAppRadioButtonVarProc(tk_, this);

    tk_->traceGlobalVar(varName_, varProc_);
  }
  else
    varProc_ = nullptr;
}

void
CTkAppRadioButton::
addRadioGroup(CTkAppRadioButton *radio, const QString &varName)
{
  auto pg = s_radioGroups.find(varName);

  if (pg == s_radioGroups.end())
    pg = s_radioGroups.insert(pg, RadioGroups::value_type(varName, RadioGroupData()));

  auto &radioGroupData = (*pg).second;

  if (! radioGroupData.group)
    radioGroupData.group = new QButtonGroup;

  radioGroupData.buttons.insert(radio);

  radioGroupData.group->addButton(radio->qradio());
}

void
CTkAppRadioButton::
removeRadioGroup(CTkAppRadioButton *radio, const QString &varName)
{
  auto pg = s_radioGroups.find(varName);
  if (pg == s_radioGroups.end()) return;

  auto &radioGroupData = (*pg).second;

  radioGroupData.buttons.erase(radio);

  if (radioGroupData.buttons.empty()) {
    delete radioGroupData.group;

    s_radioGroups.erase(varName);
  }
}

void
CTkAppRadioButton::
setChecked(bool b)
{
  if (b != qradio_->isChecked()) {
    connectSlots(false);

    qradio_->setChecked(b);

    toggleSlot(b);

    connectSlots(true);
  }
}

void
CTkAppRadioButton::
toggleSlot(bool checked)
{
  updateToVar(checked);

  runCommand();
}

void
CTkAppRadioButton::
updateFromVar()
{
  if (varName() != "") {
    QVariant var;

    if (tk_->hasGlobalVar(varName()))
      var = tk_->getGlobalVar(varName());

    bool checked  = qradio_->isChecked();
    bool checked1 = (tk_->variantToString(var) == value_);

    if (checked != checked1)
      setChecked(checked1);
  }
}

void
CTkAppRadioButton::
updateToVar()
{
  updateToVar(qradio_->isChecked());
}

void
CTkAppRadioButton::
updateToVar(bool checked)
{
  if (varName() != "") {
    if (varProc_)
      varProc_->setEnabled(false);

    if (checked)
      tk_->setStringGlobalVar(varName(), value_);
    else
      tk_->setStringGlobalVar(varName(), "");

    if (varProc_)
      varProc_->setEnabled(true);
  }
}

//---

CTkAppRadioButtonWidget::
CTkAppRadioButtonWidget(CTkAppRadioButton *radio) :
 QRadioButton(radio->getParent() ? radio->getParent()->getQWidget() : nullptr), radio_(radio)
{
  setObjectName("qradio");
}

void
CTkAppRadioButtonWidget::
updateText()
{
  if (wrapLength_ > 0) {
    auto text = CTkAppUtil::formatStringInWidth(text_, wrapLength_, font());

    QRadioButton::setText(text);
  }
  else
    QRadioButton::setText(text_);
}

void
CTkAppRadioButtonWidget::
paintEvent(QPaintEvent *e)
{
  QRadioButton::paintEvent(e);

  auto s = QRadioButton::sizeHint();

  if (! image_.isNull()) {
    QPainter painter(this);

    auto ih = image_.height();

    painter.drawImage(s.width(), (height() - ih)/2, image_);
  }

}

QSize
CTkAppRadioButtonWidget::
sizeHint() const
{
  auto s = QRadioButton::sizeHint();

  if (! image_.isNull()) {
    auto iw = image_.width();
    auto ih = image_.height();

    return QSize(s.width() + iw + 2, std::max(s.height(), ih));
  }
  else {
    QFontMetrics fm(font());

    if (radio_->userWidth() > 0)
      s.setWidth(radio_->userWidth()*fm.horizontalAdvance("0"));

    if (radio_->userHeight() > 0)
      s.setHeight(radio_->userHeight()*fm.height());

    return s;
  }
}
