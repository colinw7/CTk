#include <CTkAppWidget.h>
#include <CTkApp.h>
#include <CTkAppPackLayout.h>
#include <CTkAppPlaceLayout.h>
#include <CTkAppGridLayout.h>
#include <CTkAppImage.h>

#include <CQSlider.h>
#include <CQSpinList.h>
#include <CQLabelImage.h>

#include <CQUtil.h>
#include <CQImage.h>
#include <CStrMap.h>
#include <CStrParse.h>
#include <CRGBName.h>
#include <CScreenUnits.h>

#include <QApplication>
#include <QCheckBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSplitter>
#include <QTextEdit>
#include <QToolButton>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>

namespace {

QColor stringToQColor(const std::string &str) {
  double r = 0.0, g = 0.0, b = 0.0, a = 0.0;

  if (str != "")
    CRGBName::lookup(str, &r, &g, &b, &a);

  return QColor(255*r, 255*g, 255*b, 255*a);
}

QFont stringToQFont(const std::string &str) {
  QFont f;

  CStrParse parse(str);

  parse.skipSpace();

  std::string family;

  if (! parse.readNonSpace(family))
    return f;

  std::cerr << family << " ";

  f.setFamily(QString::fromStdString(family));

  //---

  parse.skipSpace();

  int  size   = 0;
  bool points = true;

  if (! parse.readInteger(&size))
    return f;

  if (size < 0) {
    size   = -size;
    points = false;
  }

  std::cerr << size << (points ? "pt" : "px") << " ";

  if (points)
    f.setPointSize(size);
  else
    f.setPixelSize(size);

  //---

  parse.skipSpace();

  std::vector<std::string> styles;

  while (! parse.eof()) {
    std::string style;

    if (! parse.readNonSpace(style))
      return f;

    styles.push_back(style);
  }

  for (const auto &style : styles)
    std::cerr << style << " ";

  for (const auto &style : styles) {
    if      (style == "bold")
      f.setBold(true);
    else if (style == "italic")
      f.setItalic(true);
    else
      std::cerr << "Unhandled style '" << style << "'\n";
  }

  //---

  std::cerr << "\n";

  return f;
}

bool stringToInteger(const std::string &str, int &i) {
  return CStrUtil::toInteger(str, &i);
}

bool stringToReal(const std::string &str, double &r) {
  return CStrUtil::toReal(str, &r);
}

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

bool stringToDistance(const std::string &str, double &r) {
  CStrParse parse(str);

  parse.skipSpace();

  double r1;

  if (! parse.readReal(&r1))
    return false;

  parse.skipSpace();

  if (parse.eof()) {
    r = r1;
    return true;
  }

  auto c = parse.readChar();

  double f = 1.0;

  if      (c == 'c')
    f = 1.0/2.54;
  else if (c == 'i')
    f = 1.0;
  else if (c == 'm')
    f = 1.0/25.4;
  else if (c == 'p')
    f = 1.0/72.0;
  else
    return false;

  auto dpi = CScreenUnitsMgrInst->dpi();

  r = r1*dpi*f;

  return true;
}

bool stringToDistanceI(const std::string &str, int &i) {
  double r;
  if (! stringToDistance(str, r))
    return false;
  i = int(r);
  return true;
}

bool stringToOrient(const std::string &str, Qt::Orientation &orient) {
  static CStrMap<Qt::Orientation, int> orientMap(
    "horizontal", Qt::Horizontal,
    "vertical"  , Qt::Vertical  ,
    0);

  return orientMap.map(str, orient);
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

void setWidgetAnchor(CTkAppWidget *w, const std::string &value)
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
  else std::cerr << "Invalid anchor '" << value << "'\n";

  w->setAlign(align);
}

// set borderwidth
void setBorderWidth(QWidget *w, int width) {
  auto *frame = qobject_cast<QFrame *>(w);

  if (frame)
    frame->setLineWidth(width);
}

}

//---

CTkAppRoot::
CTkAppRoot(CTkApp *tk) :
 CTkAppWidget(tk, nullptr, "")
{
  qframe_ = new CTkRootWidget(this);

  setQWidget(qframe_);
}

void
CTkAppRoot::
show()
{
  auto s = qframe_->sizeHint();

  qframe_->resize(s.expandedTo(QApplication::globalStrut()));

  CTkAppWidget::show();
}

bool
CTkAppRoot::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-background" || name == "-bg") {
    auto c = stringToQColor(value);

    CQUtil::setBackground(qframe_, c);
  }
  else if (name == "-borderwidth" || name == "-bd") {
    int w;

    if (! tk_->getOptionInt(name, value, w))
      return false;

    setBorderWidth(qframe_, w);
  }
  else if (name == "-padx") {
    int padx;

    if (! tk_->getOptionInt(name, value, padx))
      return false;

    setPadX(padx);
  }
  else if (name == "-pady") {
    int pady;

    if (! tk_->getOptionInt(name, value, pady))
      return false;

    setPadY(pady);
  }
  else if (name == "-menu") {
    auto *w = tk_->lookupWidgetByName(value);

    auto *menu = dynamic_cast<CTkMenu *>(w);
    if (! menu) { std::cerr << "No menu '" << value << "'\n"; return false; }

    qframe_->setMenu(menu->qmenu());
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

CTkRootWidget::
CTkRootWidget(CTkAppRoot *root) :
 QFrame(nullptr), root_(root)
{
  setObjectName("qroot");
}

void
CTkRootWidget::
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
CTkRootWidget::
paintEvent(QPaintEvent *e)
{
  QFrame::paintEvent(e);

//QPainter painter(this);
//auto *pack = root_->getTkPackLayout();
//if (pack) pack->draw(&painter);
}

QSize
CTkRootWidget::
sizeHint() const
{
  auto s = QFrame::sizeHint();

  if (s.width() <= 0 || s.height() <= 0)
    s = QSize(400, 400);

  return s;
}

//----------

CTkButton::
CTkButton(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qbutton_ = new QPushButton(parent_->getQWidget());
  else
    qbutton_ = new QPushButton(nullptr);

  setQWidget(qbutton_);

  connectSlots(true);
}

void
CTkButton::
connectSlots(bool b)
{
  if (b)
    connect(qbutton_, SIGNAL(clicked()), this, SLOT(clickSlot()));
  else
    disconnect(qbutton_, SIGNAL(clicked()), this, SLOT(clickSlot()));
}

bool
CTkButton::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-command")
    setCommand(value);
  else if (name == "-text")
    setText(value);
  else if (name == "-bitmap") {
    auto image = tk_->loadImage(value);

    if (! image)
      return tk_->throwError("Failed to load bitmap '" + value + "'");

    setImage(image);
  }
  else if (name == "-image") {
    auto image = tk_->getImage(value);

    if (image)
      setImage(image->getImage());
  }
  else if (name == "-default") {
    tk_->TODO(name);
  }
  else if (name == "-state") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkButton::
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
  tk_->TODO();
}

