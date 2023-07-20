#include <CTkWidget.h>
#include <CTk.h>
#include <CTkLayout.h>
#include <CTkImage.h>

#include <CQUtil.h>
#include <CQImage.h>
#include <CStrMap.h>
#include <CRGBName.h>

#include <QCheckBox>
#include <QFrame>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QMouseEvent>

CTkRootWidget::
CTkRootWidget(CTk *tk) :
 CTkWidget(tk, nullptr, "")
{
  qframe_ = new QFrame(nullptr);

  qframe_->resize(1, 1);

  setQWidget(qframe_);
}

bool
CTkRootWidget::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if      (name == "-background") {
    double r, g, b, a;

    CRGBName::lookup(value, &r, &g, &b, &a);

    QColor c(255*r, 255*g, 255*b, 255*a);

    CQUtil::setBackground(qframe_, c);
  }
  else if (name == "-borderwidth") {
    int w;

    if (CStrUtil::toInteger(value, &w))
      qframe_->setLineWidth(w);
  }
  else
    return false;

  return true;
}

bool
CTkRootWidget::
decodeWidgetName(const std::string &name, CTkWidget **parent, std::string &childName) const
{
  uint len = name.size();

  if (len == 0) return false;

  if (name[0] != '.') return false;

  if (len == 1) {
    *parent = const_cast<CTkRootWidget *>(this);

    childName = "";

    return true;
  }

  CTkWidget* parent1 = const_cast<CTkRootWidget *>(this);
  CTkWidget* child1  = nullptr;

  uint pos1 = 1;
  uint pos2 = pos1;

  while (pos2 < len) {
    while (pos2 < len) {
      if (name[pos2] == '.')
        break;

      ++pos2;
    }

    if (pos2 < len)
      childName = name.substr(pos1, pos2 - pos1);
    else
      childName = name.substr(pos1);

    if (childName == "")
      return false;

    if (pos2 >= len)
      break;

    child1 = parent1->getChild(childName);

    if (child1 == nullptr)
      return false;

    parent1 = child1;
    child1  = nullptr;

    pos1 = pos2 + 1;
    pos2 = pos1;
  }

  *parent = parent1;

  return true;
}

//----------

CTkButton::
CTkButton(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qbutton_ = new QPushButton(parent_->getQWidget());
  else
    qbutton_ = new QPushButton(nullptr);

  setQWidget(qbutton_);

  connect(qbutton_, SIGNAL(clicked()), this, SLOT(runCommandSlot()));
}

bool
CTkButton::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if      (name == "-command")
    setCommand(value);
  else if (name == "-text")
    setText(value);
  else if (name == "-image") {
    auto image = tk_->getImage(value);

    if (image)
      setImage(image->getImage());
  }
  else
    return false;

  return true;
}

void
CTkButton::
setCommand(const std::string &command)
{
  command_ = command;
}

void
CTkButton::
setText(const std::string &text)
{
  qbutton_->setText(text.c_str());
}

void
CTkButton::
setImage(CImagePtr image)
{
  auto &qimage = image.cast<CQImage>()->getQImage();

  QPixmap pixmap;

  pixmap.convertFromImage(qimage);

  qbutton_->setIcon(QIcon(pixmap));
}

CTclValueRef
CTkButton::
iexec(const std::vector<CTclValueRef> &args)
{
  const auto &arg = args[0]->toString();

  if (arg == "invoke")
    invoke();

  return CTclValueRef();
}

void
CTkButton::
runCommandSlot()
{
  invoke();
}

void
CTkButton::
invoke()
{
  if (command_ == "") return;

  tk_->getTcl()->parseLine(command_);
}

//----------

CTkCanvas::
CTkCanvas(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qcanvas_ = new CTkCanvasWidget(parent_->getQWidget());
  else
    qcanvas_ = new CTkCanvasWidget();

  setQWidget(qcanvas_);

  qwidget_->setFocusPolicy(Qt::StrongFocus);
}

bool
CTkCanvas::
notifyValueChanged(const std::string &, const std::string &)
{
  return false;
}

CTkCanvasWidget::
CTkCanvasWidget(QWidget *parent) :
 QWidget(parent)
{
}

