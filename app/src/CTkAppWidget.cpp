#include <CTkAppWidget.h>
#include <CTkApp.h>
#include <CTkAppPackLayout.h>
#include <CTkAppPlaceLayout.h>
#include <CTkAppGridLayout.h>
#include <CTkAppLayout.h>
#include <CTkAppImage.h>

#include <CQUtil.h>
#include <CQImage.h>
#include <CStrMap.h>
#include <CRGBName.h>

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QTextEdit>
#include <QToolButton>
#include <QHBoxLayout>
#include <QMouseEvent>

namespace {

QColor stringToQColor(const std::string &str) {
  double r, g, b, a;

  CRGBName::lookup(str, &r, &g, &b, &a);

  return QColor(255*r, 255*g, 255*b, 255*a);
}

}

//---

CTkAppRoot::
CTkAppRoot(CTkApp *tk) :
 CTkWidget(tk, nullptr, "")
{
  qframe_ = new CTkRootWidget(nullptr);

  qframe_->resize(qframe_->sizeHint());

  setQWidget(qframe_);
}

bool
CTkAppRoot::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if      (name == "-background") {
    auto c = stringToQColor(value);

    CQUtil::setBackground(qframe_, c);
  }
  else if (name == "-borderwidth") {
    int w;

    if (CStrUtil::toInteger(value, &w))
      qframe_->setLineWidth(w);
  }
  else
    return CTkWidget::notifyValueChanged(name, value);

  return true;
}

std::string
CTkAppRoot::
iexec(const std::vector<std::string> &args)
{
  return CTkWidget::iexec(args);
}

bool
CTkAppRoot::
decodeWidgetName(const std::string &name, CTkWidget **parent, std::string &childName) const
{
  uint len = name.size();

  if (len == 0) return false;

  if (name[0] != '.') return false;

  if (len == 1) {
    *parent = const_cast<CTkAppRoot *>(this);

    childName = "";

    return true;
  }

  CTkWidget* parent1 = const_cast<CTkAppRoot *>(this);
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

//---

CTkRootWidget::
CTkRootWidget(QWidget *parent) :
 QFrame(parent)
{
}

QSize
CTkRootWidget::
sizeHint() const
{
  return QSize(400, 400);
}

//----------

CTkButton::
CTkButton(CTkApp *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qbutton_ = new QPushButton(parent_->getQWidget());
  else
    qbutton_ = new QPushButton(nullptr);

  setQWidget(qbutton_);

  connect(qbutton_, SIGNAL(clicked()), this, SLOT(clickSlot()));
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
    return CTkWidget::notifyValueChanged(name, value);

  return true;
}

std::string
CTkButton::
iexec(const std::vector<std::string> &args)
{
  const auto &arg = args[0];

  if      (arg == "invoke")
    runCommand();
  else if (arg == "flash")
    flash();
  else
    return CTkWidget::iexec(args);

  return std::string();
}

void
CTkButton::
setText(const std::string &text)
{
  qbutton_->setText(QString::fromStdString(text));
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

void
CTkButton::
clickSlot()
{
  runCommand();
}

void
CTkButton::
flash()
{
  // TODO
}

//----------

CTkCanvas::
CTkCanvas(CTkApp *tk, CTkWidget *parent, const std::string &name) :
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
notifyValueChanged(const std::string &name, const std::string &value)
{
  return CTkWidget::notifyValueChanged(name, value);
}

std::string
CTkCanvas::
iexec(const std::vector<std::string> &args)
{
  return CTkWidget::iexec(args);
}

//---

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
CTkCheckButton(CTkApp *tk, CTkWidget *parent, const std::string &name) :
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
    return CTkWidget::notifyValueChanged(name, value);

  return true;
}

std::string
CTkCheckButton::
iexec(const std::vector<std::string> &args)
{
  return CTkWidget::iexec(args);
}

void
CTkCheckButton::
setText(const std::string &text)
{
  qcheck_->setText(QString::fromStdString(text));
}

//----------

class CTkEntryVarProc : public CTclTraceProc {
 public:
  CTkEntryVarProc(CTkApp *tk, CTkEntry *entry) :
   CTclTraceProc(tk), entry_(entry) {
  }

  void handleWrite(const char *name) override {
    entry_->setText(app_->getStringVar(name));
  }

 private:
  CTkEntry *entry_ { nullptr };
};

CTkEntry::
CTkEntry(CTkApp *tk, CTkWidget *parent, const std::string &name) :
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

    if (! tk_->hasVar(varName_))
      tk_->setStringVar(varName_, "");

    setText(tk_->getStringVar(varName_));

    tk_->traceVar(varName_, new CTkEntryVarProc(tk_, this));
  }
  else
    return CTkWidget::notifyValueChanged(name, value);

  return true;
}