//----------

CTkAppCanvas::
CTkAppCanvas(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qcanvas_ = new CTkAppCanvasWidget(parent_->getQWidget());
  else
    qcanvas_ = new CTkAppCanvasWidget();

  setQWidget(qcanvas_);

  qwidget_->setFocusPolicy(Qt::StrongFocus);
}

bool
CTkAppCanvas::
execConfig(const std::string &name, const std::string &value)
{
  return CTkAppWidget::execConfig(name, value);
}

bool
CTkAppCanvas::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  auto processShapeOpt = [&](CTkAppCanvasWidget::Shape *shape, const std::string &name,
                             const std::string &value) {
    if      (name == "-fill") {
      auto c = stringToQColor(value);

      auto b = shape->brush();
      b.setStyle(Qt::SolidPattern);
      b.setColor(c);
      shape->setBrush(b);
    }
    else if (name == "-outline") {
      auto c = stringToQColor(value);

      auto p = shape->pen();
      p.setColor(c);
      shape->setPen(p);
    }
    else if (name == "-width") {
      double width;

      if (! stringToDistance(value, width))
        return tk_->throwError("Invalid width \"" + value + "\"");

      auto p = shape->pen();
      p.setWidthF(width);
      shape->setPen(p);
    }
    else if (name == "-tags") {
      std::vector<std::string> strs;

      if (! tk_->splitList(value, strs))
        return tk_->throwError("Invalid tags \"" + value + "\"");

      shape->setTags(strs);
    }
    else
      return false;

    return true;
  };

  const auto &arg = args[0];

  if      (arg == "bind") {
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
  else if (arg == "create") {
    if (numArgs == 1)
      return tk_->wrongNumArgs(getName() + " create type coords ?arg ...?");

    const auto &type = args[1];

    if      (type == "arc") {
      tk_->TODO(type);
    }
    else if (type == "bitmap") {
      tk_->TODO(type);
    }
    else if (type == "image") {
      tk_->TODO(type);
    }
    else if (type == "line") {
      CTkAppCanvasWidget::Points points;

      uint i = 2;

      for ( ; i < numArgs; i += 2) {
        if (args[i][0] == '-')
          break;

        double x, y;
        if (! stringToDistance(args[i], x) || ! stringToDistance(args[i + 1], y))
          return tk_->throwError("Invalid coord \"" + args[i] + " " + args[i + 1] + "\"");

        points.push_back(CTkAppCanvasWidget::Point(x, y));
      }

      if (points.empty())
        return tk_->wrongNumArgs(getName() + " create line coords ?arg ...?");

      auto *shape = qcanvas_->addLine(points);

      for ( ; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (processShapeOpt(shape, name, value))
            continue;
          else
            tk_->TODO(args[i]);
        }
        else
          tk_->TODO(args[i]);
      }

      tk_->setIntegerResult(shape->id());
    }
    else if (type == "oval") {
      if (numArgs < 6)
        return tk_->wrongNumArgs(getName() + " create oval coords ?arg ...?");

      double x1, y1, x2, y2;
      if (! stringToDistance(args[2], x1) || ! stringToDistance(args[3], y1) ||
          ! stringToDistance(args[4], x2) || ! stringToDistance(args[5], y2))
        return tk_->throwError("Invalid coords \"" + args[2] + " " + args[3] + " " +
                               args[4] + " " + args[5] + "\"");

      auto *shape = qcanvas_->addOval(x1, y1, x2, y2);

      for (uint i = 6; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (processShapeOpt(shape, name, value))
            continue;
          else
            tk_->TODO(name);
        }
        else
          tk_->TODO(args[i]);
      }

      tk_->setIntegerResult(shape->id());
    }
    else if (type == "polygon") {
      tk_->TODO(type);
    }
    else if (type == "rectangle") {
      if (numArgs < 6)
        return tk_->wrongNumArgs(getName() + " create rectangle coords ?arg ...?");

      double x1, y1, x2, y2;
      if (! stringToDistance(args[2], x1) || ! stringToDistance(args[3], y1) ||
          ! stringToDistance(args[4], x2) || ! stringToDistance(args[5], y2))
        return tk_->throwError("Invalid coords \"" + args[2] + " " + args[3] + " " +
                               args[4] + " " + args[5] + "\"");

      auto *shape = qcanvas_->addRectangle(x1, y1, x2, y2);

      for (uint i = 6; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (processShapeOpt(shape, name, value))
            continue;
          else
            tk_->TODO(name);
        }
        else
          tk_->TODO(args[i]);
      }

      tk_->setIntegerResult(shape->id());
    }
    else if (type == "text") {
      if (numArgs < 4)
        return tk_->wrongNumArgs(getName() + " create text coord ?arg ...?");

      double x, y;
      if (! stringToDistance(args[2], x) || ! stringToDistance(args[3], y))
        return tk_->throwError("Invalid coord \"" + args[2] + " " + args[3] + "\"");

      CTkAppCanvasWidget::Point pos(x, y);

      auto *shape = qcanvas_->addText(pos, "");

      for (uint i = 4; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (processShapeOpt(shape, name, value))
            continue;

          if      (name == "-text")
            shape->setText(value);
          else if (name == "-anchor")
            tk_->TODO(name);
          else
            tk_->TODO(name);
        }
        else
          tk_->TODO(args[i]);
      }

      tk_->setIntegerResult(shape->id());
    }
    else if (type == "window") {
      tk_->TODO(type);
    }
    else
      tk_->TODO(type);
  }
  else if (arg == "delete") {
    if (numArgs == 1)
      return tk_->wrongNumArgs(getName() + " delete ?arg ...?"); // delete all ?

    const auto &name = args[1];

    int id;

    if (! tk_->getOptionInt(arg, name, id))
      return false;

    if (! qcanvas_->deleteShape(id))
      return false;
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
  else if (arg == "lower") {
    if (numArgs == 1)
      return tk_->wrongNumArgs(getName() + " lower tagOrId ?belowThis?");

    const auto &name = args[1];

    int id;

    if (! tk_->getOptionInt(arg, name, id))
      return false;

    if (! qcanvas_->lowerShape(id))
      return false;
  }
  else if (arg == "move") {
    if (numArgs != 4)
      return tk_->wrongNumArgs(getName() + " move tagOrId xAmount yAmount");

    const auto &id = args[1];

    double dx, dy;
    if (! stringToDistance(args[2], dx) || ! stringToDistance(args[3], dy))
      return tk_->throwError("Invalid coord \"" + args[2] + " " + args[3] + "\"");

    if (! qcanvas_->moveShape(id, dx, dy))
      return false;
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
        if (! stringToDistance(args[i], x) || ! stringToDistance(args[i + 1], y))
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
  else if (arg == "itemconfigure") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " itemconfigure tagOrId ?-option value ...?");

    const auto &id = args[1];

    CTkAppCanvasShape *shape = nullptr;

    if (id == "current")
      shape = insideShape_;

    if (! shape)
      return tk_->throwError("Unhandled shape \"" + id + " for itemconfigure");

    for (uint i = 2; i < numArgs; i+= 2) {
      const auto &name = args[i];

      if (name[0] == '-') {
        const auto &value = args[i + 1];

        if (processShapeOpt(shape, name, value))
          continue;

        tk_->TODO(name);
      }
      else
        tk_->TODO(name);
    }
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

    //---

    for (auto &tag : insideShape_->tags()) {
      auto pi = idEventDatas_.find(tag);
      if (pi == idEventDatas_.end()) continue;

      for (const auto &eventData : (*pi).second) {
        if (eventData == matchEventData) {
          tk_->execEvent(this, e, eventData);
        }
      }
    }
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
              tk_->execEvent(this, e, eventData);
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
              tk_->execEvent(this, e, eventData);
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

      //---

      for (auto &tag : insideShape_->tags()) {
        auto pi = idEventDatas_.find(tag);
        if (pi == idEventDatas_.end()) continue;

        for (const auto &eventData : (*pi).second) {
          if (eventData == matchEventData) {
            tk_->execEvent(this, e, eventData);
          }
        }
      }
    }
  }

  return CTkAppWidget::triggerMouseMoveEvents(e, button, pressed);
}

