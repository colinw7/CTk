#include <CTkAppWidget.h>
#include <CTkAppCommands.h>
#include <CTkApp.h>
#include <CTkAppPackLayout.h>
#include <CTkAppPlaceLayout.h>
#include <CTkAppGridLayout.h>
#include <CTkAppImage.h>
#include <CTkAppUtil.h>

#include <CQSlider.h>
#include <CQSpinList.h>
#include <CQLabelImage.h>

#include <CQUtil.h>
#include <CQImage.h>

#include <QApplication>
#include <QGroupBox>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QRadioButton>
#include <QScrollBar>
#include <QSplitter>
#include <QTextEdit>
#include <QToolButton>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

namespace {

bool stringToLineChar(const std::string &str, int &lineNum, int &charNum) {
  CStrParse parse(str);

  parse.skipSpace();

  if (! parse.readInteger(&lineNum))
    return false;

  if (parse.isChar('.')) {
    parse.skipChar();

    if (! parse.readInteger(&charNum))
      return false;
  }
  else
    charNum = -1;

  return true;
}

// set relief
void setFrameRelief(QWidget *w, const std::string &value) {
  auto *frame = qobject_cast<QFrame *>(w);
  auto *edit  = qobject_cast<QLineEdit *>(w);
  auto *text  = qobject_cast<QTextEdit *>(w);

  if      (frame) {
    if      (value == "raised") {
      frame->setFrameShadow(QFrame::Raised);
      frame->setFrameShape (QFrame::Panel);
    }
    else if (value == "sunken") {
      frame->setFrameShadow(QFrame::Sunken);
      frame->setFrameShape (QFrame::Panel);
    }
    else if (value == "flat") {
      frame->setFrameShadow(QFrame::Plain);
      frame->setFrameShape (QFrame::NoFrame);
    }
    else if (value == "ridge") {
      frame->setFrameShadow(QFrame::Raised);
      frame->setFrameShape (QFrame::Box);
    }
    else if (value == "solid") {
      frame->setFrameShadow(QFrame::Plain);
      frame->setFrameShape (QFrame::Panel);
    }
    else if (value == "groove") {
      frame->setFrameShadow(QFrame::Sunken);
      frame->setFrameShape (QFrame::Box);
    }
  }
  else if (edit) {
    edit->setFrame(true);
  }
  else if (text) {
    if      (value == "raised") {
      text->setFrameShadow(QFrame::Raised);
      text->setFrameShape (QFrame::Panel);
    }
    else if (value == "sunken") {
      text->setFrameShadow(QFrame::Sunken);
      text->setFrameShape (QFrame::Panel);
    }
    else if (value == "flat") {
      text->setFrameShadow(QFrame::Plain);
      text->setFrameShape (QFrame::NoFrame);
    }
    else if (value == "ridge") {
      text->setFrameShadow(QFrame::Raised);
      text->setFrameShape (QFrame::Box);
    }
    else if (value == "solid") {
      text->setFrameShadow(QFrame::Plain);
      text->setFrameShape (QFrame::Panel);
    }
    else if (value == "groove") {
      text->setFrameShadow(QFrame::Sunken);
      text->setFrameShape (QFrame::Box);
    }
  }
}

void setWidgetState(CTkAppWidget *w, const std::string &value)
{
  if      (value == "normal")
    w->qwidget()->setEnabled(true);
  else if (value == "disabled")
    w->qwidget()->setEnabled(false);
  else if (value == "active")
    std::cerr << "Unsupported state '" << value << "'\n";
  else
    std::cerr << "Invalid state '" << value << "'\n";
}

Qt::Alignment stringToAlign(const std::string &value)
{
  Qt::Alignment align = Qt::AlignCenter;

  if      (value == "n"     ) align = Qt::AlignTop;
  else if (value == "ne"    ) align = Qt::AlignTop    | Qt::AlignRight;
  else if (value == "e"     ) align =                   Qt::AlignRight;
  else if (value == "se"    ) align = Qt::AlignBottom | Qt::AlignRight;
  else if (value == "s"     ) align = Qt::AlignBottom;
  else if (value == "sw"    ) align = Qt::AlignBottom | Qt::AlignLeft;
  else if (value == "w"     ) align =                   Qt::AlignLeft;
  else if (value == "nw"    ) align = Qt::AlignTop    | Qt::AlignLeft;
  else if (value == "center") align = Qt::AlignCenter;
  else std::cerr << "Invalid align string '" << value << "'\n";

  return align;
}

Qt::Alignment stringToJustify(const std::string &value)
{
  Qt::Alignment align = Qt::AlignCenter;

  if      (value == "left"  ) align = Qt::AlignLeft;
  else if (value == "right" ) align = Qt::AlignRight;
  else if (value == "center") align = Qt::AlignHCenter;
  else std::cerr << "Invalid justify string '" << value << "'\n";

  return align;
}

std::string underlineLabel(const std::string &label, long pos) {
  auto label1 = label;

  if (pos >= 0 && pos < long(label1.size()))
    label1 = label1.substr(0, pos) + "&" + label1.substr(pos);

  return label1;
}

}

//---

CTkAppRoot::
CTkAppRoot(CTkApp *tk) :
 CTkAppWidget(tk, nullptr, "")
{
  qroot_ = new CTkAppRootWidget(this);

  setQWidget(qroot_);
}

void
CTkAppRoot::
setRoot(QFrame *qroot)
{
  delete qroot_;

  qroot_ = qroot;

  setQWidget(qroot_);
}

void
CTkAppRoot::
show()
{
  if (! qroot_->isVisible()) {
    auto s = qroot_->sizeHint();

    qroot_->resize(s.expandedTo(QApplication::globalStrut()));
  }

  CTkAppWidget::show();
}