std::string
CTkEntry::
iexec(const std::vector<std::string> &args)
{
  return CTkWidget::iexec(args);
}

void
CTkEntry::
setText(const std::string &text)
{
  qedit_->setText(QString::fromStdString(text));
}

void
CTkEntry::
valueChangedSlot()
{
  if (varName_ != "") {
    if (! tk_->hasVar(varName_))
      tk_->setStringVar(varName_, "");

    tk_->setStringVar(varName_, qedit_->text().toStdString());
  }
}

//----------

CTkFrame::
CTkFrame(CTkApp *tk, CTkWidget *parent, const std::string &name) :
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
  else
    return CTkWidget::notifyValueChanged(name, value);

  return true;
}

std::string
CTkFrame::
iexec(const std::vector<std::string> &args)
{
  return CTkWidget::iexec(args);
}

//----------

CTkLabel::
CTkLabel(CTkApp *tk, CTkWidget *parent, const std::string &name) :
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
    return CTkWidget::notifyValueChanged(name, value);

  return true;
}

std::string
CTkLabel::
iexec(const std::vector<std::string> &args)
{
  return CTkWidget::iexec(args);
}

void
CTkLabel::
setText(const std::string &text)
{
  qlabel_->setText(QString::fromStdString(text));
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
CTkLabelFrame(CTkApp *tk, CTkWidget *parent, const std::string &name) :
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
    return CTkWidget::notifyValueChanged(name, value);

  return true;
}

std::string
CTkLabelFrame::
iexec(const std::vector<std::string> &args)
{
  return CTkWidget::iexec(args);
}

void
CTkLabelFrame::
setText(const std::string &text)
{
  qframe_->setTitle(QString::fromStdString(text));
}

//----------

CTkListBox::
CTkListBox(CTkApp *tk, CTkWidget *parent, const std::string &name) :
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
notifyValueChanged(const std::string &name, const std::string &value)
{
  return CTkWidget::notifyValueChanged(name, value);
}

std::string
CTkListBox::
iexec(const std::vector<std::string> &args)
{
  return CTkWidget::iexec(args);
}

//----------

CTkMenu::
CTkMenu(CTkApp *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qmenu_ = new QMenu(parent_->getQWidget());
  else
    qmenu_ = new QMenu(nullptr);

  setQWidget(qmenu_);

  auto *menuButton = dynamic_cast<CTkMenuButton *>(parent);

  if (menuButton)
    menuButton->setMenu(this);
}

bool
CTkMenu::
notifyValueChanged(const std::string &name, const std::string &value)
{
  return CTkWidget::notifyValueChanged(name, value);
}

std::string
CTkMenu::
iexec(const std::vector<std::string> &args)
{
  uint numArgs = args.size();

  const auto &arg = args[0];

  if (arg == "add") {
    if (numArgs < 2)
      tk_->throwError("Invalid menu add args");

    const auto &type = args[1];

    QAction *action = nullptr;

    if      (type == "cascade") {
    }
    else if (type == "checkbutton") {
    }
    else if (type == "command") {
      action = new QAction("");
    }
    else if (type == "radiobutton") {
    }
    else if (type == "separator") {
    }

    for (uint i = 2; i < numArgs - 1; i += 2) {
      const auto &name  = args[i    ];
      const auto &value = args[i + 1];

      if      (name == "-command") {
        if (action) {
          action->setProperty("CTkApp::Command", QString::fromStdString(value));

          connect(action, SIGNAL(triggered()), this, SLOT(actionPressedSlot()));
        }
      }
      else if (name == "-label") {
        if (action)
          action->setText(QString::fromStdString(value));
      }
      else
        tk_->throwError("Invalid menu add option \"" + name + "\"");
    }

    if (action)
      qmenu_->addAction(action);
  }
  else
    return CTkWidget::iexec(args);

  return std::string();
}

