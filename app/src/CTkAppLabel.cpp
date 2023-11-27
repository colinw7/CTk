#include <CTkAppLabel.h>
#include <CTkAppImage.h>

class CTkAppLabelVarProc : public CTclTraceProc {
 public:
  CTkAppLabelVarProc(CTkApp *tk, CTkAppLabel *label) :
   CTclTraceProc(tk), label_(label) {
  }

  void handleWrite(const char *) override {
    if (label_)
      label_->updateFromVar();
  }

 private:
  using LabelP = QPointer<CTkAppLabel>;

  LabelP label_;
};

//---

CTkAppLabel::
CTkAppLabel(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qlabel_ = new CTkAppLabelWidget(this);

  setQWidget(qlabel_);
}

bool
CTkAppLabel::
execConfig(const QString &name, const QVariant &var)
{
  auto value = tk_->variantToString(var);

  if      (name == "-compound") {
    CTkAppCompoundType compoundType;
    if (! CTkAppUtil::variantToCompound(tk_, var, compoundType))
      return tk_->throwError("bad compound \"" + value + "\": must be "
                             "bottom, center, left, none, right, or top");

    setCompoundType(compoundType);

    auto type = CQLabelImage::Type::NONE;
    if      (compoundType == CTkAppCompoundType::BOTTOM) type = CQLabelImage::Type::BOTTOM;
    else if (compoundType == CTkAppCompoundType::CENTER) type = CQLabelImage::Type::CENTER;
    else if (compoundType == CTkAppCompoundType::LEFT  ) type = CQLabelImage::Type::LEFT;
    else if (compoundType == CTkAppCompoundType::NONE  ) type = CQLabelImage::Type::NONE;
    else if (compoundType == CTkAppCompoundType::RIGHT ) type = CQLabelImage::Type::RIGHT;
    else if (compoundType == CTkAppCompoundType::TOP   ) type = CQLabelImage::Type::TOP;

    qlabel_->setType(type);
  }
  else if (name == "-height") {
    CTkAppDistance height;
    if (! tk_->variantToDistanceI(var, height))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    qlabel_->setHeight(height.ivalue);
  }
  else if (name == "-state") {
    if (value == "readonly") {
      //qlabel_->setReadOnly(true);
    }
    else {
      if (! setWidgetStateFromString(value))
        return tk_->throwError(tk_->msg() + "bad state \"" + value + "\": must be "
                               "active, disabled, or normal");
    }
  }
  else if (name == "-textvariable") {
    setVarName(value);

    if (! isInitNotify() && ! tk_->hasGlobalVar(varName()))
      tk_->setStringGlobalVar(varName(), QString());

    if (tk_->hasGlobalVar(varName()))
      setText(tk_->getStringGlobalVar(varName()));

    varProc_ = new CTkAppLabelVarProc(tk_, this);

    tk_->traceGlobalVar(varName(), varProc_);
  }
  else if (name == "-underline") {
    long pos;
    if (! tk_->getOptionInt(name, var, pos))
      return tk_->invalidInteger(var);
  }
  else if (name == "-width") {
    CTkAppDistance width;
    if (! tk_->variantToDistanceI(var, width))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    qlabel_->setWidth(width.rvalue);
  }
  else if (name == "-wraplength") {
    CTkAppDistance length;
    if (! tk_->variantToDistance(var, length))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    qlabel_->setWrapLength(length.rvalue);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppLabel::
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
CTkAppLabel::
setText(const QString &text)
{
  CTkAppWidget::setText(text);

  qlabel_->setLabel(text);
}

void
CTkAppLabel::
setImageRef(const CTkAppImageRef &image)
{
  CTkAppWidget::setImageRef(image);

  auto qimage = image->getQImage();

  qlabel_->setImage(qimage);
}

void
CTkAppLabel::
setJustify(const Qt::Alignment &a)
{
  CTkAppWidget::setAnchor(a);

  qlabel_->setJustify(a);
}

void
CTkAppLabel::
updateFromVar()
{
  if (varName() != "" && tk_->hasGlobalVar(varName()))
    setText(tk_->getStringGlobalVar(varName()));
}

//---

CTkAppLabelWidget::
CTkAppLabelWidget(CTkAppLabel *label) :
 CQLabelImage(label->getParent() ? label->getParent()->getQWidget() : nullptr), label_(label)
{
  setObjectName("qlabel");
}

void
CTkAppLabelWidget::
updateText()
{
  if (wrapLength_ > 0) {
    auto text = CTkAppUtil::formatStringInWidth(text_, wrapLength_, font());

    CQLabelImage::setLabel(text);
  }
  else
    CQLabelImage::setLabel(text_);
}

QSize
CTkAppLabelWidget::
sizeHint() const
{
  auto s = CQLabelImage::sizeHint();

  QFontMetrics fm(font());

  QImage image;

  if (label_->getImageRef())
    image = label_->getImageRef()->getQImage();

  if (! image.isNull()) {
    s.setWidth (image.width ());
    s.setHeight(image.height());

    if (width_ && *width_ > 0)
      s.setWidth(*width_);

    if (height_ && *height_ > 0)
      s.setHeight(*height_);
  }
  else {
    if (width_ && *width_ > 0)
      s.setWidth((*width_)*fm.horizontalAdvance("0"));

    if (height_ && *height_ > 0)
      s.setHeight((*height_)*fm.height());
  }

  return s;
}
