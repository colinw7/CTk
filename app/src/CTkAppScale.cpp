#include <CTkAppScale.h>
#include <CTkAppCommands.h>

#include <CQSlider.h>
#include <CQUtil.h>

class CTkAppScaleVarProc : public CTclTraceProc {
 public:
  CTkAppScaleVarProc(CTkApp *tk, CTkAppScale *scale) :
   CTclTraceProc(tk), scale_(scale) {
  }

  void handleWrite(const char *) override {
    if (scale_)
      scale_->updateFromVar();
  }

  void handleUnset(const char *) override {
    if (scale_)
      app_->setRealGlobalVar(scale_->varName(), scale_->qscale()->value());
  }

 private:
  using ScaleP = QPointer<CTkAppScale>;

  ScaleP scale_;
};

//---

CTkAppScale::
CTkAppScale(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qscale_ = new CQRealSlider(parent ? parent->getQWidget() : nullptr);

  qscale_->setValuePosition(CQSliderBase::ValuePosition::NONE);

  setQWidget(qscale_);

  connectSlots(true);
}

void
CTkAppScale::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qscale_,
    SIGNAL(valueChanged(double)), this, SLOT(valueSlot(double)));
}

bool
CTkAppScale::
execConfig(const QString &name, const QVariant &var)
{
  // widget specific options
  if      (name == "-bigincrement") {
    double r;
    if (! tk_->variantToReal(var, r))
      return tk_->invalidReal(var);

    qscale_->setPageStep(r);
  }
  else if (name == "-command") {
    auto value = tk_->variantToString(var);

    setCommand(value);
  }
  else if (name == "-digits") {
    long n;
    if (! tk_->getOptionInt(name, var, n))
      return tk_->invalidInteger(var);

    qscale_->setPrecision(n);
  }
  else if (name == "-from") {
    double from;
    if (! tk_->variantToReal(var, from))
      return tk_->invalidReal(var);

    qscale_->setMinimum(from);
  }
  else if (name == "-label") {
    auto value = tk_->variantToString(var);

    qscale_->setLabel(value);
  }
  else if (name == "-length") {
    CTkAppDistance l;
    if (! tk_->variantToDistanceI(var, l))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    length_ = l.ivalue;

    updateSize();
  }
  else if (name == "-orient") {
    Qt::Orientation orient;
    if (! CTkAppUtil::variantToOrient(tk_, var, orient))
      return tk_->throwError(tk_->msg() + "bad orient \"" + var + "\": must be "
                             "horizontal or vertical");

    qscale_->setOrientation(orient);

    updateSize();
  }
  else if (name == "-resolution") {
    double r;

    if (! tk_->getOptionReal(name, var, r))
      return false;

    if (r > 0.0)
      qscale_->setSingleStep(r);
    else
      qscale_->setSingleStep(1.0);
  }
  else if (name == "-showvalue") {
    bool b;
    if (! tk_->variantToBool(var, b))
      return tk_->throwError(tk_->msg() + "expected boolean value but got \"" + var + "\"");

    if (b)
      qscale_->setValuePosition(CQSliderBase::ValuePosition::ABOVE);
    else
      qscale_->setValuePosition(CQSliderBase::ValuePosition::NONE);
  }
  else if (name == "-sliderlength") {
    CTkAppDistance w;
    if (! tk_->variantToDistanceI(var, w))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    tk_->TODO(name);
  }
  else if (name == "-sliderrelief") {
    Relief relief { Relief::NONE };
    if (! CTkAppUtil::variantToRelief(tk_, var, relief))
      return tk_->throwError(tk_->msg() + "bad relief \"" + var + "\": must be "
                             "flat, groove, raised, ridge, solid, or sunken");

    tk_->TODO(name);
  }
  else if (name == "-state") {
    auto value = tk_->variantToString(var);

    if (! setWidgetStateFromString(value))
      return tk_->throwError(tk_->msg() + "bad state \"" + value + "\": must be "
                             "active, disabled, or normal");

    widgetCommand_->setNameValue("-state", widgetStateToString(widgetState()));
  }
  else if (name == "-tickinterval") {
    double r;

    if (! tk_->getOptionReal(name, var, r))
      return false;

    if (r > 0.0)
      qscale_->setTickLabelDelta(r);
    else
      qscale_->setTickLabelDelta(0.0);
  }
  else if (name == "-to") {
    double to;
    if (! tk_->variantToReal(var, to))
      return tk_->invalidReal(var);

    qscale_->setMaximum(to);
  }
  else if (name == "-troughcolor") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");

    setTroughColor(c);
  }
  else if (name == "-variable") {
    auto value = tk_->variantToString(var);

    setVarName(value);

    if (! tk_->hasGlobalVar(varName())) {
      tk_->setRealGlobalVar(varName(), 0.0);

      updateToVar();
    }
    else
      updateFromVar();

    varProc_ = new CTkAppScaleVarProc(tk_, this);

    tk_->traceGlobalVar(varName(), varProc_);
  }
  else if (name == "-width") {
    CTkAppDistance w;
    if (! tk_->variantToDistanceI(var, w))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    width_ = w.ivalue;

    updateSize();
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppScale::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  static auto optionNames = std::vector<QString>({
    "cget", "configure", "coords", "get", "identify", "set"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "coords") {
    auto value = qscale_->value();

    if (numArgs == 1 || numArgs == 2) {
      if (numArgs == 2) {
        if (! tk_->variantToReal(args[1], value))
          return tk_->invalidReal(args[1]);
      }

      int pos = qscale_->valueToPos(value);

      if (qscale_->orientation() == Qt::Horizontal)
        tk_->setStringResult(QString("%1 0").arg(pos));
      else
        tk_->setStringResult(QString("0 %1").arg(pos));
    }
    else
      return tk_->wrongNumArgs(getFullName() + " coords ?value?");
  }
  else if (option == "get") {
    if      (numArgs == 1) {
      auto value = qscale_->value();
      tk_->setRealResult(value);
    }
    else if (numArgs == 3) {
      long x, y;
      if (! tk_->variantToInt(args[1], x))
        return tk_->invalidInteger(args[1]);
      if (! tk_->variantToInt(args[2], y))
        return tk_->invalidInteger(args[2]);

      if (qscale_->orientation() == Qt::Horizontal)
        tk_->setRealResult(qscale_->posToValue(x));
      else
        tk_->setRealResult(qscale_->posToValue(y));
    }
    else
      return tk_->wrongNumArgs(getFullName() + " get ?x y?");
  }
  else if (option == "identify") {
    if (numArgs == 3) {
      long x, y;
      if (! tk_->variantToInt(args[1], x))
        return tk_->invalidInteger(args[1]);
      if (! tk_->variantToInt(args[2], y))
        return tk_->invalidInteger(args[2]);

      tk_->setStringResult("");
    }
    else
      return tk_->wrongNumArgs(getFullName() + " identify x y");
  }
  else if (option == "set") {
    if (numArgs == 2) {
      double value;
      if (! tk_->variantToReal(args[1], value))
        return tk_->invalidReal(args[1]);

      qscale_->setValue(value);
    }
    else
      return tk_->wrongNumArgs(getFullName() + " set value");
  }
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

void
CTkAppScale::
updateSize()
{
  if (qscale_->orientation() == Qt::Horizontal) {
    setWidth(length_); setHeight(0);
  }
  else {
    setWidth(0); setHeight(length_);
  }
}

void
CTkAppScale::
valueSlot(double value)
{
  updateToVar();

  Args args;

  args.push_back(QString::number(value));

  runCommand(args);
}

void
CTkAppScale::
updateFromVar()
{
  if (varName() != "" && tk_->hasGlobalVar(varName())) {
    double r;
    if (tk_->getRealGlobalVar(varName(), r))
      qscale_->setValue(r);
    else
      tk_->throwError("can't assign non-numeric value to scale variable");
  }
}

void
CTkAppScale::
updateToVar()
{
  if (varName() != "") {
    if (varProc_)
      varProc_->setEnabled(false);

    tk_->setIntegerGlobalVar(varName(), qscale_->value());

    if (varProc_)
      varProc_->setEnabled(true);
  }
}