QSize
CTkCanvasWidget::
sizeHint() const
{
  return QSize(100, 100);
}

//----------

CTkCheckButton::
CTkCheckButton(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qcheck_ = new QCheckBox(parent_->getQWidget());
  else
    qcheck_ = new QCheckBox(nullptr);

  setQWidget(qcheck_);
}

bool
CTkCheckButton::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-text")
    setText(value);
  else
    return false;

  return true;
}

void
CTkCheckButton::
setText(const std::string &text)
{
  qcheck_->setText(text.c_str());
}

//----------

class CTkEntryVarProc : public CTclVariableProc {
 public:
  CTkEntryVarProc(CTkEntry *entry) :
   entry_(entry) {
  }

  void notify(CTclVariable *var) override {
    entry_->setText(var->getValue()->toString());
  }

 private:
  CTkEntry *entry_;
};

CTkEntry::
CTkEntry(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qedit_ = new QLineEdit(parent_->getQWidget());
  else
    qedit_ = new QLineEdit(nullptr);

  setQWidget(qedit_);

  qwidget_->setFocusPolicy(Qt::StrongFocus);

  connect(qedit_, SIGNAL(editingFinished()), this, SLOT(valueChangedSlot()));
}

bool
CTkEntry::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-textvariable") {
    varName_ = value;

    CTclVariableRef var = tk_->getTcl()->getVariable(varName_);

    if (! var.isValid())
      var = tk_->getTcl()->addVariable(varName_, tk_->getTcl()->createValue(""));

    setText(var->getValue()->toString());

    var->addNotifyProc(new CTkEntryVarProc(this));
  }
  else
    return false;

  return true;
}

void
CTkEntry::
setText(const std::string &text)
{
  qedit_->setText(text.c_str());
}

void
CTkEntry::
valueChangedSlot()
{
  if (varName_ != "") {
    CTclVariableRef var = tk_->getTcl()->getVariable(varName_);

    if (! var.isValid())
      var = tk_->getTcl()->addVariable(varName_, CTclValueRef());

    var->setValue(tk_->getTcl()->createValue(qedit_->text().toStdString()));
  }
}

//----------

CTkFrame::
CTkFrame(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qframe_ = new QFrame(parent_->getQWidget());
  else
    qframe_ = new QFrame(nullptr);

  setQWidget(qframe_);
}

bool
CTkFrame::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if      (name == "-relief") {
    if      (value == "raised") {
      qframe_->setFrameShadow(QFrame::Raised);
      qframe_->setFrameShape (QFrame::Panel);
    }
    else if (value == "sunken") {
      qframe_->setFrameShadow(QFrame::Sunken);
      qframe_->setFrameShape (QFrame::Panel);
    }
    else if (value == "flat") {
      qframe_->setFrameShadow(QFrame::Plain);
      qframe_->setFrameShape (QFrame::NoFrame);
    }
    else if (value == "ridge") {
      qframe_->setFrameShadow(QFrame::Raised);
      qframe_->setFrameShape (QFrame::Box);
    }
    else if (value == "solid") {
      qframe_->setFrameShadow(QFrame::Plain);
      qframe_->setFrameShape (QFrame::Panel);
    }
    else if (value == "groove") {
      qframe_->setFrameShadow(QFrame::Sunken);
      qframe_->setFrameShape (QFrame::Box);
    }
  }
  else if (name == "-borderwidth") {
    int w;

    if (CStrUtil::toInteger(value, &w))
      qframe_->setLineWidth(w);
  }

  return false;
}

//----------

CTkLabel::
CTkLabel(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qlabel_ = new QLabel(parent_->getQWidget());
  else
    qlabel_ = new QLabel(nullptr);

  setQWidget(qlabel_);
}

bool
CTkLabel::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if      (name == "-text")
    setText(value);
  else if (name == "-image") {
    auto image = tk_->getImage(value);

    if (image)
      setImage(image->getImage());
  }
  else
    return false;

  return true;
}

void
CTkLabel::
setText(const std::string &text)
{
  qlabel_->setText(text.c_str());
}