void
CTkMenu::
actionPressedSlot()
{
  auto *action = qobject_cast<QAction *>(sender());
  if (! action) return;

  auto cmd = action->property("CTkApp::Command").toString();

  if (cmd != "")
    tk_->eval(cmd.toLatin1().constData());
}

//----------

CTkMenuButton::
CTkMenuButton(CTkApp *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qbutton_ = new QToolButton(parent_->getQWidget());
  else
    qbutton_ = new QToolButton(nullptr);

  qbutton_->setToolButtonStyle(Qt::ToolButtonTextOnly);

  setQWidget(qbutton_);

  connect(qbutton_, SIGNAL(clicked()), this, SLOT(clickSlot()));
}

bool
CTkMenuButton::
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
    return CTkWidget::notifyValueChanged(name, value);

  return true;
}

std::string
CTkMenuButton::
iexec(const std::vector<std::string> &args)
{
  const auto &arg = args[0];

  if (arg == "invoke")
    runCommand();
  else
    return CTkWidget::iexec(args);

  return std::string();
}

void
CTkMenuButton::
setText(const std::string &text)
{
  qbutton_->setText(QString::fromStdString(text));
}

void
CTkMenuButton::
setImage(CImagePtr image)
{
  auto &qimage = image.cast<CQImage>()->getQImage();

  QPixmap pixmap;

  pixmap.convertFromImage(qimage);

  qbutton_->setIcon(QIcon(pixmap));
}

void
CTkMenuButton::
setMenu(CTkMenu *menu)
{
  qbutton_->setPopupMode(QToolButton::InstantPopup);

  qbutton_->setMenu(qobject_cast<QMenu *>(menu->getQWidget()));
}

void
CTkMenuButton::
clickSlot()
{
  runCommand();
}

//----------

class CTkMessageVarProc : public CTclTraceProc {
 public:
  CTkMessageVarProc(CTkApp *tk, CTkMessage *message) :
   CTclTraceProc(tk), message_(message) {
  }

  void handleWrite(const char *name) override {
    message_->setText(app_->getStringVar(name));
  }

 private:
  CTkMessage *message_ { nullptr };
};

CTkMessage::
CTkMessage(CTkApp *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qedit_ = new QTextEdit(parent_->getQWidget());
  else
    qedit_ = new QTextEdit(nullptr);

  qedit_->setReadOnly(true);

  setQWidget(qedit_);
}

bool
CTkMessage::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if      (name == "-text")
    setText(value);
  else if (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify()) {
      if (! tk_->hasVar(varName_))
        tk_->setStringVar(varName_, "");

      setText(tk_->getStringVar(varName_));
    }

    tk_->traceVar(varName_, new CTkMessageVarProc(tk_, this));
  }
  else if (name == "-relief") {
    if      (value == "raised") {
      qedit_->setFrameShadow(QFrame::Raised);
      qedit_->setFrameShape (QFrame::Panel);
    }
    else if (value == "sunken") {
      qedit_->setFrameShadow(QFrame::Sunken);
      qedit_->setFrameShape (QFrame::Panel);
    }
    else if (value == "flat") {
      qedit_->setFrameShadow(QFrame::Plain);
      qedit_->setFrameShape (QFrame::NoFrame);
    }
    else if (value == "ridge") {
      qedit_->setFrameShadow(QFrame::Raised);
      qedit_->setFrameShape (QFrame::Box);
    }
    else if (value == "solid") {
      qedit_->setFrameShadow(QFrame::Plain);
      qedit_->setFrameShape (QFrame::Panel);
    }
    else if (value == "groove") {
      qedit_->setFrameShadow(QFrame::Sunken);
      qedit_->setFrameShape (QFrame::Box);
    }
  }
  else
    return CTkWidget::notifyValueChanged(name, value);

  return true;
}

std::string
CTkMessage::
iexec(const std::vector<std::string> &args)
{
  return CTkWidget::iexec(args);
}

void
CTkMessage::
setText(const std::string &text)
{
  qedit_->setText(QString::fromStdString(text));
}

//----------