//---

CTkAppCanvasWidget::
CTkAppCanvasWidget(QWidget *parent) :
 QWidget(parent)
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
      for (uint i = 0; i < n - 1; ++i) {
        const auto &point1 = points[i    ];
        const auto &point2 = points[i + 1];

        p->drawLine(point1.x, point1.y, point2.x, point2.y);
      }
      break;
    }
    case CTkAppCanvasShapeType::TEXT: {
      auto *t = static_cast<Text *>(s);
      p->drawText(t->pos().x, t->pos().y, QString::fromStdString(t->text()));
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
  return QSize(400, 400);
}

//----------

class CTkCheckButtonVarProc : public CTclTraceProc {
 public:
  CTkCheckButtonVarProc(CTkApp *tk, CTkCheckButton *check) :
   CTclTraceProc(tk), check_(check) {
  }

  void handleWrite(const char *) override {
    check_->updateVariable();
  }

 private:
  CTkCheckButton *check_ { nullptr };
};

CTkCheckButton::
CTkCheckButton(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qcheck_ = new QCheckBox(parent_->getQWidget());
  else
    qcheck_ = new QCheckBox(nullptr);

  setQWidget(qcheck_);

  connectSlots(true);
}

void
CTkCheckButton::
connectSlots(bool b)
{
  if (b)
    connect(qcheck_, SIGNAL(stateChanged(int)), this, SLOT(stateChangedSlot(int)));
  else
    disconnect(qcheck_, SIGNAL(stateChanged(int)), this, SLOT(stateChangedSlot(int)));
}