void
CTkLabel::
setImage(CImagePtr image)
{
  auto &qimage = image.cast<CQImage>()->getQImage();

  QPixmap pixmap;

  pixmap.convertFromImage(qimage);

  qlabel_->setPixmap(pixmap);
}

//----------

CTkLabelFrame::
CTkLabelFrame(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qframe_ = new QGroupBox(parent_->getQWidget());
  else
    qframe_ = new QGroupBox(nullptr);

  setQWidget(qframe_);
}

bool
CTkLabelFrame::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-text")
    setText(value);
  else
    return false;

  return true;
}

void
CTkLabelFrame::
setText(const std::string &text)
{
  qframe_->setTitle(text.c_str());
}

//----------

CTkListBox::
CTkListBox(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qlist_ = new QListWidget(parent_->getQWidget());
  else
    qlist_ = new QListWidget(nullptr);

  setQWidget(qlist_);
}

bool
CTkListBox::
notifyValueChanged(const std::string &, const std::string &)
{
  return false;
}

//----------

CTkPanedWindow::
CTkPanedWindow(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qpane_ = new QSplitter(parent_->getQWidget());
  else
    qpane_ = new QSplitter(nullptr);

  qpane_->setChildrenCollapsible(false);

  setQWidget(qpane_);
}

bool
CTkPanedWindow::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-orient") {
    static CStrMap<Qt::Orientation, int> orientMap(
      "horizontal", Qt::Horizontal,
      "vertical"  , Qt::Vertical  ,
      0);

    Qt::Orientation orient;

    bool rc = orientMap.map(value, orient);

    if (rc)
      qpane_->setOrientation(orient);

    return true;
  }
  else
    return false;
}

CTclValueRef
CTkPanedWindow::
iexec(const std::vector<CTclValueRef> &args)
{
  uint numArgs = args.size();

  const auto &arg = args[0]->toString();

  if (arg == "add") {
    if (numArgs < 2) {
      tk_->getTcl()->wrongNumArgs(getName() + " add widget ?widget ...?");
      return CTclValueRef();
    }

    int min_size = -1;
    int padx     = -1;
    int pady     = -1;

    for (uint i = 1; i < numArgs; ++i) {
      const auto &name = args[i]->toString();

      if (name.size() > 0 && name[0] == '-') {
        if      (name == "-minsize") {
          ++i;

          if (i >= numArgs) {
            tk_->getTcl()->throwError("Missing value for \"-minsize\"");
            return CTclValueRef();
          }

          std::string value = args[i]->toString();

          int i;

          if (CStrUtil::toInteger(value, &i))
            min_size = i;
        }
        else if (name == "-padx") {
          ++i;

          if (i >= numArgs) {
            tk_->getTcl()->throwError("Missing value for \"-padx\"");
            return CTclValueRef();
          }

          std::string value = args[i]->toString();

          int i;

          if (CStrUtil::toInteger(value, &i))
            padx = i;

          std::cerr << "padx " << padx << " not supported" << std::endl;
        }
        else if (name == "-pady") {
          ++i;

          if (i >= numArgs) {
            tk_->getTcl()->throwError("Missing value for \"-pady\"");
            return CTclValueRef();
          }

          std::string value = args[i]->toString();

          int i;

          if (CStrUtil::toInteger(value, &i))
            pady = i;

          std::cerr << "pady " << pady << " not supported" << std::endl;
        }
      }
      else {
        CTkWidget*  parent;
        std::string childName;

        if (! root()->decodeWidgetName(name, &parent, childName)) {
          tk_->getTcl()->throwError("Invalid name '" + name + "'");
          return CTclValueRef();
        }

        auto *child = parent->getChild(childName);

        if (child == nullptr) {
          tk_->getTcl()->throwError("Invalid name '" + name + "'");
          return CTclValueRef();
        }

        QWidget *qwidget = child->getQWidget();

        if (min_size > 0) {
          if (qpane_->orientation() == Qt::Horizontal)
            qwidget->setMinimumHeight(min_size);
          else
            qwidget->setMinimumWidth (min_size);
        }

        qpane_->addWidget(qwidget);
      }
    }
  }

  return CTclValueRef();
}