CTkPanedWindow::
CTkPanedWindow(CTkApp *tk, CTkWidget *parent, const std::string &name) :
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
  if      (name == "-orient") {
    static CStrMap<Qt::Orientation, int> orientMap(
      "horizontal", Qt::Horizontal,
      "vertical"  , Qt::Vertical  ,
      0);

    Qt::Orientation orient;

    bool rc = orientMap.map(value, orient);

    if (rc)
      qpane_->setOrientation(orient);
  }
  else if (name == "-showhandle") {
    // TODO
  }
  else if (name == "-padx") {
    // TODO
  }
  else if (name == "-pady") {
    // TODO
  }
  else
    return CTkWidget::notifyValueChanged(name, value);

  return true;
}

std::string
CTkPanedWindow::
iexec(const std::vector<std::string> &args)
{
  uint numArgs = args.size();

  const auto &arg = args[0];

  if (arg == "add") {
    if (numArgs < 2) {
      tk_->wrongNumArgs(getName() + " add widget ?widget ...?");
      return std::string();
    }

    int min_size = -1;
    int padx     = -1;
    int pady     = -1;

    for (uint i = 1; i < numArgs; ++i) {
      const auto &name = args[i];

      if (name.size() > 0 && name[0] == '-') {
        if      (name == "-minsize") {
          ++i;

          if (i >= numArgs) {
            tk_->throwError("Missing value for \"-minsize\"");
            return std::string();
          }

          std::string value = args[i];

          int i;

          if (CStrUtil::toInteger(value, &i))
            min_size = i;
        }
        else if (name == "-padx") {
          ++i;

          if (i >= numArgs) {
            tk_->throwError("Missing value for \"-padx\"");
            return std::string();
          }

          int pad;

          if (CStrUtil::toInteger(args[i], &pad))
            padx = pad;

          std::cerr << "padx " << padx << " not supported\n";
        }
        else if (name == "-pady") {
          ++i;

          if (i >= numArgs) {
            tk_->throwError("Missing value for \"-pady\"");
            return std::string();
          }

          int pad;

          if (CStrUtil::toInteger(args[i], &pad))
            pady = pad;

          std::cerr << "pady " << pady << " not supported\n";
        }
      }
      else {
        CTkWidget*  parent;
        std::string childName;

        if (! root()->decodeWidgetName(name, &parent, childName)) {
          tk_->throwError("Invalid name '" + name + "'");
          return std::string();
        }

        auto *child = parent->getChild(childName);

        if (child == nullptr) {
          tk_->throwError("Invalid name '" + name + "'");
          return std::string();
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
  else
    return CTkWidget::iexec(args);

  return std::string();
}

//----------

CTkRadioButton::
CTkRadioButton(CTkApp *tk, CTkWidget *parent, const std::string &name) :
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
    return CTkWidget::notifyValueChanged(name, value);

  return true;
}

std::string
CTkRadioButton::
iexec(const std::vector<std::string> &args)
{
  return CTkWidget::iexec(args);
}

void
CTkRadioButton::
setText(const std::string &text)
{
  qradio_->setText(QString::fromStdString(text));
}

//----------

CTkScale::
CTkScale(CTkApp *tk, CTkWidget *parent, const std::string &name) :
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
    return CTkWidget::notifyValueChanged(name, value);

  return true;
}

std::string
CTkScale::
iexec(const std::vector<std::string> &args)
{
  return CTkWidget::iexec(args);
}

void
CTkScale::
setText(const std::string &)
{
  //qscale_->setLabel(QString::fromStdString(text));
}

//----------

CTkScrollBar::
CTkScrollBar(CTkApp *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qscrollbar_ = new QScrollBar(parent_->getQWidget());
  else
    qscrollbar_ = new QScrollBar(nullptr);

  connect(qscrollbar_, SIGNAL(valueChanged(int)), this, SLOT(scrollSlot(int)));

  setQWidget(qscrollbar_);
}

bool
CTkScrollBar::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if      (name == "-orient") {
    static CStrMap<Qt::Orientation, int> orientMap(
      "horizontal", Qt::Horizontal,
      "vertical"  , Qt::Vertical  ,
      0);

    Qt::Orientation orient;

    bool rc = orientMap.map(value, orient);

    if (rc)
      qscrollbar_->setOrientation(orient);
  }
  else if (name == "-command") {
    setCommand(value);
  }
  else
    return CTkWidget::notifyValueChanged(name, value);

  return true;
}

std::string
CTkScrollBar::
iexec(const std::vector<std::string> &args)
{
  return CTkWidget::iexec(args);
}

void
CTkScrollBar::
scrollSlot(int)
{
  runCommand();
}

//----------

CTkSpinBox::
CTkSpinBox(CTkApp *tk, CTkWidget *parent, const std::string &name) :
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
notifyValueChanged(const std::string &name, const std::string &value)
{
  return CTkWidget::notifyValueChanged(name, value);
}

std::string
CTkSpinBox::
iexec(const std::vector<std::string> &args)
{
  return CTkWidget::iexec(args);
}

//----------

CTkText::
CTkText(CTkApp *tk, CTkWidget *parent, const std::string &name) :
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
  if      (name == "-text") {
    setText(value);
  }
  else if (name == "-wrap") {
    // TODO
  }
  else if (name == "-xscrollcommand") {
    xScrollCommand_ = value;
  }
  else if (name == "-yscrollcommand") {
    yScrollCommand_ = value;
  }
  else if (name == "-command") {
    setCommand(value);
  }
  else
    return CTkWidget::notifyValueChanged(name, value);

  return true;
}