bool
CTkCheckButton::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-text")
    setText(value);
  else if (name == "-variable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasVar(varName_))
      tk_->setStringVar(varName_, "");

    updateVariable();

    varProc_ = new CTkCheckButtonVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-value") {
    bool b;

    if (CStrUtil::toBool(value, &b))
      setChecked(b);
  }
  else if (name == "-tristatevalue") {
    tk_->TODO(name);
  }
  else if (name == "-onvalue") {
    tk_->TODO(name);
  }
  else if (name == "-offvalue") {
    tk_->TODO(name);
  }
  else if (name == "-relief") {
    setFrameRelief(qcheck_, value);
  }
  else if (name == "-anchor") {
    setWidgetAnchor(this, value);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkCheckButton::
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
CTkCheckButton::
setText(const std::string &text)
{
  qcheck_->setText(QString::fromStdString(text));
}

void
CTkCheckButton::
updateVariable()
{
  if (varName_ != "" && tk_->hasVar(varName_))
    setChecked(tk_->getBoolVar(varName_));
}

void
CTkCheckButton::
setChecked(bool b)
{
  if (b != qcheck_->isChecked()) {
    connectSlots(false);

    qcheck_->setChecked(b);

    connectSlots(true);
  }
}

void
CTkCheckButton::
stateChangedSlot(int)
{
  if (varName_ != "") {
    varProc_->setEnabled(false);

    if (! tk_->hasVar(varName_))
      tk_->setBoolVar(varName_, false);

    tk_->setBoolVar(varName_, qcheck_->isChecked());

    varProc_->setEnabled(true);
  }
}

//----------

class CTkEntryVarProc : public CTclTraceProc {
 public:
  CTkEntryVarProc(CTkApp *tk, CTkEntry *entry) :
   CTclTraceProc(tk), entry_(entry) {
  }

  void handleWrite(const char *) override {
    entry_->updateVariable();
  }

 private:
  CTkEntry *entry_ { nullptr };
};

class CTkEntryValidator : public QValidator {
 public:
  CTkEntryValidator(CTkApp *tk, CTkEntry *entry) :
   QValidator(entry), tk_(tk), entry_(entry) {
  }

  State validate(QString &input, int & /*pos*/) const override {
    if (entry_->validate(input.toStdString()))
      return Acceptable;

    return Invalid;
  }

 private:
  CTkApp*   tk_    { nullptr };
  CTkEntry* entry_ { nullptr };
};

CTkEntry::
CTkEntry(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qedit_ = new QLineEdit(parent_->getQWidget());
  else
    qedit_ = new QLineEdit(nullptr);

  setQWidget(qedit_);

  qwidget_->setFocusPolicy(Qt::StrongFocus);

  connectSlots(true);
}

void
CTkEntry::
connectSlots(bool b)
{
  if (b)
    connect(qedit_, SIGNAL(editingFinished()), this, SLOT(valueChangedSlot()));
  else
    disconnect(qedit_, SIGNAL(editingFinished()), this, SLOT(valueChangedSlot()));
}

bool
CTkEntry::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasVar(varName_))
      tk_->setStringVar(varName_, "");

    if (tk_->hasVar(varName_))
      setText(tk_->getStringVar(varName_));

    varProc_ = new CTkEntryVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-borderwidth" || name == "-bd") {
    int w;

    if (! tk_->getOptionInt(name, value, w))
      return false;

    setBorderWidth(qedit_, w);
  }
  else if (name == "-relief") {
    setFrameRelief(qedit_, value);
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
      validateProc_ = new CTkEntryValidator(tk_, this);

      qedit_->setValidator(validateProc_);
    }
  }
  else if (name == "-validatecommand") {
    validateCmd_ = value;
  }
  else if (name == "-takefocus") {
    tk_->TODO(name);
  }
  else if (name == "-highlightthickness") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkEntry::
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
CTkEntry::
setText(const std::string &text)
{
  qedit_->setText(QString::fromStdString(text));
}

void
CTkEntry::
updateVariable()
{
  if (varName_ != "" && tk_->hasVar(varName_))
    setText(tk_->getStringVar(varName_));
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

bool
CTkEntry::
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

    if (! CStrUtil::toBool(res, &b))
      return false;

    return b;
  }

  return true;
}

//----------

CTkFrame::
CTkFrame(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qframe_ = new QFrame(parent_->getQWidget());
  else
    qframe_ = new QFrame(nullptr);

  setQWidget(qframe_);
}

bool
CTkFrame::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-relief") {
    setFrameRelief(qframe_, value);
  }
  else if (name == "-borderwidth" || name == "-bd") {
    int w;

    if (! tk_->getOptionInt(name, value, w))
      return false;

    setBorderWidth(qframe_, w);
  }
  else if (name == "-class") {
    tk_->TODO(name);
  }
  else if (name == "-takefocus") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkFrame::
execOp(const Args &args)
{
  return CTkAppWidget::execOp(args);
}

//----------

class CTkLabelVarProc : public CTclTraceProc {
 public:
  CTkLabelVarProc(CTkApp *tk, CTkLabel *label) :
   CTclTraceProc(tk), label_(label) {
  }

  void handleWrite(const char *) override {
    label_->updateVariable();
  }

 private:
  CTkLabel *label_ { nullptr };
};

CTkLabel::
CTkLabel(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qlabel_ = new CQLabelImage(parent_->getQWidget());
  else
    qlabel_ = new CQLabelImage(nullptr);

  setQWidget(qlabel_);
}