bool
CTkAppRoot::
execConfig(const std::string &name, const std::string &value)
{
  if (name == "-menu") {
    menuName_ = value;

    auto *w = tk_->lookupWidgetByName(menuName_, /*quiet*/true);

    auto *menu = dynamic_cast<CTkAppMenu *>(w);

    if (menu) {
      auto *rootWidget = qobject_cast<CTkAppRootWidget *>(qroot_);

      if (rootWidget)
        rootWidget->setMenu(menu->qmenu());
    }
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppRoot::
execOp(const Args &args)
{
  return CTkAppWidget::execOp(args);
}

bool
CTkAppRoot::
decodeWidgetName(const std::string &name, CTkAppWidget **parent, std::string &childName) const
{
  uint len = name.size();

  if (len == 0) return false;

  if (name[0] != '.') return false;

  if (len == 1) {
    *parent = const_cast<CTkAppRoot *>(this);

    childName = "";

    return true;
  }

  CTkAppWidget* parent1 = const_cast<CTkAppRoot *>(this);
  CTkAppWidget* child1  = nullptr;

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

CTkAppRootWidget::
CTkAppRootWidget(CTkAppRoot *root) :
 QFrame(nullptr), root_(root)
{
  setObjectName("qroot");
}

void
CTkAppRootWidget::
setMenu(QMenu *menu)
{
  menu_ = menu;

  if (! menuBar_)
    menuBar_ = new QMenuBar(this);

  for (auto *a : menu_->actions()) {
    if (a->menu())
      menuBar_->addMenu(a->menu());
  }

  QFontMetrics fm(font());

  setContentsMargins(0, fm.height(), 0, 0);

  menuBar_->move(0, 0);
  menuBar_->resize(width(), fm.height());
}

void
CTkAppRootWidget::
paintEvent(QPaintEvent *e)
{
  QFrame::paintEvent(e);

//QPainter painter(this);
//auto *pack = root_->getTkPackLayout();
//if (pack) pack->draw(&painter);

  QPainter painter(this);
  auto *grid = root_->getTkGridLayout(/*create*/false);
  if (grid) grid->draw(&painter);
}

QSize
CTkAppRootWidget::
sizeHint() const
{
  auto s = QFrame::sizeHint();

  if (s.width() <= 0 || s.height() <= 0)
    s = QSize(200, 200);

  return s;
}

//----------

CTkAppButton::
CTkAppButton(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qbutton_ = new CTkAppButtonWidget(this);

  setQWidget(qbutton_);

  connectSlots(true);
}

void
CTkAppButton::
connectSlots(bool b)
{
  if (b)
    connect(qbutton_, SIGNAL(clicked()), this, SLOT(clickSlot()));
  else
    disconnect(qbutton_, SIGNAL(clicked()), this, SLOT(clickSlot()));
}

bool
CTkAppButton::
execConfig(const std::string &name, const std::string &value)
{
  long underlinePos = -1;

  if      (name == "-command") {
    setCommand(value);
  }
  else if (name == "-default") {
    if      (value == "normal")
      tk_->TODO(name);
    else if (value == "active")
      tk_->TODO(name);
    else if (value == "disabled")
      tk_->TODO(value);
    else
      return false;
  }
  else if (name == "-height") {
    // height of image (pixels) or text (chars)
    double h;

    if (! CTkAppUtil::stringToDistance(value, h))
      return tk_->throwError("Invalid height \"" + value + "\"");

    qbutton_->setHeight(h);
  }
  else if (name == "-overrelief") {
    if (value == "raised")
      tk_->TODO(name);
    else
      tk_->TODO(value);
  }
  else if (name == "-state") {
    if      (value == "normal") {
      qbutton_->setEnabled(true);
      qbutton_->setDown(false);
    }
    else if (value == "active")
      qbutton_->setDown(true);
    else if (value == "disabled")
      qbutton_->setEnabled(false);
    else
      return false;
  }
  else if (name == "-width") {
    // width of image (pixels) or text (chars)
    double w;

    if (! CTkAppUtil::stringToDistance(value, w))
      return tk_->throwError("Invalid width \"" + value + "\"");

    qbutton_->setWidth(w);
  }
  else if (name == "-text") {
    qbutton_->setText(QString::fromStdString(value));
  }
  else if (name == "-bitmap") {
    auto image = tk_->getBitmap(value);

    if (! image)
      return tk_->throwError("Failed to load bitmap '" + value + "'");

    auto *cqimage = image.cast<CQImage>();
    if (! cqimage) return false;

    auto &qimage = cqimage->getQImage();

    setImage(qimage);
  }
  else if (name == "-image") {
    auto image = tk_->getImage(value);

    if (image)
      setImage(image->getQImage());
  }
  else if (name == "-wraplength") {
    double length;
    if (! CTkAppUtil::stringToDistance(value, length))
      return tk_->throwError("Invalid width \"" + value + "\"");
    qbutton_->setWrapLength(length);
  }
  else if (name == "-underline") {
    if (! tk_->getOptionInt(name, value, underlinePos))
      return false;
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppButton::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "flash") {
    flash();
  }
  else if (arg == "invoke") {
    runCommand();
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppButton::
setImage(const QImage &qimage)
{
  QPixmap pixmap;

  pixmap.convertFromImage(qimage);

  qbutton_->setIcon(QIcon(pixmap));
}

void
CTkAppButton::
clickSlot()
{
  runCommand();
}

void
CTkAppButton::
flash()
{
  tk_->TODO();
}

//---

CTkAppButtonWidget::
CTkAppButtonWidget(CTkAppButton *button) :
 QPushButton(button->getParent() ? button->getParent()->getQWidget() : nullptr), button_(button)
{
}

void
CTkAppButtonWidget::
setText(const QString &s)
{
  text_ = s;

  updateText();
}

void
CTkAppButtonWidget::
setWrapLength(double r)
{
  wrapLength_ = r;

  updateText();
}

void
CTkAppButtonWidget::
updateText()
{
  if (wrapLength_ > 0) {
    QFontMetrics fm(font());

    auto tw = fm.horizontalAdvance(text_);

    if (tw > wrapLength_) {
      QStringList strs;

      auto text = text_;

      while (text.length() > 0) {
        auto str = text.left(1);

        text = text.mid(1);

        tw = fm.horizontalAdvance(str);

        while (text.length() > 0 && tw < wrapLength_) {
          str += text.left(1);

          text = text.mid(1);

          tw = fm.horizontalAdvance(str);
        }

        strs << str;
      }

      text = strs.join("\n");

      QPushButton::setText(text);
    }
    else
      QPushButton::setText(text_);
  }
  else
    QPushButton::setText(text_);
}

//----------

CTkAppCanvas::
CTkAppCanvas(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qcanvas_ = new CTkAppCanvasWidget(this);

  setQWidget(qcanvas_);

  qwidget_->setFocusPolicy(Qt::StrongFocus);
}

bool
CTkAppCanvas::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-closeenough") {
    tk_->TODO(name);
  }
  else if (name == "-confine") {
    tk_->TODO(name);
  }
  else if (name == "-height") {
    double height;
    if (! CTkAppUtil::stringToDistance(value, height))
      return tk_->throwError("Invalid width \"" + value + "\"");
    setHeight(height);
  }
  else if (name == "-scrollregion") {
    tk_->TODO(name);
  }
  else if (name == "-state") {
    tk_->TODO(name);
  }
  else if (name == "-width") {
    double width;
    if (! CTkAppUtil::stringToDistance(value, width))
      return tk_->throwError("Invalid width \"" + value + "\"");
    setWidth(width);
  }
  else if (name == "-xscrollincrement") {
    tk_->TODO(name);
  }
  else if (name == "-yscrollincrement") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppCanvas::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  auto setShapeOpt = [&](CTkAppCanvasWidget::Shape *shape, const std::string &name,
                         const std::string &value) {
    if      (name == "-anchor") {
      auto align = stringToAlign(value);

      shape->setAlign(align);
    }
    else if (name == "-dash") {
      tk_->TODO(name);
    }
    else if (name == "-activedash") {
      tk_->TODO(name);
    }
    else if (name == "-disabledash") {
      tk_->TODO(name);
    }
    else if (name == "-dashoffset") {
      tk_->TODO(name);
    }
    else if (name == "-fill") {
      auto c = CTkAppUtil::stringToQColor(value);

      auto b = shape->brush();
      b.setStyle(Qt::SolidPattern);
      b.setColor(c);
      shape->setBrush(b);
    }
    else if (name == "-activefill") {
      tk_->TODO(name);
    }
    else if (name == "-disabledfill") {
      tk_->TODO(name);
    }
    else if (name == "-outline") {
      auto c = CTkAppUtil::stringToQColor(value);

      auto p = shape->pen();
      p.setColor(c);
      shape->setPen(p);
    }
    else if (name == "-activeoutline") {
      tk_->TODO(name);
    }
    else if (name == "-disabledoutline") {
      tk_->TODO(name);
    }
    else if (name == "-offset") {
      tk_->TODO(name);
    }
    else if (name == "-outlinestipple") {
      tk_->TODO(name);
    }
    else if (name == "-activeoutlinestipple") {
      tk_->TODO(name);
    }
    else if (name == "-disabledoutlinestipple") {
      tk_->TODO(name);
    }
    else if (name == "-outlineoffset") {
      tk_->TODO(name);
    }
    else if (name == "-stipple") {
      tk_->TODO(name);
    }
    else if (name == "-activestipple") {
      tk_->TODO(name);
    }
    else if (name == "-disabledstipple") {
      tk_->TODO(name);
    }
    else if (name == "-state") {
      tk_->TODO(name);
    }
    else if (name == "-tags" || name == "-tag") {
      std::vector<std::string> strs;

      if (! tk_->splitList(value, strs))
        return tk_->throwError("Invalid tags \"" + value + "\"");

      shape->setTags(strs);
    }
    else if (name == "-width") {
      double width;

      if (! CTkAppUtil::stringToDistance(value, width))
        return tk_->throwError("Invalid width \"" + value + "\"");

      auto p = shape->pen();
      p.setWidthF(width);
      shape->setPen(p);
    }
    else if (name == "-activewidth") {
      tk_->TODO(name);
    }
    else if (name == "-disabledwidth") {
      tk_->TODO(name);
    }
    else
      return false;

    return true;
  };

  auto getShapeOpt = [&](CTkAppCanvasWidget::Shape *shape, const std::string &name,
                         std::string &value) {
    if      (name == "-fill") {
      auto b = shape->brush();
      value = b.color().name().toStdString();
    }
    else if (name == "-outline") {
      auto p = shape->pen();
      value = p.color().name().toStdString();
    }
    else if (name == "-width") {
      auto p = shape->pen();
      value = std::to_string(p.widthF());
    }
    else if (name == "-tags" || name == "-tag") {
      value = tk_->mergeList(shape->tags());
    }
    else
      return false;

    return true;
  };

  //---

  const auto &arg = args[0];

  if      (arg == "addtag") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " addtag tag searchCommand ?arg ...?");

//  const auto &tag = args[1];
    const auto &cmd = args[2];

    if      (cmd == "above") {
      tk_->TODO(cmd);
    }
    else if (cmd == "all") {
      tk_->TODO(cmd);
    }
    else if (cmd == "below") {
      tk_->TODO(cmd);
    }
    else if (cmd == "closest") {
      tk_->TODO(cmd);
    }
    else if (cmd == "enclosed") {
      tk_->TODO(cmd);
    }
    else if (cmd == "overlapping") {
      tk_->TODO(cmd);
    }
    else if (cmd == "withtag") {
      tk_->TODO(cmd);
    }
    else
      return tk_->throwError("bad search command \"" + cmd + "\": must be "
               "above, all, below, closest, enclosed, overlapping, or withtag");
  }
  else if (arg == "bbox") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " bbox tagOrId ?tagOrId ...?");

    tk_->TODO(arg);
  }
  else if (arg == "bind") {
    if (numArgs < 2 || numArgs > 4)
      return tk_->wrongNumArgs(getName() + " bind tagOrId ?sequence? ?command?");

    auto id = args[1];

    std::string sequence;

    if (numArgs > 2)
      sequence = args[2];

    CTkAppEventData data;

    if (sequence != "" && ! tk_->parseEvent(sequence, data))
      return tk_->throwError("Invalid event \"" + sequence + "\"");

    if (numArgs > 3)
      data.command = args[3];

    idEventDatas_[id].push_back(data);
  }
  else if (arg == "canvasx") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " canvasx screenx ?gridspacing?");

    tk_->TODO(arg);
  }
  else if (arg == "canvasy") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " canvasy screenx ?gridspacing?");

    tk_->TODO(arg);
  }
  else if (arg == "coords") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " coords tagOrId ?x y x y ...?");

    auto id = args[1];

    CTkAppCanvasWidget::Points points;

    if (numArgs > 2) {
      for (uint i = 2; i < numArgs; i += 2) {
        if (args[i][0] == '-')
          break;

        double x, y;
        if (! CTkAppUtil::stringToDistance(args[i], x) ||
            ! CTkAppUtil::stringToDistance(args[i + 1], y))
          return tk_->throwError("Invalid coord \"" + args[i] + " " + args[i + 1] + "\"");

        points.push_back(CTkAppCanvasWidget::Point(x, y));
      }

      if (points.empty())
        return false;

      if (! qcanvas_->setShapePoints(id, points))
        return false;
    }
    else {
      if (! qcanvas_->getShapePoints(id, points))
        return false;

      if (points.empty())
        return false;

      std::vector<std::string> strs;

      for (const auto &p : points) {
        strs.push_back(CStrUtil::toString(p.x));
        strs.push_back(CStrUtil::toString(p.y));
      }

      tk_->setStringArrayResult(strs);
    }
  }
  else if (arg == "create") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " create type coords ?arg ...?");

    static auto typeNames = std::vector<std::string>({
      "arc", "bitmap", "image", "line", "oval", "polygon", "rectangle", "text", "window" });

    std::string type;
    if (! tk_->lookupName("type", typeNames, args[1], type))
      return false;

    if      (type == "arc") {
      CTkAppCanvasWidget::Points points;

      uint i = 2;

      for (int ip = 0; ip < 2; ++ip) {
        double x, y;
        if (! CTkAppUtil::stringToDistance(args[i    ], x) ||
            ! CTkAppUtil::stringToDistance(args[i + 1], y))
          return tk_->throwError("Invalid coord \"" + args[i] + " " + args[i + 1] + "\"");

        points.push_back(CTkAppCanvasWidget::Point(x, y));

        i += 2;
      }

      if (points.size() != 2)
        return tk_->wrongNumArgs(getName() + " create arc coords ?arg ...?");

      auto *arc = qcanvas_->addArc(points);

      long start  = 0;
      long extent = 360;

      auto arcType = CTkAppCanvasWidget::ArcType::ARC;

      for ( ; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (setShapeOpt(arc, name, value))
            continue;

          if      (name == "-extent") {
            if (! CTkAppUtil::stringToInt(value, extent))
              return false;
          }
          else if (name == "-start") {
            if (! CTkAppUtil::stringToInt(value, start))
              return false;
          }
          else if (name == "-style") {
            // pieslice, chord, arc
            if      (value == "pieslice")
              arcType = CTkAppCanvasWidget::ArcType::PIE;
            else if (value == "chord")
              arcType = CTkAppCanvasWidget::ArcType::CHORD;
            else if (value == "arc")
              arcType = CTkAppCanvasWidget::ArcType::ARC;
            else
              tk_->TODO(name + "/" + value);
          }
          else
            tk_->TODO(name);
        }
        else
          tk_->TODO(args[i]);
      }

      arc->setStart (int(start ));
      arc->setExtent(int(extent));

      arc->setArcType(arcType);

      tk_->setIntegerResult(arc->id());
    }
    else if (type == "bitmap") {
      uint i = 2;

      double x, y;
      if (! CTkAppUtil::stringToDistance(args[i    ], x) ||
          ! CTkAppUtil::stringToDistance(args[i + 1], y))
        return tk_->throwError("Invalid coord \"" + args[i] + " " + args[i + 1] + "\"");

      auto *bitmap = qcanvas_->addBitmap(CTkAppCanvasWidget::Point(x, y));

      i += 2;

      for ( ; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (setShapeOpt(bitmap, name, value))
            continue;

          if      (name == "-background") {
            tk_->TODO(name);
          }
          else if (name == "-activebackground") {
            tk_->TODO(name);
          }
          else if (name == "-activeforeground") {
            tk_->TODO(name);
          }
          else if (name == "-disabledbackground") {
            tk_->TODO(name);
          }
          else if (name == "-disabledforeground") {
            tk_->TODO(name);
          }
          else if (name == "-bitmap") {
            auto image = tk_->getBitmap(value);

            bitmap->setImage(image);
          }
          else if (name == "-activebitmap") {
            tk_->TODO(name);
          }
          else if (name == "-disabledbitmap") {
            tk_->TODO(name);
          }
          else if (name == "-foreground") {
            tk_->TODO(name);
          }
          else
            tk_->TODO(name);
        }
        else
          tk_->TODO(args[i]);
      }

      tk_->setIntegerResult(bitmap->id());
    }
    else if (type == "image") {
      uint i = 2;

      double x, y;
      if (! CTkAppUtil::stringToDistance(args[i    ], x) ||
          ! CTkAppUtil::stringToDistance(args[i + 1], y))
        return tk_->throwError("Invalid coord \"" + args[i] + " " + args[i + 1] + "\"");

      auto *image = qcanvas_->addImage(CTkAppCanvasWidget::Point(x, y));

      i += 2;

      for ( ; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (setShapeOpt(image, name, value))
            continue;

          if      (name == "-image") {
            auto appImage = tk_->getImage(value);

            image->setImage(appImage->getImage());
          }
          else if (name == "-activeimage") {
            tk_->TODO(name);
          }
          else if (name == "-disabledimage") {
            tk_->TODO(name);
          }
          else
            tk_->TODO(name);
        }
        else
          tk_->TODO(args[i]);
      }

      tk_->setIntegerResult(image->id());
    }
    else if (type == "line") {
      CTkAppCanvasWidget::Points points;

      uint i = 2;

      for ( ; i < numArgs; i += 2) {
        if (args[i][0] == '-')
          break;

        double x, y;
        if (! CTkAppUtil::stringToDistance(args[i    ], x) ||
            ! CTkAppUtil::stringToDistance(args[i + 1], y))
          return tk_->throwError("Invalid coord \"" + args[i] + " " + args[i + 1] + "\"");

        points.push_back(CTkAppCanvasWidget::Point(x, y));
      }

      if (points.empty())
        return tk_->wrongNumArgs(getName() + " create line coords ?arg ...?");

      auto *line = qcanvas_->addLine(points);

      for ( ; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (setShapeOpt(line, name, value))
            continue;

          if      (name == "-arrow") {
            auto arrowType = line->arrowType();

            if      (value == "first")
              arrowType = CTkAppCanvasWidget::ArrowType::FIRST;
            else if (value == "last")
              arrowType = CTkAppCanvasWidget::ArrowType::LAST;
            else if (value == "both")
              arrowType = CTkAppCanvasWidget::ArrowType::BOTH;
            else if (value == "none")
              arrowType = CTkAppCanvasWidget::ArrowType::NONE;
            else
              tk_->TODO(name + "/" + value);

            line->setArrowType(arrowType);
          }
          else if (name == "-arrowshape") {
            tk_->TODO(name);
          }
          else if (name == "-capstyle") {
            tk_->TODO(name);
          }
          else if (name == "-joinstyle") {
            tk_->TODO(name);
          }
          else if (name == "-smooth") {
            tk_->TODO(name);
          }
          else if (name == "-splinesteps") {
            tk_->TODO(name);
          }
          else
            tk_->TODO(name);
        }
        else
          tk_->TODO(args[i]);
      }

      tk_->setIntegerResult(line->id());
    }
    else if (type == "oval") {
      if (numArgs < 6)
        return tk_->wrongNumArgs(getName() + " create oval coords ?arg ...?");

      double x1, y1, x2, y2;
      if (! CTkAppUtil::stringToDistance(args[2], x1) ||
          ! CTkAppUtil::stringToDistance(args[3], y1) ||
          ! CTkAppUtil::stringToDistance(args[4], x2) ||
          ! CTkAppUtil::stringToDistance(args[5], y2))
        return tk_->throwError("Invalid coords \"" + args[2] + " " + args[3] + " " +
                               args[4] + " " + args[5] + "\"");

      auto *oval = qcanvas_->addOval(x1, y1, x2, y2);

      for (uint i = 6; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (setShapeOpt(oval, name, value))
            continue;

          tk_->TODO(name);
        }
        else
          tk_->TODO(args[i]);
      }

      tk_->setIntegerResult(oval->id());
    }
    else if (type == "polygon") {
      CTkAppCanvasWidget::Points points;

      uint i = 2;

      for ( ; i < numArgs; i += 2) {
        if (args[i][0] == '-')
          break;

        double x, y;
        if (! CTkAppUtil::stringToDistance(args[i    ], x) ||
            ! CTkAppUtil::stringToDistance(args[i + 1], y))
          return tk_->throwError("Invalid coord \"" + args[i] + " " + args[i + 1] + "\"");

        points.push_back(CTkAppCanvasWidget::Point(x, y));
      }

      if (points.size() < 3)
        return tk_->wrongNumArgs(getName() + " create polygon coords ?arg ...?");

      auto *polygon = qcanvas_->addPolygon(points);

      for ( ; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (setShapeOpt(polygon, name, value))
            continue;

          tk_->TODO(name);
        }
        else
          tk_->TODO(args[i]);
      }

      tk_->setIntegerResult(polygon->id());
    }
    else if (type == "rectangle") {
      if (numArgs < 6)
        return tk_->wrongNumArgs(getName() + " create rectangle coords ?arg ...?");

      double x1, y1, x2, y2;
      if (! CTkAppUtil::stringToDistance(args[2], x1) ||
          ! CTkAppUtil::stringToDistance(args[3], y1) ||
          ! CTkAppUtil::stringToDistance(args[4], x2) ||
          ! CTkAppUtil::stringToDistance(args[5], y2))
        return tk_->throwError("Invalid coords \"" + args[2] + " " + args[3] + " " +
                               args[4] + " " + args[5] + "\"");

      auto *rectangle = qcanvas_->addRectangle(x1, y1, x2, y2);

      for (uint i = 6; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (setShapeOpt(rectangle, name, value))
            continue;

          if      (name == "-joinstyle") {
            tk_->TODO(name);
          }
          else if (name == "-smooth") {
            tk_->TODO(name);
          }
          else if (name == "-splinesteps") {
            tk_->TODO(name);
          }
          else
            tk_->TODO(name);
        }
        else
          tk_->TODO(args[i]);
      }

      tk_->setIntegerResult(rectangle->id());
    }
    else if (type == "text") {
      if (numArgs < 4)
        return tk_->wrongNumArgs(getName() + " create text coord ?arg ...?");

      double x, y;
      if (! CTkAppUtil::stringToDistance(args[2], x) ||
          ! CTkAppUtil::stringToDistance(args[3], y))
        return tk_->throwError("Invalid coord \"" + args[2] + " " + args[3] + "\"");

      CTkAppCanvasWidget::Point pos(x, y);

      long underlinePos = -1;

      auto *text = qcanvas_->addText(pos, "");

      for (uint i = 4; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (setShapeOpt(text, name, value))
            continue;

          if      (name == "-angle") {
            tk_->TODO(name);
          }
          else if (name == "-font") {
            auto f = CTkAppUtil::stringToQFont(value);

            text->setFont(f);
          }
          else if (name == "-justify") {
            auto align = stringToJustify(value);

            text->setJustify(align);
          }
          else if (name == "-text") {
            text->setText(value);
          }
          else if (name == "-underline") {
            if (! tk_->getOptionInt(name, value, underlinePos))
              return false;
          }
          else if (name == "-width") {
            tk_->TODO(name);
          }
          else
            tk_->TODO(name);
        }
        else
          tk_->TODO(args[i]);
      }

      tk_->setIntegerResult(text->id());
    }
    else if (type == "window") {
      tk_->TODO(type);

      for (uint i = 1; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
        //const auto &value = args[i + 1];

          if      (name == "-height") {
            tk_->TODO(name);
          }
          else if (name == "-width") {
            tk_->TODO(name);
          }
          else if (name == "-window") {
            tk_->TODO(name);
          }
          else
            tk_->TODO(name);
        }
        else
          tk_->TODO(args[i]);
      }
    }
    else
      tk_->TODO(type);

    qcanvas_->update();
  }
  else if (arg == "dchars") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " dchars tagOrId first ?last?");

    tk_->TODO(arg);
  }
  else if (arg == "delete") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " delete ?arg ...?"); // delete all ?

    const auto &name = args[1];

    if (name == "all") {
      qcanvas_->deleteAllShapes();

      qcanvas_->update();
    }
    else {
      if (! qcanvas_->deleteShape(name))
        return false;

      qcanvas_->update();
    }
  }
  else if (arg == "dtag") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " dtag tagOrId ?tagToDelete?");

    tk_->TODO(arg);
  }
  else if (arg == "find") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " find searchCommand ?arg ...?");

    const auto &cmd = args[1];

    if      (cmd == "above") {
      tk_->TODO(cmd);
    }
    else if (cmd == "all") {
      tk_->TODO(cmd);
    }
    else if (cmd == "below") {
      tk_->TODO(cmd);
    }
    else if (cmd == "closest") {
      tk_->TODO(cmd);
    }
    else if (cmd == "enclosed") {
      tk_->TODO(cmd);
    }
    else if (cmd == "overlapping") {
      tk_->TODO(cmd);
    }
    else if (cmd == "withtag") {
      if (numArgs < 3)
        return tk_->wrongNumArgs(getName() + " find withtag tagOrId");

      const auto &id = args[2];

      if      (id == "current") {
        if (insideShape_)
          tk_->setIntegerResult(insideShape_->id());
        else
          tk_->setStringResult("");
      }
      else if (id == "closest") {
        auto pos = qcanvas_->mapFromGlobal(QCursor::pos());

        int nearestId = qcanvas_->findNearest(pos.x(), pos.y());

        tk_->setIntegerResult(nearestId);
      }
      else
        tk_->TODO(arg + " " + cmd + " " + id);
    }
    else
      return tk_->throwError("bad search command \"" + cmd + "\": must be "
                             "above, all, below, closest, enclosed, overlapping, or withtag");
  }
  else if (arg == "focus") {
    tk_->TODO(arg);
  }
  else if (arg == "gettags") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " gettags tagOrId");

    tk_->TODO(arg);
  }
  else if (arg == "icursor") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " icursor tagOrId index");

    tk_->TODO(arg);
  }
  else if (arg == "imove") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " imove tagOrId index x y");

    tk_->TODO(arg);
  }
  else if (arg == "index") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " index tagOrId string");

    tk_->TODO(arg);
  }
  else if (arg == "insert") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " insert tagOrId beforeThis string");

    tk_->TODO(arg);
  }
  else if (arg == "itemcget") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " itemcget tagOrId option");

    const auto &id = args[1];

    CTkAppCanvasWidget::Shapes shapes;

    if (id == "current")
      shapes.push_back(insideShape_);
    else {
      if (! qcanvas_->getShapes(id, shapes))
        return tk_->throwError("No matching shape \"" + id + "\" for itemconfigure");
    }

    std::vector<std::string> values;

    for (uint i = 2; i < numArgs; ++i) {
      const auto &name = args[i];

      if (name[0] == '-') {
        bool rc = false;

        for (auto *shape : shapes) {
          std::string value;

          if (getShapeOpt(shape, name, value)) {
            values.push_back(value);
            rc = true;
          }
        }

        if (! rc)
          tk_->TODO(name);
      }
      else
        tk_->TODO(name);
    }

    tk_->setStringArrayResult(values);
  }
  else if (arg == "itemconfigure") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " itemconfigure tagOrId ?-option value ...?");

    const auto &id = args[1];

    CTkAppCanvasWidget::Shapes shapes;

    if (id == "current")
      shapes.push_back(insideShape_);
    else {
      if (! qcanvas_->getShapes(id, shapes))
        return tk_->throwError("No matching shape \"" + id + "\" for itemconfigure");
    }

    for (uint i = 2; i < numArgs; i += 2) {
      const auto &name = args[i];

      if (name[0] == '-') {
        const auto &value = args[i + 1];

        bool rc = false;

        for (auto *shape : shapes) {
          if (setShapeOpt(shape, name, value))
            rc = true;
        }

        if (! rc)
          tk_->TODO(name);
      }
      else
        tk_->TODO(name);
    }
  }
  else if (arg == "lower") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " lower tagOrId ?belowThis?");

    const auto &name = args[1];

    if (! qcanvas_->lowerShape(name))
      return false;
  }
  else if (arg == "move") {
    if (numArgs != 4)
      return tk_->wrongNumArgs(getName() + " move tagOrId xAmount yAmount");

    const auto &id = args[1];

    double dx, dy;
    if (! CTkAppUtil::stringToDistance(args[2], dx) ||
        ! CTkAppUtil::stringToDistance(args[3], dy))
      return tk_->throwError("Invalid coord \"" + args[2] + " " + args[3] + "\"");

    if (! qcanvas_->moveShape(id, dx, dy))
      return false;
  }
  else if (arg == "moveto") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " moveto tagOrId x y");

    tk_->TODO(arg);
  }
  else if (arg == "postscript") {
    tk_->TODO(arg);
  }
  else if (arg == "raise") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " raise tagOrId ?belowThis?");

    const auto &name = args[1];

    if (! qcanvas_->raiseShape(name))
      return false;
  }
  else if (arg == "rchars") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " rchars tagOrId first last string");

    tk_->TODO(arg);
  }
  else if (arg == "scale") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " scale tagOrId xOrigin yOrigin xScale yScale");

    tk_->TODO(arg);
  }
  else if (arg == "scan") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " scan mark|dragto x y ?dragGain?");

    tk_->TODO(arg);
  }
  else if (arg == "select") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " select option ?tagOrId? ?arg?");

    tk_->TODO(arg);
  }
  else if (arg == "type") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " type tag");

    tk_->TODO(arg);
  }
  else if (arg == "xview") {
    tk_->TODO(arg);
    tk_->setStringResult("0.0 1.0");
  }
  else if (arg == "yview") {
    tk_->TODO(arg);
    tk_->setStringResult("0.0 1.0");
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

bool
CTkAppCanvas::
triggerMousePressEvents(QEvent *e, int button)
{
  if (insideShape_) {
    auto *me = static_cast<QMouseEvent *>(e);

    CTkAppEventData matchEventData;

    tk_->encodeEvent(me, CTkAppEventMode::Press, button, matchEventData);

    processShapeEvents(insideShape_, e, matchEventData);
  }

  return CTkAppWidget::triggerMousePressEvents(e, button);
}

bool
CTkAppCanvas::
triggerMouseMoveEvents(QEvent *e, int button, bool pressed)
{
  if (! pressed) {
    auto *me = static_cast<QMouseEvent *>(e);

    auto *newInsideShape = qcanvas_->insideShape(me->x(), me->y());
    auto *oldInsideShape = insideShape_;

    if (newInsideShape != oldInsideShape) {
      // Enter new inside shape
      if (newInsideShape) {
        //std::cerr << "Enter " << newInsideShape->id() << "\n";

        insideShape_ = newInsideShape;

        for (auto &tag : newInsideShape->tags()) {
          auto pi = idEventDatas_.find(tag);
          if (pi == idEventDatas_.end()) continue;

          for (const auto &eventData : (*pi).second) {
            if (eventData.type == CTkAppEventType::Enter) {
              tk_->execEvent(this, e, eventData, eventData.command);
            }
          }
        }
      }

      // Leave old inside shape
      if (oldInsideShape) {
        //std::cerr << "Leave " << oldInsideShape->id() << "\n";

        for (auto &tag : oldInsideShape->tags()) {
          auto pi = idEventDatas_.find(tag);
          if (pi == idEventDatas_.end()) continue;

          for (const auto &eventData : (*pi).second) {
            if (eventData.type == CTkAppEventType::Leave) {
              tk_->execEvent(this, e, eventData, eventData.command);
            }
          }
        }

        insideShape_ = newInsideShape;
      }
    }
  }
  else {
    if (insideShape_) {
      auto *me = static_cast<QMouseEvent *>(e);

      CTkAppEventData matchEventData;

      tk_->encodeEvent(me, CTkAppEventMode::Motion, button, matchEventData);

      processShapeEvents(insideShape_, e, matchEventData);
    }
  }

  return CTkAppWidget::triggerMouseMoveEvents(e, button, pressed);
}

void
CTkAppCanvas::
processShapeEvents(Shape *shape, QEvent *e, const CTkAppEventData &matchEventData)
{
  for (auto &tag : shape->tags()) {
    auto pi = idEventDatas_.find(tag);
    if (pi == idEventDatas_.end()) continue;

    for (const auto &eventData : (*pi).second) {
      if (eventData == matchEventData) {
        tk_->execEvent(this, e, matchEventData, eventData.command);
      }
    }
  }
}

//---

CTkAppCanvasWidget::
CTkAppCanvasWidget(CTkAppCanvas *canvas) :
 QWidget(canvas->getParent() ? canvas->getParent()->getQWidget() : nullptr), canvas_(canvas)
{
  setObjectName("qcanvas");

  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);
}

