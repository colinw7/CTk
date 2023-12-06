#include <CTkAppScrollBar.h>

#include <CQUtil.h>

CTkAppScrollBar::
CTkAppScrollBar(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qscrollbar_ = new CTkAppScrollBarWidget(this);

  setQWidget(qscrollbar_);

  connectSlots(true);
}

CTkAppScrollBar::
~CTkAppScrollBar()
{
}

void
CTkAppScrollBar::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qscrollbar_,
    SIGNAL(valueChanged(int)), this, SLOT(scrollValueSlot(int)));
  CQUtil::connectDisconnect(b, qscrollbar_,
    SIGNAL(actionTriggered(int)), this, SLOT(scrollActionSlot(int)));
}

bool
CTkAppScrollBar::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-activerelief") {
    Relief relief { Relief::NONE };
    if (! CTkAppUtil::variantToRelief(tk_, var, relief))
      return tk_->throwError(tk_->msg() + "bad relief \"" + var + "\": must be "
                             "flat, groove, raised, ridge, solid, or sunken");

    setActiveRelief(relief);
  }
  else if (name == "-command") {
    auto value = tk_->variantToString(var);

    setCommand(value);
  }
  else if (name == "-elementborderwidth") {
    CTkAppDistance w;
    if (! tk_->variantToDistance(var, w))
      return tk_->invalidDistance(var);

    setElementBorderWidth(w.rvalue);
  }
  else if (name == "-jump") {
    bool b;
    if (! tk_->variantToBool(var, b))
      return tk_->throwError(tk_->msg() + "expected boolean value but got \"" + var + "\"");

    setJump(b);
  }
  else if (name == "-orient") {
    Qt::Orientation orient;
    if (! CTkAppUtil::variantToOrient(tk_, var, orient))
      return tk_->throwError(tk_->msg() + "bad orientation \"" + var + "\": must be "
                             "vertical or horizontal");

    qscrollbar_->setOrientation(orient);
  }
  else if (name == "-troughcolor") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");

    qscrollbar_->setTroughColor(c);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppScrollBar::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  static auto optionNames = std::vector<QString>({
    "activate", "cget", "configure", "delta", "fraction", "get", "identify", "set" });

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "activate") {
    if      (numArgs == 1) {
      tk_->setStringResult(activeElement());
    }
    else if (numArgs == 2) {
      auto element = tk_->variantToString(args[1]);

      static auto names =
        std::vector<QString>({"arrow1", "slider", "arrow2"});

      QString element1;
      if (! tk_->lookupName("element", names, element, element1, /*quiet*/true))
        element1 = "";

      setActiveElement(element1);
    }
    else
      return tk_->wrongNumArgs(getFullName() + " activate element");
  }
  else if (option == "delta") {
    if (numArgs == 3) {
      long xDelta;
      if (! tk_->variantToInt(args[1], xDelta))
        return tk_->invalidInteger(args[1]);
      long yDelta;
      if (! tk_->variantToInt(args[2], yDelta))
        return tk_->invalidInteger(args[2]);

      tk_->setRealResult(0.0); // TODO
    }
    else
      return tk_->wrongNumArgs(getFullName() + " delta xDelta yDelta");
  }
  else if (option == "fraction") {
    if (numArgs == 3) {
      long x;
      if (! tk_->variantToInt(args[1], x))
        return tk_->invalidInteger(args[1]);
      long y;
      if (! tk_->variantToInt(args[2], y))
        return tk_->invalidInteger(args[2]);

      tk_->setRealResult(0.0); // TODO
    }
    else
      return tk_->wrongNumArgs(getFullName() + " fraction x y");
  }
  else if (option == "get") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getFullName() + " get");

    tk_->setStringResult("0 0 0 0");
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
  else if (option == "set") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(
               getFullName() + " set firstFraction lastFraction\" or \"" +
               getFullName() + " set totalUnits windowUnits firstUnit lastUnit");

    double pos1, pos2;
    if (! tk_->variantToReal(args[1], pos1))
      return tk_->invalidReal(args[1]);
    if (! tk_->variantToReal(args[2], pos2))
      return tk_->invalidReal(args[2]);

    connectSlots(false);

    qscrollbar_->setBounds(pos1, pos2);

    //std::cerr << "set " << pos1 << " " << pos2 << "\n";

    connectSlots(true);
  }
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

void
CTkAppScrollBar::
scrollValueSlot(int /*i*/)
{
  //Args args;

  //args.push_back("moveto");
  //args.push_back(QString::number(i/1000.0));

  //runCommand(args);
}

void
CTkAppScrollBar::
scrollActionSlot(int i)
{
//std::cerr << "CTkAppScrollBar::scrollActionSlot ";

  Args args;

  switch (i) {
    case QAbstractSlider::SliderSingleStepAdd:
      args.push_back("scroll"); args.push_back("1"); args.push_back("units"); break;
    case QAbstractSlider::SliderSingleStepSub:
      args.push_back("scroll"); args.push_back("-1"); args.push_back("units"); break;
    case QAbstractSlider::SliderPageStepAdd  :
      args.push_back("scroll"); args.push_back("1"); args.push_back("pages"); break;
    case QAbstractSlider::SliderPageStepSub  :
      args.push_back("scroll"); args.push_back("-1"); args.push_back("pages"); break;
    case QAbstractSlider::SliderToMinimum    :
      args.push_back("moveto"); args.push_back("0"); break;
    case QAbstractSlider::SliderToMaximum    :
      args.push_back("moveto"); args.push_back("1"); break;
    case QAbstractSlider::SliderMove         :
      args.push_back("moveto"); args.push_back(qscrollbar_->getSliderPos()); break;
    default:
    case QAbstractSlider::SliderNoAction     : break;
  }

  runCommand(args);
}

//---

CTkAppScrollBarWidget::
CTkAppScrollBarWidget(CTkAppScrollBar *scrollbar) :
 QScrollBar(scrollbar->getParent() ? scrollbar->getParent()->getQWidget() : nullptr),
 scrollbar_(scrollbar)
{
  setObjectName("scrollbar");

  setPageStep(resolution_);
  setRange(0, 0);
}

void
CTkAppScrollBarWidget::
setBounds(double pos1, double pos2)
{
  if (pos2 <= pos1)
    return;

  double d = (pos2 - pos1);

  setPageStep(int(d*resolution_));
  setRange   (0, int(resolution_) - pageStep());
  setValue   (int(pos1*resolution_));
}

double
CTkAppScrollBarWidget::
getRValue() const
{
  auto r = double(value())/double(resolution_);

  r = std::min(std::max(r, 0.0), 1.0);

  return r;
}

void
CTkAppScrollBarWidget::
setRValue(double r)
{
  if      (r <= 0.0)
    setValue(0);
  else if (r >= 1.0)
    setValue(resolution_);
  else
    setValue(resolution_*r);
}

double
CTkAppScrollBarWidget::
getSliderPos() const
{
  auto pos = double(sliderPosition());

  if (pos > resolution_ - pageStep())
    pos = resolution_ - pageStep();

  return pos/double(resolution_);
}