bool
CTkLabel::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-text")
    setText(value);
  else if (name == "-bitmap") {
    auto image = tk_->loadImage(value);

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

    varProc_ = new CTkLabelVarProc(tk_, this);

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
  else if (name == "-borderwidth") {
    tk_->TODO(name);
  }
  else if (name == "-relief") {
    tk_->TODO(name);
  }
  else if (name == "-highlightthickness") {
    tk_->TODO(name);
  }
  else if (name == "-takefocus") {
    tk_->TODO(name);
  }
  else if (name == "-anchor") {
    setWidgetAnchor(this, value);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkLabel::
execOp(const Args &args)
{
  return CTkAppWidget::execOp(args);
}

void
CTkLabel::
setText(const std::string &text)
{
  qlabel_->setLabel(QString::fromStdString(text));
}

void
CTkLabel::
setImage(CImagePtr image)
{
  auto &qimage = image.cast<CQImage>()->getQImage();

  //QPixmap pixmap;
  //pixmap.convertFromImage(qimage);
  //qlabel_->setPixmap(pixmap);

  qlabel_->setImage(qimage);
}

void
CTkLabel::
updateVariable()
{
  if (varName_ != "" && tk_->hasVar(varName_))
    setText(tk_->getStringVar(varName_));
}

//----------

CTkLabelFrame::
CTkLabelFrame(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qframe_ = new QGroupBox(parent_->getQWidget());
  else
    qframe_ = new QGroupBox(nullptr);

  setQWidget(qframe_);
}

bool
CTkLabelFrame::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-text") {
    setText(value);
  }
  else if (name == "-padx") {
    int padx;

    if (! tk_->getOptionInt(name, value, padx))
      return false;

    setPadX(padx);
  }
  else if (name == "-pady") {
    int pady;

    if (! tk_->getOptionInt(name, value, pady))
      return false;

    setPadY(pady);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkLabelFrame::
execOp(const Args &args)
{
  return CTkAppWidget::execOp(args);
}

void
CTkLabelFrame::
setText(const std::string &text)
{
  qframe_->setTitle(QString::fromStdString(text));
}

//----------

class CTkListBoxVarProc : public CTclTraceProc {
 public:
  CTkListBoxVarProc(CTkApp *tk, CTkListBox *listBox) :
   CTclTraceProc(tk), listBox_(listBox) {
  }

  void handleWrite(const char *) override {
    listBox_->updateVariable();
  }

 private:
  CTkListBox *listBox_ { nullptr };
};

CTkListBox::
CTkListBox(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qlist_ = new QListWidget(parent_->getQWidget());
  else
    qlist_ = new QListWidget(nullptr);

  setQWidget(qlist_);

  qlist_->verticalScrollBar  ()->setVisible(false);
  qlist_->horizontalScrollBar()->setVisible(false);

  connectSlots(true);
}

void
CTkListBox::
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
CTkListBox::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-listvariable") {
    varName_ = value;

    updateVariable();

    varProc_ = new CTkListBoxVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-relief") {
    setFrameRelief(qlist_, value);
  }
  else if (name == "-borderwidth" || name == "-bd") {
    int w;

    if (! tk_->getOptionInt(name, value, w))
      return false;

    setBorderWidth(qlist_, w);
  }
  else if (name == "-xscrollcommand") {
    xScrollCommand_ = value;
  }
  else if (name == "-yscrollcommand") {
    yScrollCommand_ = value;
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
CTkListBox::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "xview") {
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

        if (! stringToReal(opt, x))
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

        if (! stringToReal(opt, y))
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
CTkListBox::
updateVariable()
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

void
CTkListBox::
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
CTkListBox::
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

//----------

CTkMenu::
CTkMenu(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qmenu_ = new QMenu(parent_->getQWidget());
  else
    qmenu_ = new QMenu(nullptr);

  setQWidget(qmenu_);

  auto *menuButton = dynamic_cast<CTkMenuButton *>(parent);

  if (menuButton)
    menuButton->updateMenu();
}

bool
CTkMenu::
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
CTkMenu::
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
      auto *menu = new CTkSubMenu(tk_);

      for (uint i = 2; i < numArgs - 1; i += 2) {
        const auto &name  = args[i    ];
        const auto &value = args[i + 1];

        if      (name == "-label") {
          menu->setTitle(QString::fromStdString(value));
        }
        else if (name == "-menu") {
          menu->setMenu(value);
        }
        else if (name == "-underline") {
          tk_->TODO(name);
        }
        else
          tk_->throwError("Invalid menu cascade option \"" + name + "\"");
      }

      qmenu_->addMenu(menu);
    }
    else if (type == "check" || type == "checkbutton") {
      auto *check = new CTkCheckAction(tk_);

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
          tk_->TODO(name);
        }
        else
          tk_->throwError("Invalid menu check option \"" + name + "\"");
      }

      connect(check, SIGNAL(stateChanged(int)), this, SLOT(checkChangedSlot(int)));

      qmenu_->addAction(check);
    }
    else if (type == "command") {
      auto *action = new QAction("");

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
          tk_->TODO(name);
        }
        else if (name == "-accelerator") {
          tk_->TODO(name);
        }
        else
          tk_->throwError("Invalid menu command option \"" + name + "\"");
      }

      qmenu_->addAction(action);
    }
    else if (type == "radio" || type == "radiobutton") {
      auto *radio = new CTkRadioAction(tk_);

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
CTkMenu::
actionPressedSlot()
{
  auto *action = qobject_cast<QAction *>(sender());
  if (! action) return;

  auto cmd = action->property("CTkApp::Command").toString();

  if (cmd != "")
    tk_->eval(cmd.toLatin1().constData());
}

void
CTkMenu::
checkChangedSlot(int)
{
  auto *action = qobject_cast<CTkCheckAction *>(sender());
  if (! action) return;

  action->updateVariable();
}

void
CTkMenu::
radioToggledSlot(bool b)
{
  auto *action = qobject_cast<CTkRadioAction *>(sender());
  if (! action) return;

  if (b)
    action->updateVariable();
}

//---

void
CTkCheckAction::
updateVariable()
{
  if (varName_ != "")
    tk_->setBoolVar(varName_, check_->isChecked());
}

void
CTkRadioAction::
updateVariable()
{
  if (varName_ != "") {
    if (value_ != "")
      tk_->setStringVar(varName_, value_);
    else
      tk_->setBoolVar(varName_, radio_->isChecked());
  }
}

//----------

class CTkMenuButtonVarProc : public CTclTraceProc {
 public:
  CTkMenuButtonVarProc(CTkApp *tk, CTkMenuButton *menuButton) :
   CTclTraceProc(tk), menuButton_(menuButton) {
  }

  void handleWrite(const char *) override {
    menuButton_->updateVariable();
  }

 private:
  CTkMenuButton *menuButton_ { nullptr };
};

CTkMenuButton::
CTkMenuButton(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qbutton_ = new QToolButton(parent_->getQWidget());
  else
    qbutton_ = new QToolButton(nullptr);

  qbutton_->setToolButtonStyle(Qt::ToolButtonTextOnly);

  setQWidget(qbutton_);

  connectSlots(true);
}

void
CTkMenuButton::
connectSlots(bool b)
{
  if (b)
    connect(qbutton_, SIGNAL(clicked()), this, SLOT(clickSlot()));
  else
    disconnect(qbutton_, SIGNAL(clicked()), this, SLOT(clickSlot()));
}

bool
CTkMenuButton::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-text")
    setText(value);
  else if (name == "-command")
    setCommand(value);
  else if (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasVar(varName_))
      tk_->setStringVar(varName_, "");

    if (tk_->hasVar(varName_))
      setText(tk_->getStringVar(varName_));

    varProc_ = new CTkMenuButtonVarProc(tk_, this);

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
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkMenuButton::
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
updateVariable()
{
  if (varName_ != "" && tk_->hasVar(varName_))
    setText(tk_->getStringVar(varName_));
}

void
CTkMenuButton::
updateMenu()
{
  if (menuName_ != "") {
    auto *w = tk_->lookupWidgetByName(menuName_, /*quiet*/true);
    if (! w) return; // not defined yet

    auto *menu = dynamic_cast<CTkMenu *>(w);
    if (! menu) { std::cerr << "No menu '" << menuName_ << "'\n"; return; }

    setMenu(menu);
  }
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

  void handleWrite(const char *) override {
    message_->updateVariable();
  }

 private:
  CTkMessage *message_ { nullptr };
};

CTkMessage::
CTkMessage(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
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

    varProc_ = new CTkMessageVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-relief") {
    setFrameRelief(qedit_, value);
  }
  else if (name == "-borderwidth" || name == "-bd") {
    int w;

    if (! tk_->getOptionInt(name, value, w))
      return false;

    setBorderWidth(qedit_, w);
  }
  else if (name == "-padx") {
    int padx;

    if (! tk_->getOptionInt(name, value, padx))
      return false;

    setPadX(padx);
  }
  else if (name == "-pady") {
    int pady;

    if (! tk_->getOptionInt(name, value, pady))
      return false;

    setPadY(pady);
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
CTkMessage::
execOp(const Args &args)
{
  return CTkAppWidget::execOp(args);
}

void
CTkMessage::
setText(const std::string &text)
{
  qedit_->setText(QString::fromStdString(text));
}

void
CTkMessage::
updateVariable()
{
  if (varName_ != "" && tk_->hasVar(varName_))
    setText(tk_->getStringVar(varName_));
}

//----------

CTkPanedWindow::
CTkPanedWindow(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
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
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-orient") {
    Qt::Orientation orient;

    if (! stringToOrient(value, orient))
      return false;

    qpane_->setOrientation(orient);
  }
  else if (name == "-showhandle") {
    tk_->TODO(name);
  }
  else if (name == "-padx") {
    int padx;

    if (! tk_->getOptionInt(name, value, padx))
      return false;

    setPadX(padx);
  }
  else if (name == "-pady") {
    int pady;

    if (! tk_->getOptionInt(name, value, pady))
      return false;

    setPadY(pady);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkPanedWindow::
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

          int i;

          if (! tk_->getOptionInt(name, value, i))
            return false;

          min_size = i;
        }
        else if (name == "-padx") {
          ++i;

          if (i >= numArgs)
            return tk_->throwError("Missing value for \"-padx\"");

          int pad;

          if (! tk_->getOptionInt(name, args[i], pad))
            return false;

          padx = pad;

          tk_->TODO("padx " + std::to_string(padx) + " not supported");
        }
        else if (name == "-pady") {
          ++i;

          if (i >= numArgs)
            return tk_->throwError("Missing value for \"-pady\"");

          int pad;

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

class CTkRadioButtonVarProc : public CTclTraceProc {
 public:
  CTkRadioButtonVarProc(CTkApp *tk, CTkRadioButton *radio) :
   CTclTraceProc(tk), radio_(radio) {
  }

  void handleWrite(const char *) override {
    radio_->updateVariable();
  }

 private:
  CTkRadioButton *radio_ { nullptr };
};

CTkRadioButton::
CTkRadioButton(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qradio_ = new QRadioButton(parent_->getQWidget());
  else
    qradio_ = new QRadioButton(nullptr);

  setQWidget(qradio_);

  connectSlots(true);
}

void
CTkRadioButton::
connectSlots(bool b)
{
  if (b)
    connect(qradio_, SIGNAL(toggled(bool)), this, SLOT(toggleSlot(bool)));
  else
    disconnect(qradio_, SIGNAL(toggled(bool)), this, SLOT(toggleSlot(bool)));
}

bool
CTkRadioButton::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-text")
    setText(value);
  else if (name == "-variable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasVar(varName_))
      tk_->setStringVar(varName_, "");

    updateVariable();

    varProc_ = new CTkRadioButtonVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-value") {
    value_ = value;

    updateVariable();
  }
  else if (name == "-tristatevalue") {
    tk_->TODO(name);
  }
  else if (name == "-anchor") {
    setWidgetAnchor(this, value);
  }
  else if (name == "-relief") {
    setFrameRelief(qradio_, value);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkRadioButton::
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
CTkRadioButton::
setText(const std::string &text)
{
  qradio_->setText(QString::fromStdString(text));
}

void
CTkRadioButton::
updateVariable()
{
  if (varName_ != "" && tk_->hasVar(varName_))
    setValue(tk_->getStringVar(varName_));
}

void
CTkRadioButton::
setValue(const std::string &value)
{
  bool checked = qradio_->isChecked();

  if (value_ != "")
    checked = (value == value_);
  else {
    bool b;

    if (CStrUtil::toBool(value, &b))
      checked = b;
  }

  setChecked(checked);
}

void
CTkRadioButton::
setChecked(bool b)
{
  if (b != qradio_->isChecked()) {
    connectSlots(false);

    qradio_->setChecked(b);

    connectSlots(true);
  }
}

void
CTkRadioButton::
toggleSlot(bool)
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

//----------

class CTkScaleVarProc : public CTclTraceProc {
 public:
  CTkScaleVarProc(CTkApp *tk, CTkScale *scale) :
   CTclTraceProc(tk), scale_(scale) {
  }

  void handleWrite(const char *) override {
    scale_->updateVariable();
  }

 private:
  CTkScale *scale_ { nullptr };
};

CTkScale::
CTkScale(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qscale_ = new CQRealSlider(parent_->getQWidget());
  else
    qscale_ = new CQRealSlider(nullptr);

  qscale_->setValuePosition(CQSliderBase::ValuePosition::NONE);

  setQWidget(qscale_);

  connectSlots(true);
}

void
CTkScale::
connectSlots(bool b)
{
  if (b)
    connect(qscale_, SIGNAL(valueChanged(int)), this, SLOT(valueSlot(int)));
  else
    disconnect(qscale_, SIGNAL(valueChanged(int)), this, SLOT(valueSlot(int)));
}

bool
CTkScale::
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
    int n;

    if (! tk_->getOptionInt(name, value, n))
      return false;

    qscale_->setPrecision(n);
  }
  else if (name == "-from") {
    int from;

    if (! tk_->getOptionInt(name, value, from))
      return false;

    qscale_->setMinimum(from);
  }
  else if (name == "-label") {
    qscale_->setLabel(QString::fromStdString(value));
  }
  else if (name == "-length") {
    int l;

    if (! stringToDistanceI(value, l))
      return tk_->throwError("Invalid length \"" + value + "\"");

    length_ = l;

    updateSize();
  }
  else if (name == "-orient") {
    Qt::Orientation orient;

    if (! stringToOrient(value, orient))
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

    if (! CStrUtil::toBool(value, &b))
      return tk_->throwError("Invalid showvalue \"" + value + "\"");

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
    int to;

    if (! tk_->getOptionInt(name, value, to))
      return false;

    qscale_->setMaximum(to);
  }
  else if (name == "-variable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasVar(varName_))
      tk_->setIntegerVar(varName_, 0);

    updateVariable();

    varProc_ = new CTkScaleVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-width") {
    int w;

    if (! stringToDistanceI(value, w))
      return tk_->throwError("Invalid width \"" + value + "\"");

    width_ = w;

    updateSize();
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkScale::
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
CTkScale::
updateVariable()
{
  if (varName_ != "" && tk_->hasVar(varName_))
    qscale_->setValue(tk_->getIntegerVar(varName_));
}

void
CTkScale::
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
CTkScale::
valueSlot(int value)
{
  if (varName_ != "" && tk_->hasVar(varName_)) {
    varProc_->setEnabled(false);

    tk_->setIntegerVar(varName_, value);

    varProc_->setEnabled(true);
  }

  Args args;

  args.push_back(std::to_string(value));

  runCommand(args);
}

//----------

CTkScrollBar::
CTkScrollBar(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qscrollbar_ = new QScrollBar(parent_->getQWidget());
  else
    qscrollbar_ = new QScrollBar(nullptr);

  qscrollbar_->setRange(0, 1000);

  setQWidget(qscrollbar_);

  connectSlots(true);
}

void
CTkScrollBar::
connectSlots(bool b)
{
  if (b)
    connect(qscrollbar_, SIGNAL(valueChanged(int)), this, SLOT(scrollSlot(int)));
  else
    disconnect(qscrollbar_, SIGNAL(valueChanged(int)), this, SLOT(scrollSlot(int)));
}

bool
CTkScrollBar::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-orient") {
    Qt::Orientation orient;

    if (! stringToOrient(value, orient))
      return false;

    qscrollbar_->setOrientation(orient);
  }
  else if (name == "-command") {
    setCommand(value);
  }
  else if (name == "-highlightthickness") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkScrollBar::
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

    if (stringToReal(args[1], pos1) && stringToReal(args[2], pos2)) {
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
CTkScrollBar::
scrollSlot(int i)
{
  Args args;

  args.push_back(std::to_string(i/1000.0));

  runCommand(args);
}

//----------

class CTkSpinBoxVarProc : public CTclTraceProc {
 public:
  CTkSpinBoxVarProc(CTkApp *tk, CTkSpinBox *spin) :
   CTclTraceProc(tk), spin_(spin) {
  }

  void handleWrite(const char *) override {
    spin_->updateVariable();
  }

 private:
  CTkSpinBox *spin_ { nullptr };
};

CTkSpinBox::
CTkSpinBox(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qspin_ = new CQSpinList(parent_->getQWidget());
  else
    qspin_ = new CQSpinList(nullptr);

  setQWidget(qspin_);

  qwidget_->setFocusPolicy(Qt::StrongFocus);
}

bool
CTkSpinBox::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasVar(varName_))
      tk_->setStringVar(varName_, "");

    if (tk_->hasVar(varName_))
      setValue(tk_->getStringVar(varName_));

    varProc_ = new CTkSpinBoxVarProc(tk_, this);

    tk_->traceVar(varName_, varProc_);
  }
  else if (name == "-width") {
    tk_->TODO(name);
  }
  else if (name == "-borderwidth" || name == "-bd") {
    int w;

    if (! tk_->getOptionInt(name, value, w))
      return false;

    setBorderWidth(qspin_, w);
  }
  else if (name == "-relief") {
    setFrameRelief(qspin_, value);
  }
  else if (name == "-from") {
    int from;

    if (! tk_->getOptionInt(name, value, from))
      return false;

    qspin_->setMinimum(from);
  }
  else if (name == "-to") {
    int to;

    if (! tk_->getOptionInt(name, value, to))
      return false;

    qspin_->setMaximum(to);
  }
  else if (name == "-increment") {
    int step;

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
CTkSpinBox::
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
CTkSpinBox::
setValue(const std::string &)
{
}

void
CTkSpinBox::
updateVariable()
{
  if (varName_ != "" && tk_->hasVar(varName_))
    qspin_->setValue(tk_->getIntegerVar(varName_));
}

//----------

CTkText::
CTkText(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  if (parent)
    qtext_ = new QTextEdit(parent_->getQWidget());
  else
    qtext_ = new QTextEdit(nullptr);

  qtext_->setFocusPolicy(Qt::StrongFocus);

  setQWidget(qtext_);

  qtext_->verticalScrollBar  ()->setVisible(false);
  qtext_->horizontalScrollBar()->setVisible(false);

  connectSlots(true);
}

void
CTkText::
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
CTkText::
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
  else if (name == "-xscrollcommand") {
    xScrollCommand_ = value;
  }
  else if (name == "-yscrollcommand") {
    yScrollCommand_ = value;
  }
  else if (name == "-width") {
    tk_->TODO(name);
  }
  else if (name == "-height") {
    tk_->TODO(name);
  }
  else if (name == "-relief") {
    setFrameRelief(qtext_, value);
  }
  else if (name == "-borderwidth") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkText::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "xview") {
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

        if (! stringToReal(opt, y))
          return tk_->throwError("Invalid yview value '" + opt + "'");

        qtext_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
    }
  }
  else if (arg == "insert") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " insert index chars ?tagList chars tagList ...?");

    int index;

    if (args[1] == "end") {
      index = -1;
    }
    else {
      if (! stringToInteger(args[1], index))
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
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkText::
setText(const std::string &text)
{
  qtext_->setText(QString::fromStdString(text));
}

void
CTkText::
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
CTkText::
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

CTkTopLevel::
CTkTopLevel(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qframe_ = new QFrame(nullptr);

  setQWidget(qframe_);
}

bool
CTkTopLevel::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-borderwidth") {
    tk_->TODO(name);
  }
  else if (name == "-relief") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkTopLevel::
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
}