//----------

CTkRadioButton::
CTkRadioButton(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qradio_ = new QRadioButton(parent_->getQWidget());
  else
    qradio_ = new QRadioButton(nullptr);

  setQWidget(qradio_);
}

bool
CTkRadioButton::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-text")
    setText(value);
  else
    return false;

  return true;
}

void
CTkRadioButton::
setText(const std::string &text)
{
  qradio_->setText(text.c_str());
}

//----------

CTkScale::
CTkScale(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qscale_ = new QSlider(parent_->getQWidget());
  else
    qscale_ = new QSlider(nullptr);

  setQWidget(qscale_);
}

bool
CTkScale::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-text")
    setText(value);
  else
    return false;

  return true;
}

void
CTkScale::
setText(const std::string &)
{
  //qscale_->setLabel(text.c_str());
}

//----------

CTkScrollBar::
CTkScrollBar(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qscrollbar_ = new QScrollBar(parent_->getQWidget());
  else
    qscrollbar_ = new QScrollBar(nullptr);

  setQWidget(qscrollbar_);
}

bool
CTkScrollBar::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-orient") {
    static CStrMap<Qt::Orientation, int> orientMap(
      "horizontal", Qt::Horizontal,
      "vertical"  , Qt::Vertical  ,
      0);

    Qt::Orientation orient;

    bool rc = orientMap.map(value, orient);

    if (rc)
      qscrollbar_->setOrientation(orient);

    return true;
  }
  else
    return false;
}

//----------

CTkSpinBox::
CTkSpinBox(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qspin_ = new QSpinBox(parent_->getQWidget());
  else
    qspin_ = new QSpinBox(nullptr);

  setQWidget(qspin_);

  qwidget_->setFocusPolicy(Qt::StrongFocus);
}

bool
CTkSpinBox::
notifyValueChanged(const std::string &, const std::string &)
{
  return false;
}

//----------

CTkText::
CTkText(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qtext_ = new QTextEdit(parent_->getQWidget());
  else
    qtext_ = new QTextEdit(nullptr);

  setQWidget(qtext_);

  qwidget_->setFocusPolicy(Qt::StrongFocus);
}

bool
CTkText::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-text")
    setText(value);
  else
    return false;

  return true;
}

void
CTkText::
setText(const std::string &text)
{
  qtext_->setText(text.c_str());
}

//----------

CTkTopLevel::
CTkTopLevel(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  qframe_ = new QFrame(nullptr);

  setQWidget(qframe_);
}

bool
CTkTopLevel::
notifyValueChanged(const std::string &, const std::string &)
{
  return false;
}

//----------

CTkWidget::
CTkWidget(CTk *tk, CTkWidget *parent, const std::string &name) :
 tk_(tk), parent_(parent), name_(name), w_(0), h_(0), deleted_(false), qwidget_(nullptr)
{
  if (parent_)
    parent_->addChild(this);

  tk_->addWidget(this);
}

CTkWidget::
~CTkWidget()
{
  if (parent_)
    parent_->removeChild(this);

  tk_->removeWidget(this);

  delete qwidget_;
}

CTkRootWidget *
CTkWidget::
root() const
{
  return tk_->getRootWidget();
}

std::string
CTkWidget::
getFullName() const
{
  std::string name = getName();

  if (parent_ != nullptr) {
    std::string pname = parent_->getFullName();

    if (pname != ".")
      pname += ".";

    name = pname + name;
  }
  else {
    if (name == "")
      name = ".";
  }

  return name;
}

void
CTkWidget::
setQWidget(QWidget *w)
{
  qwidget_ = w;

  if (name_ != "")
    qwidget_->setObjectName(name_.c_str());
  else
    qwidget_->setObjectName(".");

  qwidget_->installEventFilter(new CTkWidgetEventFilter(this));

  qwidget_->setFocusPolicy(Qt::ClickFocus);
}

QWidget *
CTkWidget::
getQWidget() const
{
  return qwidget_;
}

int
CTkWidget::
getWidth() const
{
  return (w_ > 0 ? w_ : qwidget_->sizeHint().width());
}