std::string
CTkText::
iexec(const std::vector<std::string> &args)
{
  const auto &arg = args[0];

  if      (arg == "xview") {
    // TODO
  }
  else if (arg == "yview") {
    // TODO
  }
  else
    return CTkWidget::iexec(args);

  return std::string();
}

void
CTkText::
setText(const std::string &text)
{
  qtext_->setText(QString::fromStdString(text));
}

//----------

CTkTopLevel::
CTkTopLevel(CTkApp *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  qframe_ = new QFrame(nullptr);

  setQWidget(qframe_);
}

bool
CTkTopLevel::
notifyValueChanged(const std::string &name, const std::string &value)
{
  return CTkWidget::notifyValueChanged(name, value);
}

std::string
CTkTopLevel::
iexec(const std::vector<std::string> &args)
{
  return CTkWidget::iexec(args);
}

//----------

CTkWidget::
CTkWidget(CTkApp *tk, CTkWidget *parent, const std::string &name) :
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

CTkAppRoot *
CTkWidget::
root() const
{
  return tk_->root();
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
    qwidget_->setObjectName(QString::fromStdString(name_));
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
  WidgetMap::iterator p = children_.find(w->getName());

  if (p != children_.end())
    children_.erase(p);
}

void
CTkWidget::
getChildren(std::vector<CTkWidget *> &children) const
{
  WidgetMap::const_iterator p1, p2;

  for (p1 = children_.begin(), p2 = children_.end(); p1 != p2; ++p1)
    children.push_back((*p1).second);
}

CTkWidget *
CTkWidget::
getChild(const std::string &name) const
{
  WidgetMap::const_iterator p = children_.find(name);

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

CTkAppPackLayout *
CTkWidget::
getTkPackLayout()
{
  QWidget *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  auto *l1 = qobject_cast<CTkAppPackLayout *>(l);

  if (! l1)
    l1 = new CTkAppPackLayout(w);

  return l1;
}

CTkAppGridLayout *
CTkWidget::
getTkGridLayout()
{
  QWidget *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  auto *l1 = qobject_cast<CTkAppGridLayout *>(l);

  if (! l1)
    l1 = new CTkAppGridLayout(w);

  return l1;
}

CTkAppPlaceLayout *
CTkWidget::
getTkPlaceLayout()
{
  QWidget *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  auto *l1 = qobject_cast<CTkAppPlaceLayout *>(l);

  if (! l1)
    l1 = new CTkAppPlaceLayout(w);

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
    auto c = stringToQColor(value);

    CQUtil::setBackground(qwidget_, c);
  }
  else if (name == "-foreground") {
    auto c = stringToQColor(value);

    CQUtil::setForeground(qwidget_, c);
  }
  else if (name == "-font") {
    // TODO
  }
  else
    return tk_->throwError("Invalid value name \"" + name + "\" "
                           "for " + std::string(getClassName()));

  return true;
}

std::string
CTkWidget::
iexec(const std::vector<std::string> &args)
{
  const auto &arg = args[0];

  (void) tk_->throwError("Invalid " + std::string(getClassName()) + " operation \"" + arg + "\"");

  return std::string();
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
  qwidget_->setWindowTitle(QString::fromStdString(title));
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

void
CTkWidget::
runCommand()
{
  if (getCommand() != "")
    tk_->eval(getCommand());
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