void
CTkAppCanvasWidget::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  for (auto *s : shapes_)
    drawShape(&p, s);
}

void
CTkAppCanvasWidget::
drawShape(QPainter *p, Shape *s)
{
  p->setPen  (s->pen());
  p->setBrush(s->brush());

  switch (s->type()) {
    case CTkAppCanvasShapeType::RECTANGLE: {
      auto *r = static_cast<Rectangle *>(s);
      p->drawRect(QRectF(r->x1(), r->y1(), r->x2() - r->x1(), r->y2() - r->y1()));
      break;
    }
    case CTkAppCanvasShapeType::OVAL: {
      auto *o = static_cast<Oval *>(s);
      p->drawEllipse(QRectF(o->x1(), o->y1(), o->x2() - o->x1(), o->y2() - o->y1()));
      break;
    }
    case CTkAppCanvasShapeType::LINE: {
      auto *l = static_cast<Line *>(s);
      const auto &points = l->points();
      auto n = points.size();

      QPainterPath path;
      for (uint i = 0; i < n; ++i) {
        const auto &point = points[i];

        if (i == 0)
          path.moveTo(point.x, point.y);
        else
          path.lineTo(point.x, point.y);
      }
      p->drawPath(path);
      break;
    }
    case CTkAppCanvasShapeType::POLYGON: {
      auto *l = static_cast<Line *>(s);
      const auto &points = l->points();
      auto n = points.size();

      QPainterPath path;
      for (uint i = 0; i < n; ++i) {
        const auto &point = points[i];

        if (i == 0)
          path.moveTo(point.x, point.y);
        else
          path.lineTo(point.x, point.y);
      }
      path.closeSubpath();
      p->drawPath(path);
      break;
    }
    case CTkAppCanvasShapeType::ARC: {
      auto *a = static_cast<Arc *>(s);
      const auto &points = a->points();
      assert(points.size() == 2);
      QRectF r(points[0].x, points[0].y, points[1].x - points[0].x, points[1].y - points[0].y);

      QPainterPath path;
      path.arcMoveTo(r, a->start());
      path.arcTo(r, a->start(), a->extent());
      p->drawPath(path);
      break;
    }
    case CTkAppCanvasShapeType::TEXT: {
      auto *t = static_cast<Text *>(s);
      auto text = QString::fromStdString(t->text());
      QFontMetricsF fm(font());
      auto tw = fm.horizontalAdvance(text);
      auto tx = t->pos().x;
      auto ty = t->pos().y;
      double dtx = 0.0;
      double dty = 0.0;
      if      (s->align() & Qt::AlignLeft   ) { dtx = 0.0; }
      else if (s->align() & Qt::AlignHCenter) { dtx = -tw/2.0; }
      else if (s->align() & Qt::AlignRight  ) { dtx = -tw; }
      if      (s->align() & Qt::AlignTop    ) { dty = fm.ascent(); }
      else if (s->align() & Qt::AlignVCenter) { dty = (fm.ascent() - fm.descent())/2.0; }
      else if (s->align() & Qt::AlignBottom ) { dty = -fm.descent(); }
      p->drawText(tx + dtx, ty + dty, QString::fromStdString(t->text()));
      break;
    }
    case CTkAppCanvasShapeType::IMAGE: {
      auto *i = static_cast<Image *>(s);

      auto *cqimage = i->getImage().cast<CQImage>();
      if (! cqimage) return;

      auto &qimage = cqimage->getQImage();
      p->drawImage(i->pos().x, i->pos().y, qimage);
      break;
    }
    case CTkAppCanvasShapeType::BITMAP: {
      auto *i = static_cast<Bitmap *>(s);

      auto *cqimage = i->getImage().cast<CQImage>();
      if (! cqimage) return;

      auto &qimage = cqimage->getQImage();
      p->drawImage(i->pos().x, i->pos().y, qimage);
      break;
    }
    default:
      break;
  }
}