int
CTkWidget::
getHeight() const
{
  return (h_ > 0 ? h_ : qwidget_->sizeHint().height());
}

void
CTkWidget::
setWidth(int w)
{
  w_ = w;
}

void
CTkWidget::
setHeight(int h)
{
  h_ = h;
}

void
CTkWidget::
addChild(CTkWidget *w)
{
  children_[w->getName()] = w;
}

void
CTkWidget::
removeChild(CTkWidget *w)
{
  auto p = children_.find(w->getName());

  if (p != children_.end())
    children_.erase(p);
}

void
CTkWidget::
getChildren(std::vector<CTkWidget *> &children) const
{
  for (const auto &pc: children_)
    children.push_back(pc.second);
}

CTkWidget *
CTkWidget::
getChild(const std::string &name) const
{
  auto p = children_.find(name);

  if (p == children_.end())
    return nullptr;

  return (*p).second;
}

void
CTkWidget::
setFocus()
{
  QWidget *w = getQWidget();
  assert(w);

  w->setFocus();
}

void
CTkWidget::
lower()
{
  QWidget *w = getQWidget();
  assert(w);

  w->lower();
}

void
CTkWidget::
raise()
{
  QWidget *w = getQWidget();
  assert(w);

  w->raise();
}

CTkPackLayout *
CTkWidget::
getTkPackLayout()
{
  QWidget *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  auto *l1 = qobject_cast<CTkPackLayout *>(l);

  if (! l1)
    l1 = new CTkPackLayout(w);

  return l1;
}

CTkGridLayout *
CTkWidget::
getTkGridLayout()
{
  QWidget *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  auto *l1 = qobject_cast<CTkGridLayout *>(l);

  if (! l1)
    l1 = new CTkGridLayout(w);

  return l1;
}

CTkPlaceLayout *
CTkWidget::
getTkPlaceLayout()
{
  QWidget *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  auto *l1 = qobject_cast<CTkPlaceLayout *>(l);

  if (! l1)
    l1 = new CTkPlaceLayout(w);

  return l1;
}

bool
CTkWidget::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if      (name == "-width") {
    int w;

    if (CStrUtil::toInteger(value, &w)) {
      setWidth(w);

      qwidget_->resize(getWidth(), getHeight());
    }
  }
  else if (name == "-height") {
    int h;

    if (CStrUtil::toInteger(value, &h)) {
      setHeight(h);

      qwidget_->resize(getWidth(), getHeight());
    }
  }
  else if (name == "-background") {
    double r, g, b, a;

    CRGBName::lookup(value, &r, &g, &b, &a);

    QColor c(255*r, 255*g, 255*b, 255*a);

    CQUtil::setBackground(qwidget_, c);
  }
  else
    return false;

  return true;
}

void
CTkWidget::
show()
{
  if (qwidget_)
    qwidget_->show();
}

void
CTkWidget::
setTitle(const std::string &title)
{
  qwidget_->setWindowTitle(title.c_str());
}

std::string
CTkWidget::
getTitle() const
{
  return qwidget_->windowTitle().toStdString();
}

void
CTkWidget::
bindEvent(const std::string &pattern, const std::string &command)
{
  /*
  Modifier Types
  --------------
  Control Mod2, M2
  Shift   Mod3, M3
  Lock    Mod4, M4
  Button1, B1   Mod5, M5
  Button2, B2   Meta, M
  Button3, B3   Alt
  Button4, B4   Double
  Button5, B5   Triple
  Mod1, M1      Quadruple
  */

  /*
  Event Types
  -----------
  Activate Destroy Map
  ButtonPress, Button Enter MapRequest
  ButtonRelease Expose Motion
  Circulate FocusIn MouseWheel
  CirculateRequest FocusOut Property
  Colormap Gravity Reparent
  Configure KeyPress, Key ResizeRequest
  ConfigureRequest KeyRelease Unmap
  Create Leave Visibility
  Deactivate
  */

  auto len = pattern.size();

  if      (len > 2 && pattern[0] == '<' && isdigit(pattern[1]) && pattern[len - 1] == '>')
    events_["<ButtonPress-" + pattern.substr(1, len - 2) + ">"] = command;
  else if (len > 10 && pattern.substr(0, 2) == "<B" && pattern.substr(len - 8, 8) == "-Motion>")
    events_["<ButtonMotion-" + pattern.substr(2, len - 10) + ">"] = command;
  else
    events_[pattern] = command;
}