CTkAppRoot *
CTkAppWidget::
root() const
{
  return tk_->root();
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

  qwidget_->installEventFilter(new CTkWidgetEventFilter(this));

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
  WidgetMap::iterator p = children_.find(w->getName());

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
  WidgetMap::const_iterator p = children_.find(name);

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
getTkPackLayout()
{
  auto *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  auto *l1 = qobject_cast<CTkAppPackLayout *>(l);

  if (! l1)
    l1 = new CTkAppPackLayout(w);

  return l1;
}

CTkAppGridLayout *
CTkAppWidget::
getTkGridLayout()
{
  auto *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  auto *l1 = qobject_cast<CTkAppGridLayout *>(l);

  if (! l1)
    l1 = new CTkAppGridLayout(w);

  return l1;
}

CTkAppPlaceLayout *
CTkAppWidget::
getTkPlaceLayout()
{
  auto *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  auto *l1 = qobject_cast<CTkAppPlaceLayout *>(l);

  if (! l1)
    l1 = new CTkAppPlaceLayout(w);

  return l1;
}

bool
CTkAppWidget::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-width") {
    int w;

    if (! stringToDistanceI(value, w))
      return tk_->throwError("Invalid width \"" + value + "\"");

    setWidth(w);

    qwidget_->resize(getWidth(), getHeight());
  }
  else if (name == "-height") {
    int h;

    if (! stringToDistanceI(value, h))
      return tk_->throwError("Invalid height \"" + value + "\"");

    setHeight(h);

    qwidget_->resize(getWidth(), getHeight());
  }
  else if (name == "-background" || name == "-bg") {
    auto c = stringToQColor(value);

    CQUtil::setBackground(qwidget_, c);
  }
  else if (name == "-foreground" || name == "-fg") {
    auto c = stringToQColor(value);

    CQUtil::setForeground(qwidget_, c);
  }
  else if (name == "-font") {
    auto f = stringToQFont(value);

    qwidget_->setFont(f);
  }
  else if (name == "-cursor") {
    tk_->TODO(name);
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
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  const auto &arg = args[0];

  return tk_->throwError("bad option \"" + arg + "\" for " + std::string(getClassName()));
}