QSize
CTkAppCanvasWidget::
sizeHint() const
{
  int w = std::max(int(canvas_->width ()), 1);
  int h = std::max(int(canvas_->height()), 1);

  return QSize(w, h);
}

//----------

class CTkAppCheckButtonVarProc : public CTclTraceProc {
 public:
  CTkAppCheckButtonVarProc(CTkApp *tk, CTkAppCheckButton *check) :
   CTclTraceProc(tk), check_(check) {
  }

  void handleWrite(const char *) override {
    check_->updateFromVar();
  }

 private:
  CTkAppCheckButton *check_ { nullptr };
};

CTkAppCheckButton::
CTkAppCheckButton(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qcheck_ = new CTkAppCheckButtonWidget(this);

  setQWidget(qcheck_);

  connectSlots(true);
}

void
CTkAppCheckButton::
connectSlots(bool b)
{
  if (b)
    connect(qcheck_, SIGNAL(stateChanged(int)), this, SLOT(stateChangedSlot(int)));
  else
    disconnect(qcheck_, SIGNAL(stateChanged(int)), this, SLOT(stateChangedSlot(int)));
}

bool
CTkAppCheckButton::
execConfig(const std::string &name, const std::string &value)
{
  long underlinePos = -1;

  if      (name == "-command") {
    setCommand(value);
  }
#if 0
  else if (name == "-height") {
    // height of image (pixels) or text (chars)
    double h;

    if (! CTkAppUtil::stringToDistance(value, h))
      return tk_->throwError("Invalid height \"" + value + "\"");

    qcheck_->setHeight(h);
  }
#endif
  else if (name == "-indicatoron") {
    tk_->TODO(name);
  }
  else if (name == "-offrelief") {
    tk_->TODO(name);
  }
  else if (name == "-offvalue") {
    setOffValue(value);

    updateToVar();
  }
  else if (name == "-onrelief") {
    tk_->TODO(name);
  }
  else if (name == "-onvalue") {
    setOnValue(value);

    updateToVar();
  }
  else if (name == "-overrelief") {
    if (value == "raised")
      tk_->TODO(name);
    else
      tk_->TODO(value);
  }
  else if (name == "-selectcolor") {
    tk_->TODO(name);
  }
  else if (name == "-selectimage") {
    tk_->TODO(name);
  }
  else if (name == "-state") {
    tk_->TODO(name);
  }
  else if (name == "-variable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasVar(varName_))
      updateToVar();

    updateFromVar();

    varProc_ = new CTkAppCheckButtonVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
#if 0
  else if (name == "-width") {
    // width of image (pixels) or text (chars)
    double w;

    if (! CTkAppUtil::stringToDistance(value, w))
      return tk_->throwError("Invalid width \"" + value + "\"");

    qcheck_->setWidth(w);
  }
#endif
  else if (name == "-value") {
    bool b;

    if (! CTkAppUtil::stringToBool(value, b))
      return tk_->throwError("Invalid " + name + " \"" + value + "\"");

    setChecked(b);
  }
  else if (name == "-text") {
    setText(value);
  }
  else if (name == "-tristatevalue") {
    tk_->TODO(name);
  }
  else if (name == "-underline") {
    if (! tk_->getOptionInt(name, value, underlinePos))
      return false;
  }
  else if (name == "-wraplength") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppCheckButton::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "deselect") {
    setChecked(false);
  }
  else if (arg == "flash") {
    tk_->TODO(arg);
  }
  else if (arg == "select") {
    setChecked(true);
  }
  else if (arg == "toggle") {
    setChecked(! qcheck_->isChecked());
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppCheckButton::
setText(const std::string &text)
{
  qcheck_->setText(QString::fromStdString(text));
}

void
CTkAppCheckButton::
setChecked(bool b)
{
  if (b != qcheck_->isChecked()) {
    connectSlots(false);

    qcheck_->setChecked(b);

    updateToVar();

    connectSlots(true);
  }
}

void
CTkAppCheckButton::
stateChangedSlot(int)
{
  updateToVar();

  runCommand();
}

void
CTkAppCheckButton::
updateFromVar()
{
  if (varName_ != "" && tk_->hasVar(varName_)) {
    bool checked = false;

    if      (onValue() && tk_->getStringVar(varName_) == *onValue())
      checked = true;
    else if (offValue() && tk_->getStringVar(varName_) == *offValue())
      checked = true;
    else {
      bool checked1 = tk_->getBoolVar(varName_);

      if ((checked1 && ! onValue()) || (! checked && ! offValue()))
        checked = checked1;
    }

    setChecked(checked);
  }
}

void
CTkAppCheckButton::
updateToVar()
{
  if (varName_ != "") {
    varProc_->setEnabled(false);

    if (qcheck_->isChecked()) {
      if (onValue())
        tk_->setStringVar(varName_, *onValue());
      else
        tk_->setBoolVar(varName_, true);
    }
    else {
      if (offValue())
        tk_->setStringVar(varName_, *offValue());
      else
        tk_->setBoolVar(varName_, false);
    }

    varProc_->setEnabled(true);
  }
}

//---

CTkAppCheckButtonWidget::
CTkAppCheckButtonWidget(CTkAppCheckButton *check) :
 QCheckBox(check->getParent() ? check->getParent()->getQWidget() : nullptr)
{
}

//----------

class CTkAppEntryVarProc : public CTclTraceProc {
 public:
  CTkAppEntryVarProc(CTkApp *tk, CTkAppEntry *entry) :
   CTclTraceProc(tk), entry_(entry) {
  }

  void handleWrite(const char *) override {
    entry_->updateFromVar();
  }

 private:
  CTkAppEntry *entry_ { nullptr };
};

class CTkAppEntryValidator : public QValidator {
 public:
  CTkAppEntryValidator(CTkApp *tk, CTkAppEntry *entry) :
   QValidator(entry), tk_(tk), entry_(entry) {
  }

  State validate(QString &input, int & /*pos*/) const override {
    if (entry_->validate(input.toStdString()))
      return Acceptable;

    return Invalid;
  }

 private:
  CTkApp*      tk_    { nullptr };
  CTkAppEntry* entry_ { nullptr };
};

CTkAppEntry::
CTkAppEntry(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qedit_ = new QLineEdit(getParent()->getQWidget());
  else
    qedit_ = new QLineEdit(nullptr);

  setQWidget(qedit_);

  qwidget_->setFocusPolicy(Qt::StrongFocus);

  connectSlots(true);
}

void
CTkAppEntry::
connectSlots(bool b)
{
  if (b)
    connect(qedit_, SIGNAL(editingFinished()), this, SLOT(valueChangedSlot()));
  else
    disconnect(qedit_, SIGNAL(editingFinished()), this, SLOT(valueChangedSlot()));
}

bool
CTkAppEntry::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasVar(varName_))
      tk_->setStringVar(varName_, "");

    if (tk_->hasVar(varName_))
      setText(tk_->getStringVar(varName_));

    varProc_ = new CTkAppEntryVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-show") {
    qedit_->setEchoMode(QLineEdit::Password);
  }
  else if (name == "-width") {
    // width in characters for text
    tk_->TODO(name);
  }
  else if (name == "-validate") {
    if      (value == "none"    ) validateMode_ = ValidateMode::NONE;
    else if (value == "focus"   ) validateMode_ = ValidateMode::FOCUS;
    else if (value == "focusin" ) validateMode_ = ValidateMode::FOCUSIN;
    else if (value == "focusout") validateMode_ = ValidateMode::FOCUSOUT;
    else if (value == "key"     ) validateMode_ = ValidateMode::KEY;
    else if (value == "all"     ) validateMode_ = ValidateMode::ALL;
    else return tk_->throwError("Invalid validate mode '" + value + "'");

    if (! validateProc_) {
      validateProc_ = new CTkAppEntryValidator(tk_, this);

      qedit_->setValidator(validateProc_);
    }
  }
  else if (name == "-validatecommand") {
    validateCmd_ = value;
  }
  else if (name == "-state") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppEntry::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "delete") {
    tk_->TODO(arg);
  }
  else if (arg == "insert") {
    tk_->TODO(arg);
  }
  else if (arg == "get") {
    tk_->TODO(arg);
  }
  else if (arg == "-state") {
    tk_->TODO(arg);
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppEntry::
setText(const std::string &text)
{
  qedit_->setText(QString::fromStdString(text));
}

void
CTkAppEntry::
valueChangedSlot()
{
  if (varName_ != "") {
    varProc_->setEnabled(false);

    if (! tk_->hasVar(varName_))
      tk_->setStringVar(varName_, "");

    tk_->setStringVar(varName_, qedit_->text().toStdString());

    varProc_->setEnabled(true);
  }
}

bool
CTkAppEntry::
validate(const std::string &s) const
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

    auto pos = cmd.find("%P");
    if (pos != std::string::npos)
      cmd = cmd.substr(0, pos) + "{" + s + "}" + cmd.substr(pos + 2);

    if (! tk_->eval(cmd))
      return false;

    auto res = tk_->getStringResult();

    bool b;

    if (! CTkAppUtil::stringToBool(res, b))
      return false;

    return b;
  }

  return true;
}

void
CTkAppEntry::
updateFromVar()
{
  if (varName_ != "" && tk_->hasVar(varName_))
    setText(tk_->getStringVar(varName_));
}

//----------

CTkAppFrame::
CTkAppFrame(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qframe_ = new QFrame(getParent()->getQWidget());
  else
    qframe_ = new QFrame(nullptr);

  setQWidget(qframe_);
}

bool
CTkAppFrame::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-class") {
    setOptionClass(value);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppFrame::
execOp(const Args &args)
{
  return CTkAppWidget::execOp(args);
}

//----------

class CTkAppLabelVarProc : public CTclTraceProc {
 public:
  CTkAppLabelVarProc(CTkApp *tk, CTkAppLabel *label) :
   CTclTraceProc(tk), label_(label) {
  }

  void handleWrite(const char *) override {
    label_->updateFromVar();
  }

 private:
  CTkAppLabel *label_ { nullptr };
};

CTkAppLabel::
CTkAppLabel(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qlabel_ = new CQLabelImage(getParent()->getQWidget());
  else
    qlabel_ = new CQLabelImage(nullptr);

  setQWidget(qlabel_);
}

bool
CTkAppLabel::
execConfig(const std::string &name, const std::string &value)
{
  long underlinePos = -1;

  if      (name == "-text")
    setText(value);
  else if (name == "-bitmap") {
    auto image = tk_->getBitmap(value);

    if (! image)
      return tk_->throwError("Failed to load bitmap '" + value + "'");

    setImage(image);
  }
  else if (name == "-image") {
    auto image = tk_->getImage(value);

    if (! image)
      return tk_->throwError("Failed to get image '" + value + "'");

    setImage(image->getImage());
  }
  else if (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasVar(varName_))
      tk_->setStringVar(varName_, "");

    if (tk_->hasVar(varName_))
      setText(tk_->getStringVar(varName_));

    varProc_ = new CTkAppLabelVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-compound") {
    // none, bottom, top, left, right, or center.
    auto type = CQLabelImage::Type::NONE;

    if      (value == "none"  ) type = CQLabelImage::Type::NONE;
    else if (value == "bottom") type = CQLabelImage::Type::BOTTOM;
    else if (value == "top"   ) type = CQLabelImage::Type::TOP;
    else if (value == "left"  ) type = CQLabelImage::Type::LEFT;
    else if (value == "right" ) type = CQLabelImage::Type::RIGHT;
    else if (value == "center") type = CQLabelImage::Type::CENTER;

    qlabel_->setType(type);
  }
  else if (name == "-wraplength") {
    tk_->TODO(name);
  }
  else if (name == "-underline") {
    if (! tk_->getOptionInt(name, value, underlinePos))
      return false;
  }
  else if (name == "-state") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppLabel::
execOp(const Args &args)
{
  return CTkAppWidget::execOp(args);
}

void
CTkAppLabel::
setText(const std::string &text)
{
  qlabel_->setLabel(QString::fromStdString(text));
}

void
CTkAppLabel::
setImage(CImagePtr image)
{
  auto *cqimage = image.cast<CQImage>();
  if (! cqimage) return;

  auto &qimage = cqimage->getQImage();

  //QPixmap pixmap;
  //pixmap.convertFromImage(qimage);
  //qlabel_->setPixmap(pixmap);

  qlabel_->setImage(qimage);
}

void
CTkAppLabel::
updateFromVar()
{
  if (varName_ != "" && tk_->hasVar(varName_))
    setText(tk_->getStringVar(varName_));
}

//----------

CTkAppLabelFrame::
CTkAppLabelFrame(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qframe_ = new QGroupBox(getParent()->getQWidget());
  else
    qframe_ = new QGroupBox(nullptr);

  setQWidget(qframe_);
}