bool
CTkWidget::
triggerEnterEvents(QEvent *e)
{
  auto p = events_.find("<Enter>");

  if (p != events_.end())
    return tk_->execEvent(this, e, (*p).second);

  return tk_->triggerEnterEvents(getClassName(), this, e);
}

bool
CTkWidget::
triggerLeaveEvents(QEvent *e)
{
  auto p = events_.find("<Leave>");

  if (p != events_.end())
    return tk_->execEvent(this, e, (*p).second);

  return tk_->triggerLeaveEvents(getClassName(), this, e);
}

bool
CTkWidget::
triggerMousePressEvents(QEvent *e)
{
  auto *me = static_cast<QMouseEvent *>(e);

  auto p = events_.find("<Button>");

  if (p == events_.end())
    p = events_.find("<ButtonPress>");

  if (p == events_.end()) {
    auto eventName = "<ButtonPress-" + std::to_string(me->button()) + ">";

    p = events_.find(eventName);
  }

  if (p != events_.end())
    return tk_->execEvent(this, e, (*p).second);

  return false;
}

bool
CTkWidget::
triggerMouseMoveEvents(QEvent *e, int button)
{
  auto p = events_.find("<ButtonMotion>");

  if (p == events_.end()) {
    auto eventName = "<ButtonMotion-" + std::to_string(button) + ">";

    p = events_.find(eventName);
  }

  if (p != events_.end())
    return tk_->execEvent(this, e, (*p).second);

  return false;
}

bool
CTkWidget::
triggerMouseReleaseEvents(QEvent *e)
{
  auto *me = static_cast<QMouseEvent *>(e);

  auto p = events_.find("<ButtonRelease>");

  if (p == events_.end()) {
    auto eventName = "<ButtonRelease-" + std::to_string(me->button()) + ">";

    p = events_.find(eventName);
  }

  if (p != events_.end())
    return tk_->execEvent(this, e, (*p).second);

  return false;
}

bool
CTkWidget::
triggerKeyPressEvents(QEvent *e)
{
  auto *ke = dynamic_cast<QKeyEvent *>(e);

  int k = ke->key();

  std::string str("<");

  if (ke->modifiers() & Qt::ControlModifier)
    str += "Control-";

  if (ke->modifiers() & Qt::ShiftModifier)
    str += "Shift-";

  if (k > 32 && k < 0x7f)
    str += tolower(char(k));
  else
    return false;

  str += ">";

  auto p = events_.find(str);

  if (p != events_.end())
    return tk_->execEvent(this, e, (*p).second);

  return tk_->triggerKeyPressEvents(getClassName(), this, e, str);
}

void
CTkWidget::
deleteLater()
{
  deleted_ = true;

  tk_->addDeleteWidget(this);
}

//----------

bool
CTkWidgetEventFilter::
eventFilter(QObject *obj, QEvent *event)
{
  if (w_->isDeleted()) return false;

  switch (event->type()) {
    case QEvent::Enter: w_->triggerEnterEvents(event); break;
    case QEvent::Leave: w_->triggerLeaveEvents(event); break;

    case QEvent::MouseButtonPress: {
      auto *me = static_cast<QMouseEvent *>(event);

      pressed_     = true;
      pressButton_ = me->button();

      w_->triggerMousePressEvents(event);

      break;
    }
    case QEvent::MouseMove: {
      w_->triggerMouseMoveEvents(event, pressButton_);
      break;
    }
    case QEvent::MouseButtonRelease: {
      w_->triggerMouseReleaseEvents(event);

      pressed_     = true;
      pressButton_ = 0;

      break;
    }

    case QEvent::KeyPress: {
      w_->triggerKeyPressEvents(event);
      break;
    }

    default: break;
  }

  // standard event processing
  return QObject::eventFilter(obj, event);
}