void
CTkAppWidget::
show()
{
  if (qwidget_)
    qwidget_->show();
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

bool
CTkAppWidget::
bindEvent(const CTkAppEventData &data)
{
  eventDatas_.push_back(data);

  return true;
}

bool
CTkAppWidget::
triggerEnterEvents(QEvent *e)
{
  CTkAppEventData matchEventData;

  matchEventData.type = CTkAppEventType::Enter;

  //---

  // widget specific events
  for (const auto &eventData : eventDatas_) {
    if (eventData == matchEventData)
      tk_->execEvent(this, e, eventData);
  }

  // class and global events
  return tk_->triggerEnterEvents(getClassName(), this, e);
}

bool
CTkAppWidget::
triggerLeaveEvents(QEvent *e)
{
  CTkAppEventData matchEventData;

  matchEventData.type = CTkAppEventType::Leave;

  //---

  // widget specific events
  for (const auto &eventData : eventDatas_) {
    if (eventData == matchEventData)
      tk_->execEvent(this, e, eventData);
  }

  // class and global events
  return tk_->triggerLeaveEvents(getClassName(), this, e);
}

bool
CTkAppWidget::
triggerMousePressEvents(QEvent *e, int button)
{
  auto *me = static_cast<QMouseEvent *>(e);

  CTkAppEventData matchEventData;

  tk_->encodeEvent(me, CTkAppEventMode::Press, button, matchEventData);

  //---

  for (const auto &eventData : eventDatas_) {
    if (eventData == matchEventData)
      tk_->execEvent(this, e, eventData);
  }

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

  //---

  for (const auto &eventData : eventDatas_) {
    if (eventData == matchEventData)
      tk_->execEvent(this, e, eventData);
  }

  return true;
}

bool
CTkAppWidget::
triggerMouseReleaseEvents(QEvent *e, int button)
{
  auto *me = static_cast<QMouseEvent *>(e);

  CTkAppEventData matchEventData;

  tk_->encodeEvent(me, CTkAppEventMode::Release, button, matchEventData);

  //---

  for (const auto &eventData : eventDatas_) {
    if (eventData == matchEventData)
      tk_->execEvent(this, e, eventData);
  }

  return true;
}

bool
CTkAppWidget::
triggerKeyPressEvents(QEvent *e)
{
  auto *ke = dynamic_cast<QKeyEvent *>(e);

  CTkAppEventData matchEventData;

  tk_->encodeEvent(ke, /*press*/true, matchEventData);

  //---

  for (const auto &eventData : eventDatas_) {
    if (eventData == matchEventData)
      tk_->execEvent(this, e, eventData);
  }

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

CTkSubMenu::
CTkSubMenu(CTkApp *tk) :
 QMenu(nullptr), tk_(tk)
{
  connectSlots(true);
}

void
CTkSubMenu::
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
CTkSubMenu::
showSlot()
{
  std::cerr << "Show " << menu_ << "\n";

  auto *w    = tk_->lookupWidgetByName(menu_);
  auto *menu = dynamic_cast<CTkMenu *>(w);
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
CTkSubMenu::
hideSlot()
{
  std::cerr << "Hide " << menu_ << "\n";

  auto *w    = tk_->lookupWidgetByName(menu_);
  auto *menu = dynamic_cast<CTkMenu *>(w);
  if (! menu) { std::cerr << "No menu '" << menu_ << "'\n"; return; }

  auto actions = this->actions();

  for (auto *action : actions) {
    std::cerr << "Remove Action " << action->text().toStdString() << "\n";

    //action->setParent(menu->qmenu());

    menu->qmenu()->addAction(action);
  }
}