bool
CTkAppLabelFrame::
execConfig(const std::string &name, const std::string &value)
{
  if (name == "-text") {
    setText(value);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppLabelFrame::
execOp(const Args &args)
{
  return CTkAppWidget::execOp(args);
}

void
CTkAppLabelFrame::
setText(const std::string &text)
{
  qframe_->setTitle(QString::fromStdString(text));
}

//----------

class CTkAppListBoxVarProc : public CTclTraceProc {
 public:
  CTkAppListBoxVarProc(CTkApp *tk, CTkAppListBox *listBox) :
   CTclTraceProc(tk), listBox_(listBox) {
  }

  void handleWrite(const char *) override {
    listBox_->updateFromVar();
  }

 private:
  CTkAppListBox *listBox_ { nullptr };
};

CTkAppListBox::
CTkAppListBox(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qlist_ = new QListWidget(getParent()->getQWidget());
  else
    qlist_ = new QListWidget(nullptr);

  setQWidget(qlist_);

  qlist_->verticalScrollBar  ()->setVisible(false);
  qlist_->horizontalScrollBar()->setVisible(false);

  connectSlots(true);
}

void
CTkAppListBox::
connectSlots(bool b)
{
  if (b) {
    connect(qlist_->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(vscrollSlot(int)));
    connect(qlist_->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(hscrollSlot(int)));
  }
  else {
    disconnect(qlist_->verticalScrollBar(), SIGNAL(valueChanged(int)),
               this, SLOT(vscrollSlot(int)));
    disconnect(qlist_->horizontalScrollBar(), SIGNAL(valueChanged(int)),
               this, SLOT(hscrollSlot(int)));
  }
}

bool
CTkAppListBox::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-listvariable") {
    varName_ = value;

    updateFromVar();

    varProc_ = new CTkAppListBoxVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-selectmode") {
    tk_->TODO(name);
  }
  else if (name == "-exportselection") {
    tk_->TODO(name);
  }
  else if (name == "-setgrid") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppListBox::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "curselection") {
    tk_->TODO(arg);
  }
  else if (arg == "delete") {
    tk_->TODO(arg);
  }
  else if (arg == "get") {
    tk_->TODO(arg);
  }
  else if (arg == "index") {
    tk_->TODO(arg);
  }
  else if (arg == "insert") {
    tk_->TODO(arg);
  }
  else if (arg == "itemcget") {
    tk_->TODO(arg);
  }
  else if (arg == "itemconfigure") {
    tk_->TODO(arg);
  }
  else if (arg == "nearest") {
    tk_->TODO(arg);
  }
  else if (arg == "scan") {
    tk_->TODO(arg);
  }
  else if (arg == "see") {
    tk_->TODO(arg);
  }
  else if (arg == "selection") {
    tk_->TODO(arg);
  }
  else if (arg == "size") {
    tk_->TODO(arg);
  }
  else if (arg == "xview") {
    int step  = qlist_->horizontalScrollBar()->pageStep();
    int min   = qlist_->horizontalScrollBar()->minimum();
    int max   = qlist_->horizontalScrollBar()->maximum() - step;
    int value = qlist_->horizontalScrollBar()->value();

    if (numArgs < 2) {
      std::vector<double> reals;

      reals.push_back( value        /1000.0);
      reals.push_back((value + step)/1000.0);

      tk_->setRealArrayResult(reals);
    }
    else {
      auto opt = args[1];

      if      (opt == "moveto") {
        if (numArgs < 3)
          return tk_->wrongNumArgs(getName() + "xview moveto number");

        double x;

        if (! tk_->getOptionReal(opt, args[2], x))
          return false;

        qlist_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
      else if (opt == "scroll") {
        if (numArgs < 4)
          return tk_->wrongNumArgs(getName() + "xview scroll number pages|units");

        double x;

        if (! tk_->getOptionReal(opt, args[2], x))
          return false;

        if      (args[3] == "pages")
          value += x*1000*step;
        else if (args[3] == "units")
          value += x*1000;
        else
          return tk_->throwError("bad argument '" + args[3] + "' ust be pages or units");

        qlist_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
      else {
        double x;

        if (! CTkAppUtil::stringToReal(opt, x))
          return tk_->throwError("Invalid xview value '" + opt + "'");

        qlist_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
    }
  }
  else if (arg == "yview") {
    int step  = qlist_->verticalScrollBar()->pageStep();
    int min   = qlist_->verticalScrollBar()->minimum();
    int max   = qlist_->verticalScrollBar()->maximum() - step;
    int value = qlist_->verticalScrollBar()->value();

    if (numArgs < 2) {
      std::vector<double> reals;

      reals.push_back( value        /1000.0);
      reals.push_back((value + step)/1000.0);

      tk_->setRealArrayResult(reals);
    }
    else {
      auto opt = args[1];

      if      (opt == "moveto") {
        if (numArgs < 3)
          return tk_->wrongNumArgs(getName() + "yview moveto number");

        double y;

        if (! tk_->getOptionReal(opt, args[2], y))
          return false;

        qlist_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
      else if (opt == "scroll") {
        if (numArgs < 4)
          return tk_->wrongNumArgs(getName() + "yview scroll number pages|units");

        double y;

        if (! tk_->getOptionReal(opt, args[2], y))
          return false;

        if      (args[3] == "pages")
          value += y*1000*step;
        else if (args[3] == "units")
          value += y*1000;
        else
          return tk_->throwError("bad argument '" + args[3] + "' ust be pages or units");

        qlist_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
      else {
        double y;

        if (! CTkAppUtil::stringToReal(opt, y))
          return tk_->throwError("Invalid yview value '" + opt + "'");

        qlist_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
    }
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppListBox::
vscrollSlot(int value)
{
  if (yScrollCommand_ != "") {
    int step = qlist_->verticalScrollBar()->pageStep();
    int min  = qlist_->verticalScrollBar()->minimum();
    int max  = qlist_->verticalScrollBar()->maximum() - step;

    double y1 = double(value        - min)/double(max - min);
    double y2 = double(value + step - min)/double(max - min);

    auto cmd = yScrollCommand_ + " " + std::to_string(y1) + " " + std::to_string(y2);

    tk_->eval(cmd);
  }
}

void
CTkAppListBox::
hscrollSlot(int value)
{
  if (xScrollCommand_ != "") {
    int step = qlist_->horizontalScrollBar()->pageStep();
    int min  = qlist_->horizontalScrollBar()->minimum();
    int max  = qlist_->horizontalScrollBar()->maximum();

    double x1 = double(value        - min)/double(max - min);
    double x2 = double(value + step - min)/double(max - min);

    auto cmd = xScrollCommand_ + " " + std::to_string(x1) + " " + std::to_string(x2);

    tk_->eval(cmd);
  }
}

void
CTkAppListBox::
updateFromVar()
{
  if (varName_ != "" && tk_->hasVar(varName_)) {
    std::vector<std::string> strs;

    if (! tk_->getStringArrayVar(varName_, strs))
      return;

     qlist_->clear();

     for (const auto &str : strs)
       qlist_->addItem(QString::fromStdString(str));
  }
}

//----------

CTkAppMenu::
CTkAppMenu(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qmenu_ = new QMenu(getParent()->getQWidget());
  else
    qmenu_ = new QMenu(nullptr);

  setQWidget(qmenu_);

  auto *menuButton = dynamic_cast<CTkAppMenuButton *>(parent);

  if (menuButton)
    menuButton->updateMenu();
}

bool
CTkAppMenu::
execConfig(const std::string &name, const std::string &value)
{
  if (name == "-tearoff") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppMenu::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "add") {
    if (numArgs < 2)
      tk_->throwError("Invalid menu add args");

    const auto &type = args[1];

    if      (type == "cascade") {
      auto *menu = new CTkAppSubMenu(tk_);

      std::string label;
      long        underlinePos = -1;

      for (uint i = 2; i < numArgs - 1; i += 2) {
        const auto &name  = args[i    ];
        const auto &value = args[i + 1];

        if      (name == "-label") {
          label = value;
        }
        else if (name == "-menu") {
          menu->setMenu(value);
        }
        else if (name == "-underline") {
          if (! tk_->getOptionInt(name, value, underlinePos))
            return false;
        }
        else
          tk_->throwError("Invalid menu cascade option \"" + name + "\"");
      }

      if (label != "") {
        auto label1 = underlineLabel(label, underlinePos);

        menu->setTitle(QString::fromStdString(label1));
      }

      qmenu_->addMenu(menu);
    }
    else if (type == "check" || type == "checkbutton") {
      auto *check = new CTkAppCheckAction(tk_);

      long underlinePos = -1;

      for (uint i = 2; i < numArgs - 1; i += 2) {
        const auto &name  = args[i    ];
        const auto &value = args[i + 1];

        if      (name == "-label") {
          check->setText(value);
        }
        else if (name == "-command") {
          check->setProperty("CTkApp::Command", QString::fromStdString(value));

          connect(check, SIGNAL(stateChanged(int)), this, SLOT(actionPressedSlot()));
        }
        else if (name == "-variable") {
          check->setVarName(value);
        }
        else if (name == "-underline") {
          if (! tk_->getOptionInt(name, value, underlinePos))
            return false;
        }
        else
          tk_->throwError("Invalid menu check option \"" + name + "\"");
      }

      connect(check, SIGNAL(stateChanged(int)), this, SLOT(checkChangedSlot(int)));

      qmenu_->addAction(check);
    }
    else if (type == "command") {
      auto *action = new QAction("");

      long        underlinePos = -1;
      std::string accelerator;

      for (uint i = 2; i < numArgs - 1; i += 2) {
        const auto &name  = args[i    ];
        const auto &value = args[i + 1];

        if      (name == "-label") {
          action->setText(QString::fromStdString(value));
        }
        else if (name == "-command") {
          action->setProperty("CTkApp::Command", QString::fromStdString(value));

          connect(action, SIGNAL(triggered()), this, SLOT(actionPressedSlot()));
        }
        else if (name == "-variable") {
          tk_->TODO(name);
        }
        else if (name == "-underline") {
          if (! tk_->getOptionInt(name, value, underlinePos))
            return false;
        }
        else if (name == "-accelerator") {
          accelerator = name;
        }
        else
          tk_->throwError("Invalid menu command option \"" + name + "\"");
      }

      qmenu_->addAction(action);
    }
    else if (type == "radio" || type == "radiobutton") {
      auto *radio = new CTkAppRadioAction(tk_);

      for (uint i = 2; i < numArgs - 1; i += 2) {
        const auto &name  = args[i    ];
        const auto &value = args[i + 1];

        if      (name == "-label") {
          radio->setText(value);
        }
        else if (name == "-variable") {
          radio->setVarName(value);
        }
        else if (name == "-value") {
          radio->setValue(value);
        }
        else
          tk_->throwError("Invalid menu radio option \"" + name + "\"");
      }

      connect(radio, SIGNAL(toggled(bool)), this, SLOT(radioToggledSlot(bool)));

      qmenu_->addAction(radio);
    }
    else if (type == "separator") {
      qmenu_->addSeparator();
    }
    else
      tk_->throwError("Invalid menu add type \"" + type + "\"");
  }
  else if (arg == "entryconfig") {
    tk_->TODO(arg);
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppMenu::
actionPressedSlot()
{
  auto *action = qobject_cast<QAction *>(sender());
  if (! action) return;

  auto cmd = action->property("CTkApp::Command").toString();

  if (cmd != "")
    tk_->eval(cmd.toLatin1().constData());
}

void
CTkAppMenu::
checkChangedSlot(int)
{
  auto *action = qobject_cast<CTkAppCheckAction *>(sender());
  if (! action) return;

  action->updateFromVar();
}

void
CTkAppMenu::
radioToggledSlot(bool b)
{
  auto *action = qobject_cast<CTkAppRadioAction *>(sender());
  if (! action) return;

  if (b)
    action->updateFromVar();
}

//---

void
CTkAppCheckAction::
updateFromVar()
{
  if (varName_ != "") {
    tk_->setBoolVar(varName_, check_->isChecked());
  }
}

void
CTkAppRadioAction::
updateFromVar()
{
  if (varName_ != "") {
    if (value_ != "")
      tk_->setStringVar(varName_, value_);
    else
      tk_->setBoolVar(varName_, radio_->isChecked());
  }
}

//----------

class CTkAppMenuButtonVarProc : public CTclTraceProc {
 public:
  CTkAppMenuButtonVarProc(CTkApp *tk, CTkAppMenuButton *menuButton) :
   CTclTraceProc(tk), menuButton_(menuButton) {
  }

  void handleWrite(const char *) override {
    menuButton_->updateFromVar();
  }

 private:
  CTkAppMenuButton *menuButton_ { nullptr };
};

CTkAppMenuButton::
CTkAppMenuButton(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qbutton_ = new QToolButton(getParent()->getQWidget());
  else
    qbutton_ = new QToolButton(nullptr);

  qbutton_->setToolButtonStyle(Qt::ToolButtonTextOnly);

  setQWidget(qbutton_);

  connectSlots(true);
}

void
CTkAppMenuButton::
connectSlots(bool b)
{
  if (b)
    connect(qbutton_, SIGNAL(clicked()), this, SLOT(clickSlot()));
  else
    disconnect(qbutton_, SIGNAL(clicked()), this, SLOT(clickSlot()));
}

bool
CTkAppMenuButton::
execConfig(const std::string &name, const std::string &value)
{
  long underlinePos = -1;

  if      (name == "-text") {
    setText(value);
  }
  else if (name == "-command") {
    setCommand(value);
  }
  else if (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasVar(varName_))
      tk_->setStringVar(varName_, "");

    if (tk_->hasVar(varName_))
      setText(tk_->getStringVar(varName_));

    varProc_ = new CTkAppMenuButtonVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-image") {
    auto image = tk_->getImage(value);

    if (image)
      setImage(image->getImage());
  }
  else if (name == "-width") {
    // width in characters for text or screen units for image
    tk_->TODO(name);
  }
  else if (name == "-menu") {
    menuName_ = value;

    updateMenu();
  }
  else if (name == "-underline") {
    if (! tk_->getOptionInt(name, value, underlinePos))
      return false;
  }
  else if (name == "-indicatoron") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppMenuButton::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  const auto &arg = args[0];

  if (arg == "invoke")
    runCommand();
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppMenuButton::
setText(const std::string &text)
{
  qbutton_->setText(QString::fromStdString(text));
}

void
CTkAppMenuButton::
setImage(CImagePtr image)
{
  auto *cqimage = image.cast<CQImage>();
  if (! cqimage) return;

  auto &qimage = cqimage->getQImage();

  QPixmap pixmap;

  pixmap.convertFromImage(qimage);

  qbutton_->setIcon(QIcon(pixmap));
}

void
CTkAppMenuButton::
updateMenu()
{
  if (menuName_ != "") {
    auto *w = tk_->lookupWidgetByName(menuName_, /*quiet*/true);
    if (! w) return; // not defined yet

    auto *menu = dynamic_cast<CTkAppMenu *>(w);
    if (! menu) { std::cerr << "No menu '" << menuName_ << "'\n"; return; }

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
  if (varName_ != "" && tk_->hasVar(varName_))
    setText(tk_->getStringVar(varName_));
}

//----------

class CTkAppMessageVarProc : public CTclTraceProc {
 public:
  CTkAppMessageVarProc(CTkApp *tk, CTkAppMessage *message) :
   CTclTraceProc(tk), message_(message) {
  }

  void handleWrite(const char *) override {
    message_->updateFromVar();
  }

 private:
  CTkAppMessage *message_ { nullptr };
};

CTkAppMessage::
CTkAppMessage(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qedit_ = new QTextEdit(getParent()->getQWidget());
  else
    qedit_ = new QTextEdit(nullptr);

  qedit_->setReadOnly(true);

  setQWidget(qedit_);
}

bool
CTkAppMessage::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-text")
    setText(value);
  else if (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasVar(varName_))
      tk_->setStringVar(varName_, "");

    if (tk_->hasVar(varName_))
      setText(tk_->getStringVar(varName_));

    varProc_ = new CTkAppMessageVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-justify") {
    tk_->TODO(name);
  }
  else if (name == "-aspect") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppMessage::
execOp(const Args &args)
{
  return CTkAppWidget::execOp(args);
}

void
CTkAppMessage::
setText(const std::string &text)
{
  qedit_->setText(QString::fromStdString(text));
}

void
CTkAppMessage::
updateFromVar()
{
  if (varName_ != "" && tk_->hasVar(varName_))
    setText(tk_->getStringVar(varName_));
}

//----------

CTkAppPanedWindow::
CTkAppPanedWindow(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qpane_ = new QSplitter(getParent()->getQWidget());
  else
    qpane_ = new QSplitter(nullptr);

  qpane_->setChildrenCollapsible(false);

  setQWidget(qpane_);
}

bool
CTkAppPanedWindow::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-orient") {
    Qt::Orientation orient;

    if (! CTkAppUtil::stringToOrient(value, orient))
      return false;

    qpane_->setOrientation(orient);
  }
  else if (name == "-showhandle") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppPanedWindow::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "add") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " add widget ?widget ...?");

    int min_size = -1;
    int padx     = -1;
    int pady     = -1;

    for (uint i = 1; i < numArgs; ++i) {
      const auto &name = args[i];

      if (name.size() > 0 && name[0] == '-') {
        if      (name == "-minsize") {
          ++i;

          if (i >= numArgs)
            return tk_->throwError("Missing value for \"-minsize\"");

          const auto &value = args[i];

          long i;
          if (! tk_->getOptionInt(name, value, i))
            return false;

          min_size = i;
        }
        else if (name == "-padx") {
          ++i;

          if (i >= numArgs)
            return tk_->throwError("Missing value for \"-padx\"");

          long pad;
          if (! tk_->getOptionInt(name, args[i], pad))
            return false;

          padx = pad;

          tk_->TODO("padx " + std::to_string(padx) + " not supported");
        }
        else if (name == "-pady") {
          ++i;

          if (i >= numArgs)
            return tk_->throwError("Missing value for \"-pady\"");

          long pad;
          if (! tk_->getOptionInt(name, args[i], pad))
            return false;

          pady = pad;

          tk_->TODO("pady " + std::to_string(pady) + " not supported");
        }
      }
      else {
        CTkAppWidget*  parent;
        std::string childName;

        if (! root()->decodeWidgetName(name, &parent, childName))
          return tk_->throwError("Invalid name '" + name + "'");

        auto *child = parent->getChild(childName);

        if (child == nullptr)
          return tk_->throwError("Invalid name '" + name + "'");

        auto *qwidget = child->getQWidget();

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
  else if (arg == "forget") {
    tk_->TODO(arg);
  }
  else if (arg == "identify") {
    tk_->TODO(arg);
  }
  else if (arg == "panecget") {
    tk_->TODO(arg);
  }
  else if (arg == "paneconfigure") {
    tk_->TODO(arg);
  }
  else if (arg == "panes") {
    tk_->TODO(arg);
  }
  else if (arg == "proxy") {
    tk_->TODO(arg);
  }
  else if (arg == "sash") {
    tk_->TODO(arg);
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

//----------

class CTkAppRadioButtonVarProc : public CTclTraceProc {
 public:
  CTkAppRadioButtonVarProc(CTkApp *tk, CTkAppRadioButton *radio) :
   CTclTraceProc(tk), radio_(radio) {
  }

  void handleWrite(const char *) override {
    radio_->updateFromVar();
  }

 private:
  CTkAppRadioButton *radio_ { nullptr };
};

CTkAppRadioButton::
CTkAppRadioButton(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qradio_ = new CTkAppRadioButtonWidget(this);

  setQWidget(qradio_);

  connectSlots(true);
}

void
CTkAppRadioButton::
connectSlots(bool b)
{
  if (b)
    connect(qradio_, SIGNAL(toggled(bool)), this, SLOT(toggleSlot(bool)));
  else
    disconnect(qradio_, SIGNAL(toggled(bool)), this, SLOT(toggleSlot(bool)));
}

bool
CTkAppRadioButton::
execConfig(const std::string &name, const std::string &value)
{
  long underlinePos = -1;

  if      (name == "-text")
    setText(value);
  else if (name == "-variable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasVar(varName_))
      updateToVar();

    updateFromVar();

    varProc_ = new CTkAppRadioButtonVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-value") {
    value_ = value;

    updateToVar();
  }
  else if (name == "-command") {
    setCommand(value);
  }
  else if (name == "-tristatevalue") {
    tk_->TODO(name);
  }
  else if (name == "-wraplength") {
    tk_->TODO(name);
  }
  else if (name == "-underline") {
    if (! tk_->getOptionInt(name, value, underlinePos))
      return false;
  }
  else if (name == "-indicatoron") {
    tk_->TODO(name);
  }
  else if (name == "-image") {
    tk_->TODO(name);
  }
  else if (name == "-selectcolor") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppRadioButton::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "deselect") {
    setChecked(false);
  }
  else if (arg == "flash") {
    tk_->TODO(arg);
  }
  else if (arg == "select") {
    setChecked(true);
  }
  else if (arg == "toggle") {
    setChecked(! qradio_->isChecked());
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppRadioButton::
setText(const std::string &text)
{
  qradio_->setText(QString::fromStdString(text));
}

void
CTkAppRadioButton::
setValue(const std::string &value)
{
  bool checked = qradio_->isChecked();

  if (value_ != "")
    checked = (value == value_);
  else {
    bool b;

    if (CTkAppUtil::stringToBool(value, b))
      checked = b;
  }

  setChecked(checked);
}

void
CTkAppRadioButton::
setChecked(bool b)
{
  if (b != qradio_->isChecked()) {
    connectSlots(false);

    qradio_->setChecked(b);

    connectSlots(true);
  }
}

void
CTkAppRadioButton::
toggleSlot(bool)
{
  updateToVar();

  runCommand();
}

void
CTkAppRadioButton::
updateFromVar()
{
  if (varName_ != "" && tk_->hasVar(varName_))
    setValue(tk_->getStringVar(varName_));
}

void
CTkAppRadioButton::
updateToVar()
{
  if (varName_ != "") {
    varProc_->setEnabled(false);

    if (qradio_->isChecked()) {
      if (value_ != "")
        tk_->setStringVar(varName_, value_);
      else
        tk_->setBoolVar(varName_, qradio_->isChecked());
    }

    varProc_->setEnabled(true);
  }
}

//---

CTkAppRadioButtonWidget::
CTkAppRadioButtonWidget(CTkAppRadioButton *radio) :
 QRadioButton(radio->getParent() ? radio->getParent()->getQWidget() : nullptr), radio_(radio)
{
}

//----------

class CTkAppScaleVarProc : public CTclTraceProc {
 public:
  CTkAppScaleVarProc(CTkApp *tk, CTkAppScale *scale) :
   CTclTraceProc(tk), scale_(scale) {
  }

  void handleWrite(const char *) override {
    scale_->updateFromVar();
  }

 private:
  CTkAppScale *scale_ { nullptr };
};

CTkAppScale::
CTkAppScale(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qscale_ = new CQRealSlider(getParent()->getQWidget());
  else
    qscale_ = new CQRealSlider(nullptr);

  qscale_->setValuePosition(CQSliderBase::ValuePosition::NONE);

  setQWidget(qscale_);

  connectSlots(true);
}

void
CTkAppScale::
connectSlots(bool b)
{
  if (b)
    connect(qscale_, SIGNAL(valueChanged(int)), this, SLOT(valueSlot(int)));
  else
    disconnect(qscale_, SIGNAL(valueChanged(int)), this, SLOT(valueSlot(int)));
}

bool
CTkAppScale::
execConfig(const std::string &name, const std::string &value)
{
  // widget specific options
  if      (name == "-bigincrement") {
    tk_->TODO(name);
  }
  else if (name == "-command") {
    setCommand(value);
  }
  else if (name == "-digits") {
    long n;
    if (! tk_->getOptionInt(name, value, n))
      return false;

    qscale_->setPrecision(n);
  }
  else if (name == "-from") {
    long from;
    if (! tk_->getOptionInt(name, value, from))
      return false;

    qscale_->setMinimum(from);
  }
  else if (name == "-label") {
    qscale_->setLabel(QString::fromStdString(value));
  }
  else if (name == "-length") {
    int l;

    if (! CTkAppUtil::stringToDistanceI(value, l))
      return tk_->throwError("Invalid length \"" + value + "\"");

    length_ = l;

    updateSize();
  }
  else if (name == "-orient") {
    Qt::Orientation orient;

    if (! CTkAppUtil::stringToOrient(value, orient))
      return false;

    qscale_->setOrientation(orient);

    updateSize();
  }
  else if (name == "-resolution") {
    double r;

    if (! tk_->getOptionReal(name, value, r))
      return false;

    if (r > 0.0)
      qscale_->setSingleStep(r);
    else
      qscale_->setSingleStep(1.0);
  }
  else if (name == "-showvalue") {
    bool b;

    if (! CTkAppUtil::stringToBool(value, b))
      return tk_->throwError("Invalid " + name + " \"" + value + "\"");

    if (b)
      qscale_->setValuePosition(CQSliderBase::ValuePosition::ABOVE);
    else
      qscale_->setValuePosition(CQSliderBase::ValuePosition::NONE);
  }
  else if (name == "-sliderlength") {
    tk_->TODO(name);
  }
  else if (name == "-sliderrelief") {
    tk_->TODO(name);
  }
  else if (name == "-state") {
    setWidgetState(this, value);
  }
  else if (name == "-tickinterval") {
    double r;

    if (! tk_->getOptionReal(name, value, r))
      return false;

    if (r > 0.0)
      qscale_->setTickLabelDelta(r);
    else
      qscale_->setTickLabelDelta(0.0);
  }
  else if (name == "-to") {
    long to;
    if (! tk_->getOptionInt(name, value, to))
      return false;

    qscale_->setMaximum(to);
  }
  else if (name == "-variable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasVar(varName_))
      tk_->setIntegerVar(varName_, 0);

    updateFromVar();

    varProc_ = new CTkAppScaleVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-width") {
    int w;

    if (! CTkAppUtil::stringToDistanceI(value, w))
      return tk_->throwError("Invalid width \"" + value + "\"");

    width_ = w;

    updateSize();
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppScale::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  auto arg = args[0];

  if (arg == "set") {
    tk_->TODO(arg);
  }
  else
    return CTkAppWidget::execOp(args);

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
valueSlot(int value)
{
  updateToVar();

  Args args;

  args.push_back(std::to_string(value));

  runCommand(args);
}

void
CTkAppScale::
updateFromVar()
{
  if (varName_ != "" && tk_->hasVar(varName_))
    qscale_->setValue(tk_->getIntegerVar(varName_));
}

void
CTkAppScale::
updateToVar()
{
  if (varName_ != "" && tk_->hasVar(varName_)) {
    varProc_->setEnabled(false);

    tk_->setIntegerVar(varName_, qscale_->value());

    varProc_->setEnabled(true);
  }
}

//----------

CTkAppScrollBar::
CTkAppScrollBar(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qscrollbar_ = new QScrollBar(getParent()->getQWidget());
  else
    qscrollbar_ = new QScrollBar(nullptr);

  qscrollbar_->setRange(0, 1000);

  setQWidget(qscrollbar_);

  connectSlots(true);
}

void
CTkAppScrollBar::
connectSlots(bool b)
{
  if (b)
    connect(qscrollbar_, SIGNAL(valueChanged(int)), this, SLOT(scrollSlot(int)));
  else
    disconnect(qscrollbar_, SIGNAL(valueChanged(int)), this, SLOT(scrollSlot(int)));
}

bool
CTkAppScrollBar::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-orient") {
    Qt::Orientation orient;

    if (! CTkAppUtil::stringToOrient(value, orient))
      return false;

    qscrollbar_->setOrientation(orient);
  }
  else if (name == "-command") {
    setCommand(value);
  }
  else if (name == "-troughcolor") {
    setCommand(value);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppScrollBar::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  auto arg = args[0];

  if (arg == "set") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " set firstFraction lastFraction\" or \"" +
                               getName() + " set totalUnits windowUnits firstUnit lastUnit");

    double pos1, pos2;

    if (CTkAppUtil::stringToReal(args[1], pos1) && CTkAppUtil::stringToReal(args[2], pos2)) {
      connectSlots(false);

      qscrollbar_->setPageStep(int((pos2 - pos1)*1000));
      qscrollbar_->setValue   (int(pos1*1000));

      //std::cerr << "set " << pos1 << " " << pos2 << "\n";

      connectSlots(true);
    }
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppScrollBar::
scrollSlot(int i)
{
  Args args;

  args.push_back(std::to_string(i/1000.0));

  runCommand(args);
}

//----------

class CTkAppSpinBoxVarProc : public CTclTraceProc {
 public:
  CTkAppSpinBoxVarProc(CTkApp *tk, CTkAppSpinBox *spin) :
   CTclTraceProc(tk), spin_(spin) {
  }

  void handleWrite(const char *) override {
    spin_->updateFromVar();
  }

 private:
  CTkAppSpinBox *spin_ { nullptr };
};

CTkAppSpinBox::
CTkAppSpinBox(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qspin_ = new CQSpinList(getParent()->getQWidget());
  else
    qspin_ = new CQSpinList(nullptr);

  setQWidget(qspin_);

  qwidget_->setFocusPolicy(Qt::StrongFocus);
}

bool
CTkAppSpinBox::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasVar(varName_))
      tk_->setStringVar(varName_, "");

    if (tk_->hasVar(varName_))
      setValue(tk_->getStringVar(varName_));

    varProc_ = new CTkAppSpinBoxVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-width") {
    tk_->TODO(name);
  }
  else if (name == "-from") {
    long from;
    if (! tk_->getOptionInt(name, value, from))
      return false;

    qspin_->setMinimum(from);
  }
  else if (name == "-to") {
    long to;
    if (! tk_->getOptionInt(name, value, to))
      return false;

    qspin_->setMaximum(to);
  }
  else if (name == "-increment") {
    long step;
    if (! tk_->getOptionInt(name, value, step))
      return false;

    qspin_->setSingleStep(step);
  }
  else if (name == "-values") {
    std::vector<std::string> strs;

    if (! tk_->splitList(value, strs))
      return tk_->throwError("Invalid values \"" + value + "\"");

    QStringList qstrs;

    for (const auto &str : strs)
      qstrs << QString::fromStdString(str);

    qspin_->setStrings(qstrs);
  }
  else if (name == "-state") {
    if (value == "readonly")
      qspin_->setReadOnly(true);
    else
      setWidgetState(this, value);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppSpinBox::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "delete") {
    tk_->TODO(arg);
  }
  else if (arg == "insert") {
    tk_->TODO(arg);
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppSpinBox::
setValue(const std::string &)
{
}

void
CTkAppSpinBox::
updateFromVar()
{
  if (varName_ != "" && tk_->hasVar(varName_))
    qspin_->setValue(tk_->getIntegerVar(varName_));
}

//----------

CTkAppText::
CTkAppText(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qtext_ = new QTextEdit(getParent()->getQWidget());
  else
    qtext_ = new QTextEdit(nullptr);

  qtext_->setFocusPolicy(Qt::StrongFocus);

  setQWidget(qtext_);

  qtext_->verticalScrollBar  ()->setVisible(false);
  qtext_->horizontalScrollBar()->setVisible(false);

  connectSlots(true);
}

void
CTkAppText::
connectSlots(bool b)
{
  if (b) {
    connect(qtext_->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(vscrollSlot(int)));
    connect(qtext_->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(hscrollSlot(int)));
  }
  else {
    disconnect(qtext_->verticalScrollBar(), SIGNAL(valueChanged(int)),
               this, SLOT(vscrollSlot(int)));
    disconnect(qtext_->horizontalScrollBar(), SIGNAL(valueChanged(int)),
               this, SLOT(hscrollSlot(int)));
  }
}

bool
CTkAppText::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-text") {
    setText(value);
  }
  else if (name == "-wrap") {
    tk_->TODO(name);
  }
  else if (name == "-command") {
    setCommand(value);
  }
  else if (name == "-width") {
    tk_->TODO(name);
  }
  else if (name == "-height") {
    tk_->TODO(name);
  }
  else if (name == "-setgrid") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppText::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "bbox") {
    tk_->TODO(arg);
  }
  else if (arg == "xview") {
    tk_->TODO(arg);
  }
  else if (arg == "yview") {
    int step  = qtext_->verticalScrollBar()->pageStep();
    int min   = qtext_->verticalScrollBar()->minimum();
    int max   = qtext_->verticalScrollBar()->maximum() - step;
    int value = qtext_->verticalScrollBar()->value();

    if (numArgs < 2) {
      std::vector<double> reals;

      reals.push_back( value        /1000.0);
      reals.push_back((value + step)/1000.0);

      tk_->setRealArrayResult(reals);
    }
    else {
      auto opt = args[1];

      if      (opt == "moveto") {
        if (numArgs < 3)
          return tk_->wrongNumArgs(getName() + "yview moveto number");

        double y;

        if (! tk_->getOptionReal(opt, args[2], y))
          return false;

        qtext_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
      else if (opt == "scroll") {
        if (numArgs < 4)
          return tk_->wrongNumArgs(getName() + "yview scroll number pages|units");

        double y;

        if (! tk_->getOptionReal(opt, args[2], y))
          return false;

        if      (args[3] == "pages")
          value += y*1000*step;
        else if (args[3] == "units")
          value += y*1000;
        else
          return tk_->throwError("bad argument '" + args[3] + "' ust be pages or units");

        qtext_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
      else {
        double y;

        if (! CTkAppUtil::stringToReal(opt, y))
          return tk_->throwError("Invalid yview value '" + opt + "'");

        qtext_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
    }
  }
  else if (arg == "insert") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " insert index chars ?tagList chars tagList ...?");

    long index;

    if (args[1] == "end") {
      index = -1;
    }
    else {
      if (! CTkAppUtil::stringToInt(args[1], index))
        return false;
    }

    qtext_->insertPlainText(QString::fromStdString(args[2]));
  }
  else if (arg == "delete") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " delete index1 ?index2 ...?");

    struct TextInd {
      int lineNum { -1 };
      int charNum { -1 };

      TextInd(int l, int c) :
       lineNum(l), charNum(c) {
      }
    };

    std::vector<TextInd> inds;

    for (uint i = 1; i < numArgs; ++i) {
      int lineNum { 0 };
      int charNum { 0 };

      if (args[i] == "end") {
        lineNum = -1;
        charNum = -1;
      }
      else {
        if (! stringToLineChar(args[i], lineNum, charNum))
          return false;
      }

      inds.push_back(TextInd(lineNum, charNum));
    }

    tk_->TODO(arg);
  }
  else if (arg == "tag") {
    tk_->TODO(arg);
  }
  else if (arg == "index") {
    tk_->TODO(arg);
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppText::
setText(const std::string &text)
{
  qtext_->setText(QString::fromStdString(text));
}

void
CTkAppText::
vscrollSlot(int value)
{
  if (yScrollCommand_ != "") {
    int step = qtext_->verticalScrollBar()->pageStep();
    int min  = qtext_->verticalScrollBar()->minimum();
    int max  = qtext_->verticalScrollBar()->maximum() - step;

    double y1 = double(value        - min)/double(max - min);
    double y2 = double(value + step - min)/double(max - min);

    auto cmd = yScrollCommand_ + " " + std::to_string(y1) + " " + std::to_string(y2);

    tk_->eval(cmd);
  }
}

void
CTkAppText::
hscrollSlot(int value)
{
  if (xScrollCommand_ != "") {
    int step = qtext_->horizontalScrollBar()->pageStep();
    int min  = qtext_->horizontalScrollBar()->minimum();
    int max  = qtext_->horizontalScrollBar()->maximum();

    double x1 = double(value        - min)/double(max - min);
    double x2 = double(value + step - min)/double(max - min);

    auto cmd = xScrollCommand_ + " " + std::to_string(x1) + " " + std::to_string(x2);

    tk_->eval(cmd);
  }
}

//----------

CTkAppTopLevel::
CTkAppTopLevel(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qframe_ = new QFrame(nullptr);

  setQWidget(qframe_);
}

void
CTkAppTopLevel::
setFrame(QFrame *qframe)
{
  delete qframe_;

  qframe_ = qframe;

  setQWidget(qframe_);
}

bool
CTkAppTopLevel::
execConfig(const std::string &name, const std::string &value)
{
  return CTkAppWidget::execConfig(name, value);
}

bool
CTkAppTopLevel::
execOp(const Args &args)
{
  return CTkAppWidget::execOp(args);
}

//----------

CTkAppWidget::
CTkAppWidget(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 tk_(tk), parent_(parent), name_(name)
{
  setObjectName("widget");

  if (parent_)
    parent_->addChild(this);

  tk_->addWidget(this);
}

CTkAppWidget::
~CTkAppWidget()
{
  if (parent_)
    parent_->removeChild(this);

  tk_->removeWidget(this);

  delete qwidget_;

  delete getWidgetCommand();
}

void
CTkAppWidget::
setParentWidget(QWidget *w)
{
  assert(! isTopLevel());

  qwidget()->setParent(w);
}

CTkAppRoot *
CTkAppWidget::
root() const
{
  return tk_->root();
}

CTkAppTopLevel *
CTkAppWidget::
toplevel() const
{
  auto *top = dynamic_cast<const CTkAppTopLevel *>(this);
  if (top) return const_cast<CTkAppTopLevel *>(top);

  return (parent_ ? parent_->toplevel() : nullptr);
}

std::string
CTkAppWidget::
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
CTkAppWidget::
setQWidget(QWidget *w)
{
  qwidget_ = w;

  if (name_ != "")
    qwidget_->setObjectName(QString::fromStdString(name_));
  else
    qwidget_->setObjectName(".");

  qwidget_->installEventFilter(new CTkAppWidgetEventFilter(this));

  qwidget_->setFocusPolicy(Qt::ClickFocus);
}

QWidget *
CTkAppWidget::
getQWidget() const
{
  return qwidget_;
}

int
CTkAppWidget::
getWidth() const
{
  if (w_ > 0) return w_;

  auto s = qwidget_->sizeHint();

  auto w = s.width();

  if (w <= 0)
    w = 100;

  return w;
}

int
CTkAppWidget::
getHeight() const
{
  if (h_ > 0) return h_;

  auto s = qwidget_->sizeHint();

  auto h = s.height();

  if (h <= 0)
    h = 100;

  return h;
}

void
CTkAppWidget::
setWidth(int w)
{
  //assert(w >= 0 && w <= 2048);

  w_ = w;
}

void
CTkAppWidget::
setHeight(int h)
{
  //assert(h >= 0 && h <= 2048);

  h_ = h;
}

QSize
CTkAppWidget::
sizeHint() const
{
  auto s = qwidget_->sizeHint();

  auto w = (w_ > 0 ? w_ : s.width());
  auto h = (h_ > 0 ? h_ : s.height());

  return QSize(w, h);
}

QSize
CTkAppWidget::
minimumSizeHint() const
{
  auto s = qwidget_->minimumSizeHint();

  auto w = (w_ > 0 ? w_ : s.width());
  auto h = (h_ > 0 ? h_ : s.height());

  return QSize(w, h);
}

void
CTkAppWidget::
addChild(CTkAppWidget *w)
{
  children_[w->getName()] = w;
}

void
CTkAppWidget::
removeChild(CTkAppWidget *w)
{
  auto p = children_.find(w->getName());

  if (p != children_.end())
    children_.erase(p);
}

void
CTkAppWidget::
getChildren(std::vector<CTkAppWidget *> &children) const
{
  WidgetMap::const_iterator p1, p2;

  for (p1 = children_.begin(), p2 = children_.end(); p1 != p2; ++p1)
    children.push_back((*p1).second);
}

CTkAppWidget *
CTkAppWidget::
getChild(const std::string &name) const
{
  auto p = children_.find(name);

  if (p == children_.end())
    return nullptr;

  return (*p).second;
}

void
CTkAppWidget::
setFocus()
{
  auto *w = getQWidget();
  assert(w);

  w->setFocus();
}

void
CTkAppWidget::
lower()
{
  auto *w = getQWidget();
  assert(w);

  w->lower();
}

void
CTkAppWidget::
raise()
{
  auto *w = getQWidget();
  assert(w);

  w->raise();
}

CTkAppPackLayout *
CTkAppWidget::
getTkPackLayout(bool create)
{
  auto *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  auto *l1 = qobject_cast<CTkAppPackLayout *>(l);

  if (! l1 && create)
    l1 = new CTkAppPackLayout(w);

  return l1;
}

CTkAppGridLayout *
CTkAppWidget::
getTkGridLayout(bool create)
{
  auto *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  auto *l1 = qobject_cast<CTkAppGridLayout *>(l);

  if (! l1 && create)
    l1 = new CTkAppGridLayout(w);

  return l1;
}

CTkAppPlaceLayout *
CTkAppWidget::
getTkPlaceLayout(bool create)
{
  auto *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  auto *l1 = qobject_cast<CTkAppPlaceLayout *>(l);

  if (! l1 && create)
    l1 = new CTkAppPlaceLayout(w);

  return l1;
}

bool
CTkAppWidget::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-activebackground") {
    auto c = CTkAppUtil::stringToQColor(value);

    auto pal = qwidget_->palette();
    pal.setColor(QPalette::Active, qwidget()->backgroundRole(), c);
    qwidget_->setPalette(pal);
  }
  else if (name == "-activeforeground") {
    auto c = CTkAppUtil::stringToQColor(value);

    auto pal = qwidget_->palette();
    pal.setColor(QPalette::Active, qwidget()->foregroundRole(), c);
    qwidget_->setPalette(pal);
  }
  else if (name == "-anchor") {
    setWidgetAnchor(value);
  }
  else if (name == "-background" || name == "-bg") {
    auto c = CTkAppUtil::stringToQColor(value);

    auto pal = qwidget_->palette();
    pal.setColor(QPalette::Normal, qwidget()->backgroundRole(), c);
    pal.setColor(QPalette::Inactive, qwidget()->backgroundRole(), c);
    qwidget_->setPalette(pal);

    qwidget_->update();
  }
  else if (name == "-borderwidth" || name == "-bd") {
    long w;
    if (! tk_->getOptionInt(name, value, w))
      return false;
    setBorderWidth(w);
  }
  else if (name == "-compound") {
    tk_->TODO(name);
  }
  else if (name == "-cursor") {
    //auto c = CTkAppUtil::stringToCursor(value);
    //qwidget_->setCursor(c);
    tk_->TODO(name);
  }
  else if (name == "-disabledbackground") {
    auto c = CTkAppUtil::stringToQColor(value);

    auto pal = qwidget_->palette();
    pal.setColor(QPalette::Disabled, qwidget()->backgroundRole(), c);
    qwidget_->setPalette(pal);
  }
  else if (name == "-disabledforeground") {
    auto c = CTkAppUtil::stringToQColor(value);

    auto pal = qwidget_->palette();
    pal.setColor(QPalette::Disabled, qwidget()->foregroundRole(), c);
    qwidget_->setPalette(pal);
  }
  else if (name == "-foreground" || name == "-fg") {
    auto c = CTkAppUtil::stringToQColor(value);

    auto pal = qwidget_->palette();
    pal.setColor(QPalette::Normal, qwidget()->foregroundRole(), c);
    pal.setColor(QPalette::Inactive, qwidget()->backgroundRole(), c);
    qwidget_->setPalette(pal);

    qwidget_->update();
  }
  else if (name == "-highlightbackground") {
    auto c = CTkAppUtil::stringToQColor(value);

    auto pal = qwidget_->palette();
    pal.setColor(QPalette::Active, qwidget()->backgroundRole(), c);
    qwidget_->setPalette(pal);
  }
  else if (name == "-highlightcolor") {
    auto c = CTkAppUtil::stringToQColor(value);

    auto pal = qwidget_->palette();
    pal.setColor(QPalette::Active, qwidget()->foregroundRole(), c);
    qwidget_->setPalette(pal);
  }
  else if (name == "-highlightthickness") {
    double thickness;
    if (! CTkAppUtil::stringToDistance(value, thickness))
      return false;
    setHighlightThickness(thickness);
  }
  else if (name == "-insertbackground") {
    tk_->TODO(name);
  }
  else if (name == "-insertborderwidth") {
    tk_->TODO(name);
  }
  else if (name == "-insertofftime") {
    tk_->TODO(name);
  }
  else if (name == "-insertontime") {
    tk_->TODO(name);
  }
  else if (name == "-insertwidth") {
    tk_->TODO(name);
  }
  else if (name == "-padx") {
    long padx;
    if (! tk_->getOptionInt(name, value, padx))
      return false;

    setPadX(padx);
  }
  else if (name == "-pady") {
    long pady;
    if (! tk_->getOptionInt(name, value, pady))
      return false;

    setPadY(pady);
  }
  else if (name == "-relief") {
    setFrameRelief(qwidget_, value);
  }
  else if (name == "-repeatdelay") {
    tk_->TODO(name);
  }
  else if (name == "-repeatinterval") {
    tk_->TODO(name);
  }
  else if (name == "-takefocus") {
    bool b;
    if (! CTkAppUtil::stringToBool(value, b))
      return tk_->throwError("Invalid " + name + " \"" + value + "\"");
    qwidget_->setFocusPolicy(b ? Qt::TabFocus : Qt::NoFocus);
  }
  else if (name == "-selectbackground") {
    tk_->TODO(name);
  }
  else if (name == "-selectborderwidth") {
    tk_->TODO(name);
  }
  else if (name == "-selectforeground") {
    tk_->TODO(name);
  }
  else if (name == "-width") {
    int w;

    if (! CTkAppUtil::stringToDistanceI(value, w))
      return tk_->throwError("Invalid width \"" + value + "\"");

    setWidth(w);

    qwidget_->resize(getWidth(), getHeight());
  }
  else if (name == "-height") {
    int h;

    if (! CTkAppUtil::stringToDistanceI(value, h))
      return tk_->throwError("Invalid height \"" + value + "\"");

    setHeight(h);

    qwidget_->resize(getWidth(), getHeight());
  }
  else if (name == "-font") {
    auto f = CTkAppUtil::stringToQFont(value);

    qwidget_->setFont(f);

    qwidget_->update();
  }
  else if (name == "-colormap") {
    tk_->TODO(name);
  }
  else if (name == "-visual") {
    tk_->TODO(name);
  }
  else if (name == "-xscrollcommand") {
    xScrollCommand_ = value;
  }
  else if (name == "-yscrollcommand") {
    yScrollCommand_ = value;
  }
  else
    return tk_->throwError("Invalid value name \"" + name + "\" "
                           "for " + std::string(getClassName()));

  return true;
}

bool
CTkAppWidget::
execOp(const Args &args)
{
  const auto &arg = args[0];

  return tk_->throwError("bad option \"" + arg + "\" for " + std::string(getClassName()));
}

void
CTkAppWidget::
show()
{
  if (qwidget_ && ! qwidget_->isVisible())
    qwidget_->setVisible(true);
}

void
CTkAppWidget::
setTitle(const std::string &title)
{
  qwidget_->setWindowTitle(QString::fromStdString(title));
}

std::string
CTkAppWidget::
getTitle() const
{
  return qwidget_->windowTitle().toStdString();
}

void
CTkAppWidget::
setIcon(const std::string &icon)
{
  qwidget_->setWindowIcon(QIcon(QString::fromStdString(icon)));
}

std::string
CTkAppWidget::
getIcon() const
{
  return qwidget_->windowIcon().name().toStdString();
}

bool
CTkAppWidget::
setGeometry(const std::string &str)
{
  auto throwError = [&]() {
    return tk_->throwError("Invalid geometry '" + str + "'");
  };

  auto rect = qwidget_->geometry();

  int x = rect.x    (), y = rect.y     ();
  int w = rect.width(), h = rect.height();

  // format WxH+X+Y
  CStrParse parse(str);

  parse.skipSpace();

  if (parse.readInteger(&w)) {
    if (! parse.isChar('x'))
      return throwError();

    parse.skipChar();

    if (! parse.readInteger(&h))
      return throwError();

    parse.skipSpace();
  }

  if (parse.isChar('-') || parse.isChar('+')) {
    int sign = (parse.isChar('+') ? 1 : -1);

    parse.skipChar();

    if (! parse.readInteger(&x))
      return throwError();

    x *= sign;

    if (! parse.isChar('-') && ! parse.isChar('+'))
      return throwError();

    sign = (parse.isChar('+') ? 1 : -1);

    parse.skipChar();

    if (! parse.readInteger(&y))
      return throwError();

    y *= sign;
  }

  parse.skipSpace();

  if (! parse.eof())
    return throwError();

  qwidget_->setGeometry(QRect(x, y, w, h));

  return true;
}

std::string
CTkAppWidget::
getGeometry() const
{
  auto r = qwidget_->geometry();

  auto str = QString("%1x%2+%3+%4").arg(r.width()).arg(r.height()).arg(r.x()).arg(r.y());

  return str.toStdString();
}

void
CTkAppWidget::
setPadX(int padx)
{
  auto m = qwidget_->contentsMargins();

  m.setLeft (padx);
  m.setRight(padx);

  qwidget_->setContentsMargins(m);
}

void
CTkAppWidget::
setPadY(int pady)
{
  auto m = qwidget_->contentsMargins();

  m.setTop   (pady);
  m.setBottom(pady);

  qwidget_->setContentsMargins(m);
}

void
CTkAppWidget::
setWidgetAnchor(const std::string &value)
{
  auto align = stringToAlign(value);

  setAlign(align);
}

// set borderwidth
void
CTkAppWidget::
setBorderWidth(int width)
{
  auto *frame = qobject_cast<QFrame *>(qwidget_);

  if (frame)
    frame->setLineWidth(width);
}

bool
CTkAppWidget::
bindEvent(const CTkAppEventData &data)
{
  eventDatas_.push_back(data);

  return true;
}

bool
CTkAppWidget::
triggerEvents(QEvent *e, const CTkAppEventData &matchEventData)
{
  processEvents(e, matchEventData);

  // class and global events
  return tk_->triggerEvents(getClassName(), this, e, matchEventData);
}

bool
CTkAppWidget::
triggerMousePressEvents(QEvent *e, int button)
{
  auto *me = static_cast<QMouseEvent *>(e);

  CTkAppEventData matchEventData;

  tk_->encodeEvent(me, CTkAppEventMode::Press, button, matchEventData);

  processEvents(e, matchEventData);

  return true;
}

bool
CTkAppWidget::
triggerMouseMoveEvents(QEvent *e, int button, bool pressed)
{
  if (! pressed)
    return false;

  auto *me = static_cast<QMouseEvent *>(e);

  CTkAppEventData matchEventData;

  tk_->encodeEvent(me, CTkAppEventMode::Motion, button, matchEventData);

  processEvents(e, matchEventData);

  return true;
}

bool
CTkAppWidget::
triggerMouseReleaseEvents(QEvent *e, int button)
{
  auto *me = static_cast<QMouseEvent *>(e);

  CTkAppEventData matchEventData;

  tk_->encodeEvent(me, CTkAppEventMode::Release, button, matchEventData);

  processEvents(e, matchEventData);

  return true;
}

bool
CTkAppWidget::
triggerKeyPressEvents(QEvent *e)
{
  auto *ke = dynamic_cast<QKeyEvent *>(e);

  CTkAppEventData matchEventData;

  tk_->encodeEvent(ke, /*press*/true, matchEventData);

  processEvents(e, matchEventData);

  return tk_->triggerKeyPressEvents(getClassName(), this, e);
}

void
CTkAppWidget::
deleteLater()
{
  deleted_ = true;

  tk_->addDeleteWidget(this);
}

void
CTkAppWidget::
runCommand(const Args &args)
{
  auto cmd = getCommand();
  if (cmd == "") return;

  for (const auto &arg : args)
    cmd += " " + arg;

  tk_->eval(cmd);
}

bool
CTkAppWidget::
getOptionValue(const std::string &optName, const std::string &optClass,
               std::string &optValue) const
{
  const auto &opts = getWidgetCommand()->getOpts();

  if (opts.getDefValue(optName, optClass, optValue)) {
    //std::cerr << "getOptionValue: " << optName << " " << optClass << " = " << optValue << "\n";
    return true;
  }

  auto widgetClass = std::string(getClassName());

  if (getOptionClass() != "")
    widgetClass = getOptionClass();

  if (tk_->matchOption(widgetClass, optName, optClass, optValue)) {
    //std::cerr << "getOptionValue: " << optName << " " << optClass << " = " << optValue << "\n";
    return true;
  }

  std::cerr << "Error in getOptionValue: " <<
    widgetClass << ":" << optName << " " << optClass << " = " << optValue << "\n";

  return true;
}

void
CTkAppWidget::
processEvents(QEvent *e, const CTkAppEventData &matchEventData)
{
  for (const auto &eventData : eventDatas_) {
    if (eventData == matchEventData) {
      tk_->execEvent(this, e, matchEventData, eventData.command);
    }
  }
}

//----------

bool
CTkAppWidgetEventFilter::
eventFilter(QObject *obj, QEvent *event)
{
  if (w_->isDeleted()) return false;

  switch (event->type()) {
    case QEvent::Enter: {
      CTkAppEventData matchEventData;

      matchEventData.type = CTkAppEventType::Enter;

      w_->triggerEvents(event, matchEventData);

      break;
    }
    case QEvent::Leave: {
      CTkAppEventData matchEventData;

      matchEventData.type = CTkAppEventType::Leave;

      w_->triggerEvents(event, matchEventData);

      break;
    }
    case QEvent::Expose:
    case QEvent::Paint: {
      CTkAppEventData matchEventData;

      matchEventData.type = CTkAppEventType::Expose;

      w_->triggerEvents(event, matchEventData);

      break;
    }

    case QEvent::MouseButtonPress: {
      auto *me = static_cast<QMouseEvent *>(event);

      pressed_     = true;
      pressButton_ = me->button();

      w_->triggerMousePressEvents(event, pressButton_);

      break;
    }
    case QEvent::MouseMove: {
      w_->triggerMouseMoveEvents(event, pressButton_, pressed_);

      break;
    }
    case QEvent::MouseButtonRelease: {
      w_->triggerMouseReleaseEvents(event, pressButton_);

      pressed_     = false;
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

//---

CTkAppSubMenu::
CTkAppSubMenu(CTkApp *tk) :
 QMenu(nullptr), tk_(tk)
{
  connectSlots(true);
}

void
CTkAppSubMenu::
connectSlots(bool b)
{
  if (b) {
    connect(this, SIGNAL(aboutToShow()), this, SLOT(showSlot()));
    connect(this, SIGNAL(aboutToHide()), this, SLOT(hideSlot()));
  }
  else {
    disconnect(this, SIGNAL(aboutToShow()), this, SLOT(showSlot()));
    disconnect(this, SIGNAL(aboutToHide()), this, SLOT(hideSlot()));
  }
}

void
CTkAppSubMenu::
showSlot()
{
  std::cerr << "Show " << menu_ << "\n";

  auto *w    = tk_->lookupWidgetByName(menu_);
  auto *menu = dynamic_cast<CTkAppMenu *>(w);
  if (! menu) { std::cerr << "No menu '" << menu_ << "'\n"; return; }

  auto actions = menu->qmenu()->actions();

  for (auto *action : actions) {
    std::cerr << "Add Action " << action->text().toStdString() << "\n";

    //menu->qmenu()->removeAction(action);
    //action->setParent(this);

    this->addAction(action);
  }
}

void
CTkAppSubMenu::
hideSlot()
{
  std::cerr << "Hide " << menu_ << "\n";

  auto *w    = tk_->lookupWidgetByName(menu_);
  auto *menu = dynamic_cast<CTkAppMenu *>(w);
  if (! menu) { std::cerr << "No menu '" << menu_ << "'\n"; return; }

  auto actions = this->actions();

  for (auto *action : actions) {
    std::cerr << "Remove Action " << action->text().toStdString() << "\n";

    //action->setParent(menu->qmenu());

    menu->qmenu()->addAction(action);
  }
}
