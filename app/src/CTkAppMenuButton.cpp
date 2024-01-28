#include <CTkAppMenuButton.h>
#include <CTkAppMenu.h>
#include <CTkAppImage.h>

#include <CQUtil.h>

class CTkAppMenuButtonVarProc : public CTclTraceProc {
 public:
  CTkAppMenuButtonVarProc(CTkApp *tk, CTkAppMenuButton *menuButton) :
   CTclTraceProc(tk), menuButton_(menuButton) {
  }

  void handleWrite(const char *) override {
    if (menuButton_)
      menuButton_->updateFromVar();
  }

 private:
  using MenuButtonP = QPointer<CTkAppMenuButton>;

  MenuButtonP menuButton_;
};

//---

CTkAppMenuButton::
CTkAppMenuButton(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qbutton_ = new CTkAppMenuButtonWidget(this);

  setQWidget(qbutton_);

  connectSlots(true);
}

void
CTkAppMenuButton::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qbutton_,
    SIGNAL(clicked()), this, SLOT(clickSlot()));
}

bool
CTkAppMenuButton::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-command") {
    auto value = tk_->variantToString(var);

    setCommand(value);
  }
  else if (name == "-direction") {
    static auto optNames = std::vector<QString>({
      "above", "below", "flush", "left", "right"});

    QString opt;
    if (! tk_->lookupName("direction", optNames, var, opt))
      return false;

    tk_->TODO(name);
  }
  else if (name == "-indicatoron") {
    bool b;
    if (! tk_->variantToBool(var, b))
      return tk_->invalidBool(var);

    showIndicator_ = b;
  }
  else if (name == "-menu") {
    auto value = tk_->variantToString(var);

    setMenuName(value);

    updateMenu();
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
  else if (name == "-textvariable") {
    auto value = tk_->variantToString(var);

    setVarName(value);

    if (! isInitNotify() && ! tk_->hasGlobalVar(varName()))
      tk_->setStringGlobalVar(varName(), QString());

    if (tk_->hasGlobalVar(varName()))
      setText(tk_->getStringGlobalVar(varName()));

    varProc_ = new CTkAppMenuButtonVarProc(tk_, this);

    tk_->traceGlobalVar(varName(), varProc_);
  }
  else if (name == "-underline") {
    long pos;
    if (! tk_->getOptionInt(name, var, pos))
      return tk_->invalidInteger(var);
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

    setWrapLength(length.rvalue);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppMenuButton::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  static auto optionNames = std::vector<QString>({"cget", "configure"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  return CTkAppWidget::execDefaultOp(option, args);
}

void
CTkAppMenuButton::
setText(const QString &text)
{
  CTkAppWidget::setText(text);

  qbutton_->setText(text);
}

void
CTkAppMenuButton::
setImageRef(const CTkAppImageRef &image)
{
  CTkAppWidget::setImageRef(image);

  if (image) {
    auto pixmap = image->getQPixmap();

    qbutton_->setIcon(QIcon(pixmap));
  }
}

void
CTkAppMenuButton::
updateMenu()
{
  if (menuName() != "") {
    auto *w = tk_->lookupWidgetByName(menuName(), /*quiet*/true);
    if (! w) return; // not defined yet

    auto *menu = dynamic_cast<CTkAppMenu *>(w);
    if (! menu) { std::cerr << "No menu '" << menuName().toStdString() << "'\n"; return; }

    setMenu(menu);
  }
}

void
CTkAppMenuButton::
setMenu(CTkAppMenu *menu)
{
  qbutton_->setPopupMode(QToolButton::InstantPopup);

  qbutton_->setMenu(qobject_cast<QMenu *>(menu->getQWidget()));
}

void
CTkAppMenuButton::
clickSlot()
{
  runCommand();
}

void
CTkAppMenuButton::
updateFromVar()
{
  if (varName() != "" && tk_->hasGlobalVar(varName()))
    setText(tk_->getStringGlobalVar(varName()));
}

//---

CTkAppMenuButtonWidget::
CTkAppMenuButtonWidget(CTkAppMenuButton *button) :
 QToolButton(button->getParent() ? button->getParent()->getQWidget() : nullptr), button_(button)
{
  setToolButtonStyle(Qt::ToolButtonTextOnly);
}
