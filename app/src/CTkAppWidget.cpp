#include <CTkAppWidget.h>
#include <CTkAppCommands.h>
#include <CTkApp.h>
#include <CTkAppPackLayout.h>
#include <CTkAppPlaceLayout.h>
#include <CTkAppGridLayout.h>
#include <CTkAppImage.h>
#include <CTkAppUtil.h>

#include <CQSlider.h>
#include <CQLabelImage.h>
#include <CQUtil.h>

#include <CGlob.h>
#include <CSVGUtil.h>
#include <CMatrix2D.h>
#include <CStrParse.h>

#include <QApplication>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QSplitter>
#include <QTextBlock>
#include <QToolButton>

#include <deque>

namespace CTkAppUtil {

enum { END_INDEX=INT_MAX };

bool stringToIndex(const std::string &str, int &ind) {
  CStrParse parse(str);

  parse.skipSpace();

  if (parse.isString("end")) {
    ind = END_INDEX;
  }
  else {
    if (! parse.readInteger(&ind))
      return false;
  }

  return true;
}

bool stringToLineChar(const std::string &str, int &lineNum, int &charNum) {
  CStrParse parse(str);

  parse.skipSpace();

  if (! parse.readInteger(&lineNum))
    return false;

  if (parse.isChar('.')) {
    parse.skipChar();

    if (parse.isString("end")) {
      charNum = CTkAppText::TextInd::END;
    }
    else {
      if (! parse.readInteger(&charNum))
        return false;
    }
  }
  else {
    charNum = CTkAppText::TextInd::END;
  }

  return true;
}

bool stringToTextInd(const std::string &str, CTkAppText::TextInd &ind) {
  if (str == "end") {
    ind = CTkAppText::TextInd::end();
    return true;
  }

  int lineNum { -1 };
  int charNum { -1 };

  if (! stringToLineChar(str, lineNum, charNum))
    return false;

  ind = CTkAppText::TextInd(lineNum, charNum);

  return true;
}

bool stringToRelief(const std::string &str, CTkAppWidget::Relief &relief)
{
  if      (str == "raised")
    relief = CTkAppWidget::Relief::RAISED;
  else if (str == "sunken")
    relief = CTkAppWidget::Relief::SUNKEN;
  else if (str == "flat")
    relief = CTkAppWidget::Relief::FLAT;
  else if (str == "ridge")
    relief = CTkAppWidget::Relief::RIDGE;
  else if (str == "solid")
    relief = CTkAppWidget::Relief::SOLID;
  else if (str == "groove")
    relief = CTkAppWidget::Relief::GROOVE;
  else
    return false;

  return true;
}

// set relief
void setFrameRelief(QWidget *w, const CTkAppWidget::Relief &relief) {
  auto *frame = qobject_cast<QFrame *>(w);
  auto *edit  = qobject_cast<QLineEdit *>(w);
  auto *text  = qobject_cast<QTextEdit *>(w);

  if      (frame) {
    if      (relief == CTkAppWidget::Relief::RAISED) {
      frame->setFrameShadow(QFrame::Raised);
      frame->setFrameShape (QFrame::Panel);
    }
    else if (relief == CTkAppWidget::Relief::SUNKEN) {
      frame->setFrameShadow(QFrame::Sunken);
      frame->setFrameShape (QFrame::Panel);
    }
    else if (relief == CTkAppWidget::Relief::FLAT) {
      frame->setFrameShadow(QFrame::Plain);
      frame->setFrameShape (QFrame::NoFrame);
    }
    else if (relief == CTkAppWidget::Relief::RIDGE) {
      frame->setFrameShadow(QFrame::Raised);
      frame->setFrameShape (QFrame::Box);
    }
    else if (relief == CTkAppWidget::Relief::SOLID) {
      frame->setFrameShadow(QFrame::Plain);
      frame->setFrameShape (QFrame::Panel);
    }
    else if (relief == CTkAppWidget::Relief::GROOVE) {
      frame->setFrameShadow(QFrame::Sunken);
      frame->setFrameShape (QFrame::Box);
    }
  }
  else if (edit) {
    edit->setFrame(true);
  }
  else if (text) {
    if      (relief == CTkAppWidget::Relief::RAISED) {
      text->setFrameShadow(QFrame::Raised);
      text->setFrameShape (QFrame::Panel);
    }
    else if (relief == CTkAppWidget::Relief::SUNKEN) {
      text->setFrameShadow(QFrame::Sunken);
      text->setFrameShape (QFrame::Panel);
    }
    else if (relief == CTkAppWidget::Relief::FLAT) {
      text->setFrameShadow(QFrame::Plain);
      text->setFrameShape (QFrame::NoFrame);
    }
    else if (relief == CTkAppWidget::Relief::RIDGE) {
      text->setFrameShadow(QFrame::Raised);
      text->setFrameShape (QFrame::Box);
    }
    else if (relief == CTkAppWidget::Relief::SOLID) {
      text->setFrameShadow(QFrame::Plain);
      text->setFrameShape (QFrame::Panel);
    }
    else if (relief == CTkAppWidget::Relief::GROOVE) {
      text->setFrameShadow(QFrame::Sunken);
      text->setFrameShape (QFrame::Box);
    }
  }
}

bool setWidgetState(CTkAppWidget *w, const std::string &value)
{
  auto *qw = w->qwidget();

  auto *qbutton = qobject_cast<QAbstractButton *>(qw);

  if (qbutton) {
    if      (value == "normal") {
      qbutton->setEnabled(true);
      qbutton->setDown(false);
    }
    else if (value == "active")
      qbutton->setDown(true);
    else if (value == "disabled")
      qbutton->setEnabled(false);
    else {
      std::cerr << "Invalid state '" << value << "'\n";
      return false;
    }
  }
  else {
    if      (value == "normal")
      qw->setEnabled(true);
    else if (value == "active")
      std::cerr << "Unsupported state '" << value << "'\n";
    else if (value == "disabled")
      qw->setEnabled(false);
    else {
      std::cerr << "Invalid state '" << value << "'\n";
      return false;
    }
  }

  return true;
}

bool stringToAlign(const std::string &value, Qt::Alignment &align)
{
  align = Qt::AlignCenter;

  if      (value == "n"     ) align = Qt::AlignTop;
  else if (value == "ne"    ) align = Qt::AlignTop    | Qt::AlignRight;
  else if (value == "e"     ) align =                   Qt::AlignRight;
  else if (value == "se"    ) align = Qt::AlignBottom | Qt::AlignRight;
  else if (value == "s"     ) align = Qt::AlignBottom;
  else if (value == "sw"    ) align = Qt::AlignBottom | Qt::AlignLeft;
  else if (value == "w"     ) align =                   Qt::AlignLeft;
  else if (value == "nw"    ) align = Qt::AlignTop    | Qt::AlignLeft;
  else if (value == "center") align = Qt::AlignCenter;
  else if (value == "c"     ) align = Qt::AlignCenter;
  else if (value == "middle") align = Qt::AlignCenter;
  else { std::cerr << "Invalid align string '" << value << "'\n"; return false; }

  return true;
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

bool stringToCompound(const std::string &value, CTkAppCompoundType &type)
{
  type = CTkAppCompoundType::NONE;

  // none, bottom, top, left, right, or center.
  if      (value == "none"  ) type = CTkAppCompoundType::NONE;
  else if (value == "bottom") type = CTkAppCompoundType::BOTTOM;
  else if (value == "top"   ) type = CTkAppCompoundType::TOP;
  else if (value == "left"  ) type = CTkAppCompoundType::LEFT;
  else if (value == "right" ) type = CTkAppCompoundType::RIGHT;
  else if (value == "center") type = CTkAppCompoundType::CENTER;
  else return false;

  return true;
}

bool stringToPath(const QString &str, QPainterPath &path) {
  class PathVisitor : public CSVGUtil::PathVisitor {
   public:
    PathVisitor() { }

    const QPainterPath &path() const { return path_; }

    void moveTo(double x, double y) override {
      path_.moveTo(x, y);
    }

    void lineTo(double x, double y) override {
      path_.lineTo(x, y);
    }

    void arcTo(double rx, double ry, double xa, int fa, int fs, double x2, double y2) override {
      bool unit_circle = false;

      //double cx, cy, rx1, ry1, theta, delta;

      //CSVGUtil::convertArcCoords(lastX(), lastY(), x2, y2, xa, rx, ry, fa, fs, unit_circle,
      //                           &cx, &cy, &rx1, &ry1, &theta, &delta);

      //path_.arcTo(QRectF(cx - rx1, cy - ry1, 2*rx1, 2*ry1), -theta, -delta);

      //double a1 = CMathUtil::Deg2Rad(theta);
      //double a2 = CMathUtil::Deg2Rad(theta + delta);

      CSVGUtil::BezierList beziers;

      CSVGUtil::arcToBeziers(lastX(), lastY(), x2, y2, xa, rx, ry, fa, fs, unit_circle, beziers);

      auto qpoint = [](const CPoint2D &p) { return QPointF(p.x, p.y); };

      if (! beziers.empty())
        path_.lineTo(qpoint(beziers[0].getFirstPoint()));

      for (const auto &bezier : beziers)
        path_.cubicTo(qpoint(bezier.getControlPoint1()),
                      qpoint(bezier.getControlPoint2()),
                      qpoint(bezier.getLastPoint    ()));
    }

    void bezier2To(double x1, double y1, double x2, double y2) override {
      path_.quadTo(QPointF(x1, y1), QPointF(x2, y2));
    }

    void bezier3To(double x1, double y1, double x2, double y2, double x3, double y3) override {
      path_.cubicTo(QPointF(x1, y1), QPointF(x2, y2), QPointF(x3, y3));
    }

    void closePath(bool /*relative*/) override {
      path_.closeSubpath();
    }

   private:
    QPainterPath path_;
  };

  PathVisitor visitor;

  if (! CSVGUtil::visitPath(str.toStdString(), visitor)) {
    //std::cerr << "Invalid path: " << str.toStdString() << "\n";
    return false;
  }

  path = visitor.path();

  return true;
}

bool stringToMatrix(CTkApp *tk, const std::string &str, QTransform &t) {
  CTkAppCanvasWidget::Points points;

  std::vector<std::string> strs;
  if (! tk->splitList(str, strs) || strs.size() != 3)
    return false;

  for (uint j = 0; j < 3; ++j) {
    std::vector<std::string> strs1;
    if (! tk->splitList(strs[j], strs1) || strs1.size() != 2)
      return false;

    double x, y;
    if (! CTkAppUtil::stringToReal(strs1[0], x) ||
        ! CTkAppUtil::stringToReal(strs1[1], y))
      return false;

    points.push_back(CTkAppCanvasWidget::Point(x, y));
  }

  CMatrix2D m(points[0].x, points[1].x,
              points[0].y, points[1].y,
              points[2].x, points[2].y);

  t = tk->toQTransform(m);

  return true;
}

}

//---

CTkAppRoot::
CTkAppRoot(CTkApp *tk) :
 CTkAppWidget(tk, nullptr, "")
{
  qroot_ = new CTkAppRootWidget(this);

  qroot_->setWindowTitle(QString::fromStdString("."));

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
  CQUtil::connectDisconnect(b, qbutton_,
    SIGNAL(clicked()), this, SLOT(clickSlot()));
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

    qbutton_->setUserHeight(h);
  }
  else if (name == "-overrelief") {
    if (value == "raised")
      setOverRaised(true);
    else
      setOverRaised(false);

    appearanceChanged();
  }
  else if (name == "-state") {
    if (! CTkAppUtil::setWidgetState(this, value))
      return false;
  }
  else if (name == "-underline") {
    if (! tk_->getOptionInt(name, value, underlinePos))
      return false;
  }
  else if (name == "-width") {
    // width of image (pixels) or text (chars)
    double w;
    if (! CTkAppUtil::stringToDistance(value, w))
      return tk_->throwError("Invalid width \"" + value + "\"");

    qbutton_->setUserWidth(w);
  }
  else if (name == "-wraplength") {
    double length;
    if (! CTkAppUtil::stringToDistance(value, length))
      return tk_->throwError("Invalid width \"" + value + "\"");
    qbutton_->setWrapLength(length);
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
setText(const std::string &text)
{
  CTkAppWidget::setText(text);

  qbutton_->setText(QString::fromStdString(text));
}

void
CTkAppButton::
setImage(const CTkAppImageRef &image)
{
  CTkAppWidget::setImage(image);

  qbutton_->setImage(image->getQImage());
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

void
CTkAppButton::
appearanceChanged()
{
  bool autoRaise = false;

  if (relief_ == Relief::FLAT && overRaised_)
    autoRaise = true;

  qbutton_->setAutoRaise(autoRaise);
}

//---

CTkAppButtonWidget::
CTkAppButtonWidget(CTkAppButton *button) :
 CQButtonImage(button->getParent() ? button->getParent()->getQWidget() : nullptr), button_(button)
{
  setObjectName("qbutton");
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

      CQButtonImage::setLabel(text);
    }
    else
      CQButtonImage::setLabel(text_);
  }
  else
    CQButtonImage::setLabel(text_);
}

//----------

CTkAppCanvas::
CTkAppCanvas(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qcanvas_ = new CTkAppCanvasWidget(this);

  qcanvas_->setFocusPolicy(Qt::StrongFocus);

  setQWidget(qcanvas_);
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
      return tk_->throwError("Invalid height \"" + value + "\"");
    setHeight(height);
  }
  else if (name == "-scrollregion") {
    tk_->TODO(name);
  }
  else if (name == "-state") {
    if (! CTkAppUtil::setWidgetState(this, value))
      return false;
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
      Qt::Alignment align;
      if (CTkAppUtil::stringToAlign(value, align))
        shape->setAnchor(align);
    }
    else if (name == "-dash") {
      tk_->TODO(name, args);
    }
    else if (name == "-activedash") {
      tk_->TODO(name, args);
    }
    else if (name == "-disabledash") {
      tk_->TODO(name, args);
    }
    else if (name == "-dashoffset") {
      tk_->TODO(name, args);
    }
    else if (name == "-fill") {
      auto b = shape->brush();

      QGradient *g;
      if (getGradient(value, g))
        b = QBrush(*g);
      else {
        QColor c;
        if (CTkAppUtil::stringToQColor(value, c)) {
          b.setStyle(Qt::SolidPattern);
          b.setColor(c);
        }
      }
      shape->setBrush(b);
    }
    else if (name == "-fillrule") {
      tk_->TODO(name, args);
    }
    else if (name == "-fillopacity") {
      double a;
      if (! CTkAppUtil::stringToReal(value, a))
        return false;

      auto b = shape->brush();
      auto c = b.color();
      c.setAlphaF(a);
      b.setStyle(Qt::SolidPattern);
      b.setColor(c);
    }
    else if (name == "-activefill") {
      tk_->TODO(name, args);
    }
    else if (name == "-disabledfill") {
      tk_->TODO(name, args);
    }
    else if (name == "-filloverstroke") {
      tk_->TODO(name, args);
    }
    else if (name == "-outline" ||  name == "-stroke") {
      QColor c;
      if (CTkAppUtil::stringToQColor(value, c)) {
        auto p = shape->pen();
        p.setColor(c);
        shape->setPen(p);
      }
    }
    else if (name == "-activeoutline") {
      tk_->TODO(name, args);
    }
    else if (name == "-disabledoutline") {
      tk_->TODO(name, args);
    }
    else if (name == "-offset") {
      tk_->TODO(name, args);
    }
    else if (name == "-outlinestipple") {
      tk_->TODO(name, args);
    }
    else if (name == "-activeoutlinestipple") {
      tk_->TODO(name, args);
    }
    else if (name == "-disabledoutlinestipple") {
      tk_->TODO(name, args);
    }
    else if (name == "-outlineoffset") {
      tk_->TODO(name, args);
    }
    else if (name == "-stipple") {
      tk_->TODO(name, args);
    }
    else if (name == "-activestipple") {
      tk_->TODO(name, args);
    }
    else if (name == "-disabledstipple") {
      tk_->TODO(name, args);
    }
    else if (name == "-state") {
      tk_->TODO(name, args);
    }
    else if (name == "-fontfamily") {
      shape->setFontFamily(value);
    }
    else if (name == "-fontsize") {
      long s;
      if (! CTkAppUtil::stringToInt(value, s))
        return false;

      shape->setFontSize(s);
    }
    else if (name == "-tags" || name == "-tag") {
      std::vector<std::string> strs;
      if (! tk_->splitList(value, strs))
        return tk_->throwError("Invalid tags \"" + value + "\"");

      shape->setTags(strs);
    }
    else if (name == "-width" || name == "-strokewidth") {
      double width;
      if (! CTkAppUtil::stringToDistance(value, width))
        return tk_->throwError("Invalid width \"" + value + "\"");

      auto p = shape->pen();
      p.setWidthF(width);
      shape->setPen(p);
    }
    else if (name == "-strokelinecap") {
      Qt::PenCapStyle capStyle = Qt::FlatCap;

      if      (value == "round")
        capStyle = Qt::RoundCap;
      else if (value == "square")
        capStyle = Qt::SquareCap;
      else if (value == "flat")
        capStyle = Qt::FlatCap;
      else
        return false;

      auto p = shape->pen();
      p.setCapStyle(capStyle);
      shape->setPen(p);
    }
    else if (name == "-strokeopacity") {
      tk_->TODO(name, args);
    }
    else if (name == "-activewidth") {
      tk_->TODO(name, args);
    }
    else if (name == "-disabledwidth") {
      tk_->TODO(name, args);
    }
#ifdef CTK_APP_TKPATH
    else if (name == "-matrix") {
      CTkApp::MatrixData mdata;

      if (tk_->getNamedMatrix(value, mdata)) {
        shape->setTransform(mdata.transform);
      }
      else {
        QTransform t;
        if (! CTkAppUtil::stringToMatrix(tk_, value, t))
          return tk_->throwError("Invalid matrix \"" + value + "\"");
      }
    }
#endif
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
    else if (name == "-outline" ||  name == "-stroke") {
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
    else if (name == "-parent") {
      tk_->TODO(name, args);
    }
    else if (name == "-style") {
      tk_->TODO(name, args);
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
      tk_->TODO(cmd, args);
    }
    else if (cmd == "all") {
      tk_->TODO(cmd, args);
    }
    else if (cmd == "below") {
      tk_->TODO(cmd, args);
    }
    else if (cmd == "closest") {
      tk_->TODO(cmd, args);
    }
    else if (cmd == "enclosed") {
      tk_->TODO(cmd, args);
    }
    else if (cmd == "overlapping") {
      tk_->TODO(cmd, args);
    }
    else if (cmd == "withtag") {
      tk_->TODO(cmd, args);
    }
    else
      return tk_->throwError("bad search command \"" + cmd + "\": must be "
               "above, all, below, closest, enclosed, overlapping, or withtag");
  }
  else if (arg == "bbox") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " bbox tagOrId ?tagOrId ...?");

    tk_->TODO(arg, args);
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

    tk_->TODO(arg, args);
  }
  else if (arg == "canvasy") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " canvasy screenx ?gridspacing?");

    tk_->TODO(arg, args);
  }
  else if (arg == "coords") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " coords tagOrId ?x y x y ...?");

    auto id = args[1];

    CTkAppCanvasWidget::Points points;

    if (numArgs > 2) {
      for (uint i = 2; i < numArgs; i += 2) {
        auto xarg = args[i];

        if (xarg[0] == '-' && ! isdigit(xarg[1]))
          break;

        if (i + 1 >= numArgs)
          return false;

        auto yarg = args[i + 1];

        double x, y;
        if (! CTkAppUtil::stringToDistance(xarg, x) || ! CTkAppUtil::stringToDistance(yarg, y))
          return tk_->throwError("Invalid coord \"" + xarg + " " + yarg + "\"");

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
      "arc", "bitmap", "image", "line", "oval",
      "polygon", "rectangle", "text", "window"
#ifdef CTK_APP_TKPATH
    , "circle", "ellipse", "group", "path", "pimage", "pline",
      "polyline", "ppolygon", "prect", "ptext"
#endif
    });

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
              tk_->TODO(value, args);
          }
          else
            return false;
        }
        else
          return false;
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
            tk_->TODO(name, args);
          }
          else if (name == "-activebackground") {
            tk_->TODO(name, args);
          }
          else if (name == "-activeforeground") {
            tk_->TODO(name, args);
          }
          else if (name == "-disabledbackground") {
            tk_->TODO(name, args);
          }
          else if (name == "-disabledforeground") {
            tk_->TODO(name, args);
          }
          else if (name == "-bitmap") {
            auto image = tk_->getBitmap(value);

            bitmap->setImage(image);
          }
          else if (name == "-activebitmap") {
            tk_->TODO(name, args);
          }
          else if (name == "-disabledbitmap") {
            tk_->TODO(name, args);
          }
          else if (name == "-foreground") {
            tk_->TODO(name, args);
          }
          else
            return false;
        }
        else
          return false;
      }

      tk_->setIntegerResult(bitmap->id());
    }
    else if (type == "image"
#ifdef CTK_APP_TKPATH
          || type == "pimage"
#endif
            ) {
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

            if (! appImage)
              return tk_->throwError("Failed to get image '" + value + "'");

            image->setImage(appImage);
          }
          else if (name == "-activeimage") {
            tk_->TODO(name, args);
          }
          else if (name == "-disabledimage") {
            tk_->TODO(name, args);
          }
          else
            return false;
        }
        else
          return false;
      }

      tk_->setIntegerResult(image->id());
    }
    else if (type == "line"
#ifdef CTK_APP_TKPATH
          || type == "pline"
#endif
            ) {
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
              tk_->TODO(name, args);

            line->setArrowType(arrowType);
          }
          else if (name == "-arrowshape") {
            tk_->TODO(name, args);
          }
          else if (name == "-capstyle") {
            tk_->TODO(name, args);
          }
          else if (name == "-joinstyle") {
            tk_->TODO(name, args);
          }
          else if (name == "-smooth") {
            tk_->TODO(name, args);
          }
          else if (name == "-splinesteps") {
            tk_->TODO(name, args);
          }
          else
            return false;
        }
        else
          return false;
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

          return false;
        }
        else
          return false;
      }

      tk_->setIntegerResult(oval->id());
    }
#ifdef CTK_APP_TKPATH
    else if (type == "circle") {
      if (numArgs < 4)
        return tk_->wrongNumArgs(getName() + " create circle xc yc ?arg ...?");

      double xc, yc;
      if (! CTkAppUtil::stringToReal(args[2], xc) ||
          ! CTkAppUtil::stringToReal(args[3], yc))
        return false;

      auto *circle = qcanvas_->addCircle(xc, yc);

      uint i = 4;

      for ( ; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (setShapeOpt(circle, name, value))
            continue;

          if (name == "-r") {
            double r;
            if (! CTkAppUtil::stringToReal(value, r))
              return false;

            circle->setRadius(r);
          }
          else {
            return false;
          }
        }
        else
          return false;
      }

      tk_->setIntegerResult(circle->id());
    }
    else if (type == "ellipse") {
      if (numArgs < 4)
        return tk_->wrongNumArgs(getName() + " create ellipse xc yc ?arg ...?");

      double xc, yc;
      if (! CTkAppUtil::stringToReal(args[2], xc) ||
          ! CTkAppUtil::stringToReal(args[3], yc))
        return false;

      auto *ellipse = qcanvas_->addEllipse(xc, yc);

      uint i = 4;

      for ( ; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (setShapeOpt(ellipse, name, value))
            continue;

          if      (name == "-rx") {
            double rx;
            if (! CTkAppUtil::stringToReal(value, rx))
              return false;

            ellipse->setRadiusX(rx);
          }
          else if (name == "-ry") {
            double ry;
            if (! CTkAppUtil::stringToReal(value, ry))
              return false;

            ellipse->setRadiusY(ry);
          }
          else {
            tk_->TODO(name, args);
          }
        }
        else
          tk_->TODO(args);
      }

      tk_->setIntegerResult(ellipse->id());
    }
    else if (type == "group") {
      tk_->TODO(args);
    }
    else if (type == "path") {
      if (numArgs < 3)
        return tk_->wrongNumArgs(getName() + " create path path ?arg ...?");

      auto pathStr = args[2];

      QPainterPath qpath;
      if (! CTkAppUtil::stringToPath(QString::fromStdString(pathStr), qpath))
        return tk_->throwError("Invalid path \"" + pathStr + "\"");

      auto *path = qcanvas_->addPath(pathStr, qpath);

      uint i = 3;

      for ( ; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
          const auto &value = args[i + 1];

          if (setShapeOpt(path, name, value))
            continue;

          if (name == "-matrix") {
            QTransform t;
            if (! CTkAppUtil::stringToMatrix(tk_, value, t))
              return tk_->throwError("Invalid matrix \"" + value + "\"");

            path->setTransform(t);
          }
          else {
            tk_->TODO(name, args);
          }
        }
        else
          tk_->TODO(args);
      }

      tk_->setIntegerResult(path->id());
    }
#endif
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

          return false;
        }
        else
          return false;
      }

      tk_->setIntegerResult(polygon->id());
    }
    else if (type == "rectangle"
#ifdef CTK_APP_TKPATH
          || type == "prect"
#endif
            ) {
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
            tk_->TODO(name, args);
          }
          else if (name == "-smooth") {
            tk_->TODO(name, args);
          }
          else if (name == "-splinesteps") {
            tk_->TODO(name, args);
          }
          else if (name == "-rx") {
            tk_->TODO(name, args);
          }
          else
            tk_->TODO(name, args);
        }
        else
          tk_->TODO(args);
      }

      tk_->setIntegerResult(rectangle->id());
    }
    else if (type == "text"
#ifdef CTK_APP_TKPATH
          || type == "ptext"
#endif
            ) {
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
            tk_->TODO(name, args);
          }
          else if (name == "-font") {
            auto f = tk_->getQFont(value);

            text->setFont(f);
          }
          else if (name == "-justify") {
            auto align = CTkAppUtil::stringToJustify(value);

            text->setJustify(align);
          }
          else if (name == "-text") {
            text->setText(value);
          }
#ifdef CTK_APP_TKPATH
          else if (name == "-textanchor") {
            Qt::Alignment align;
            if (CTkAppUtil::stringToAlign(value, align))
              text->setTextAnchor(align);
          }
#endif
          else if (name == "-underline") {
            if (! tk_->getOptionInt(name, value, underlinePos))
              return false;
          }
          else if (name == "-width") {
            tk_->TODO(name, args);
          }
          else
            tk_->TODO(name, args);
        }
        else
          return false;
      }

      tk_->setIntegerResult(text->id());
    }
    else if (type == "window") {
      tk_->TODO(type, args);

      for (uint i = 1; i < numArgs; i += 2) {
        if (args[i][0] == '-') {
          const auto &name  = args[i    ];
        //const auto &value = args[i + 1];

          if      (name == "-height") {
            tk_->TODO(name, args);
          }
          else if (name == "-width") {
            tk_->TODO(name, args);
          }
          else if (name == "-window") {
            tk_->TODO(name, args);
          }
          else
            return false;
        }
        else
          return false;
      }
    }
    else
      return false;

    qcanvas_->update();
  }
  else if (arg == "dchars") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " dchars tagOrId first ?last?");

    tk_->TODO(arg, args);
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

    tk_->TODO(arg, args);
  }
  else if (arg == "find") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " find searchCommand ?arg ...?");

    const auto &cmd = args[1];

    if      (cmd == "above") {
      tk_->TODO(cmd, args);
    }
    else if (cmd == "all") {
      tk_->TODO(cmd, args);
    }
    else if (cmd == "below") {
      tk_->TODO(cmd, args);
    }
    else if (cmd == "closest") {
      tk_->TODO(cmd, args);
    }
    else if (cmd == "enclosed") {
      tk_->TODO(cmd, args);
    }
    else if (cmd == "overlapping") {
      tk_->TODO(cmd, args);
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
        return false;
    }
    else
      return tk_->throwError("bad search command \"" + cmd + "\": must be "
                             "above, all, below, closest, enclosed, overlapping, or withtag");
  }
  else if (arg == "focus") {
    tk_->TODO(arg, args);
  }
  else if (arg == "gettags") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " gettags tagOrId");

    tk_->TODO(arg, args);
  }
#ifdef CTK_APP_TKPATH
  else if (arg == "gradient") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getName() + " gradient create type args");

    auto opt = args[1];

    if      (opt == "cget") {
      tk_->TODO(opt, args);
    }
    else if (opt == "configure" || opt == "config") {
      tk_->TODO(opt, args);
    }
    else if (opt == "create") {
      auto type = args[2];

      if      (type == "linear") {
        double x1 { 0.0 }, y1 { 0.0 }, x2 { 1.0 }, y2 { 1.0 };

        std::map<double, QColor> stops;

        uint i = 3;

        for ( ; i < numArgs; ++i) {
          auto name = args[i];

          if      (name == "-stops") {
            ++i;

            auto value = (i < numArgs ? args[i] : "");

            std::vector<std::string> strs;
            if (! tk_->splitList(value, strs))
              return tk_->throwError("Invalid -stops \"" + value + "\"");

            for (auto &str : strs) {
              std::vector<std::string> strs1;
              if (! tk_->splitList(str, strs1))
                return tk_->throwError("Invalid -stops \"" + value + "\"");

              if (strs1.size() < 2)
                return tk_->throwError("Invalid -stops \"" + value + "\"");

              double r;
              if (! CTkAppUtil::stringToReal(strs1[0], r))
                return tk_->throwError("Invalid -stops \"" + value + "\"");

              QColor c;

              if (CTkAppUtil::stringToQColor(strs1[1], c))
                stops[r] = c;
            }
          }
          else if (name == "-lineartransition") {
            ++i;

            auto value = (i < numArgs ? args[i] : "");

            std::vector<std::string> strs;
            if (! tk_->splitList(value, strs))
              return tk_->throwError("Invalid -lineartransition \"" + value + "\"");

            if (strs.size() != 4)
              return tk_->throwError("Invalid -lineartransition \"" + value + "\"");

            if (! CTkAppUtil::stringToReal(strs[0], x1) ||
                ! CTkAppUtil::stringToReal(strs[1], y1) ||
                ! CTkAppUtil::stringToReal(strs[2], x2) ||
                ! CTkAppUtil::stringToReal(strs[3], y2))
              return tk_->throwError("Invalid -lineartransition \"" + value + "\"");
          }
          else if (name == "-units") {
            ++i;

            tk_->TODO(name, args);
          }
          else
            return false;
        }

        auto *lg = new QLinearGradient(x1, y1, x2, y2);

        lg->setCoordinateMode(QGradient::ObjectMode);

        for (const auto &ps : stops)
          lg->setColorAt(ps.first, ps.second);

        auto name = newGradientName();

        addGradient(name, lg);

        tk_->setStringResult(name);
      }
      else if (type == "radial") {
        std::map<double, QColor> stops;

        uint i = 3;

        for ( ; i < numArgs; ++i) {
          auto name = args[i];

          if      (name == "-stops") {
            ++i;

            auto value = (i < numArgs ? args[i] : "");

            std::vector<std::string> strs;
            if (! tk_->splitList(value, strs))
              return tk_->throwError("Invalid -stops \"" + value + "\"");

            for (auto &str : strs) {
              std::vector<std::string> strs1;
              if (! tk_->splitList(str, strs1))
                return tk_->throwError("Invalid -stops \"" + value + "\"");

              if (strs1.size() < 2)
                return tk_->throwError("Invalid -stops \"" + value + "\"");

              double r;
              if (! CTkAppUtil::stringToReal(strs1[0], r))
                return tk_->throwError("Invalid -stops \"" + value + "\"");

              QColor c;
              if (CTkAppUtil::stringToQColor(strs1[1], c))
                stops[r] = c;
            }
          }
          else if (name == "-units") {
            ++i;

            tk_->TODO(name, args);
          }
          else if (name == "-radialtransition") {
            ++i;

            tk_->TODO(name, args);
          }
          else
            return false;
        }

        auto *rg = new QRadialGradient;

        rg->setCoordinateMode(QGradient::ObjectMode);

        for (const auto &ps : stops)
          rg->setColorAt(ps.first, ps.second);

        auto name = newGradientName();

        addGradient(name, rg);

        tk_->setStringResult(name);
      }
      else
        tk_->TODO(type, args);
    }
    else if (opt == "delete") {
      tk_->TODO(opt, args);
    }
    else if (opt == "inuse") {
      tk_->TODO(opt, args);
    }
    else if (opt == "names") {
      tk_->TODO(opt, args);
    }
    else if (opt == "type") {
      tk_->TODO(opt, args);
    }
    else
      tk_->TODO(opt, args);
  }
  else if (arg == "style") {
    tk_->TODO(args);
  }
#endif
  else if (arg == "icursor") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " icursor tagOrId index");

    tk_->TODO(arg, args);
  }
  else if (arg == "imove") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " imove tagOrId index x y");

    tk_->TODO(arg, args);
  }
  else if (arg == "index") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " index tagOrId string");

    tk_->TODO(arg, args);
  }
  else if (arg == "insert") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " insert tagOrId beforeThis string");

    tk_->TODO(arg, args);
  }
  else if (arg == "itemcget") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " itemcget tagOrId option");

    const auto &id = args[1];

    CTkAppCanvasWidget::Shapes shapes;

    if       (id == "current")
      shapes.push_back(insideShape_);
    else if (id == "root")
      qcanvas_->getShapes(shapes);
    else {
      if (! qcanvas_->getShapes(id, shapes))
        return tk_->throwError("No matching shape \"" + id + "\" for itemcget");
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
          tk_->TODO(name, args);
      }
      else
        tk_->TODO(name, args);
    }

    tk_->setStringArrayResult(values);
  }
  else if (arg == "itemconfigure" || arg == "itemconfig") {
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
          tk_->TODO(name, args);
      }
      else
        tk_->TODO(name, args);
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
      return tk_->throwError("No shape \"" + id + "\"");
  }
  else if (arg == "moveto") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " moveto tagOrId x y");

    tk_->TODO(arg, args);
  }
  else if (arg == "postscript") {
    tk_->TODO(arg, args);
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

    tk_->TODO(arg, args);
  }
  else if (arg == "scale") {
    if (numArgs < 6)
      return tk_->wrongNumArgs(getName() + " scale tagOrId xOrigin yOrigin xScale yScale");

    const auto &name = args[1];

    double xc, yc, sx, sy;
    if (! CTkAppUtil::stringToReal(args[2], xc) ||
        ! CTkAppUtil::stringToReal(args[3], yc) ||
        ! CTkAppUtil::stringToReal(args[4], sx) ||
        ! CTkAppUtil::stringToReal(args[5], sy))
      return false;

    if (! qcanvas_->scaleShape(name, xc, yc, sx, sy))
      return false;
  }
  else if (arg == "scan") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " scan mark|dragto x y ?dragGain?");

    tk_->TODO(arg, args);
  }
  else if (arg == "select") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " select option ?tagOrId? ?arg?");

    tk_->TODO(arg, args);
  }
  else if (arg == "type") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " type tag");

    tk_->TODO(arg, args);
  }
  else if (arg == "xview") {
    tk_->TODO(arg, args);
    tk_->setStringResult("0.0 1.0");
  }
  else if (arg == "yview") {
    tk_->TODO(arg, args);
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

        CTkAppEventData matchEventData;
        matchEventData.type = CTkAppEventType::Enter;

        processShapeEvents(insideShape_, e, matchEventData);
      }

      // Leave old inside shape
      if (oldInsideShape) {
        //std::cerr << "Leave " << oldInsideShape->id() << "\n";

        CTkAppEventData matchEventData;
        matchEventData.type = CTkAppEventType::Leave;

        processShapeEvents(oldInsideShape, e, matchEventData);

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
  auto idStr = std::to_string(shape->id());

  auto pi = idEventDatas_.find(idStr);

  if (pi != idEventDatas_.end()) {
    for (const auto &eventData : (*pi).second) {
      if (eventData == matchEventData) {
        tk_->execEvent(this, e, matchEventData, eventData.command);
        return; // priority ?
      }
    }
  }

  for (auto &tag : shape->tags()) {
    auto pi = idEventDatas_.find(tag);

    if (pi != idEventDatas_.end()) {
      for (const auto &eventData : (*pi).second) {
        if (eventData == matchEventData) {
          tk_->execEvent(this, e, matchEventData, eventData.command);
          return; // priority ?
        }
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
  auto calcStrokePath = [&](const QPainterPath &path) {
    QPainterPathStroker stroker;

    auto pen = p->pen();

    stroker.setCapStyle   (pen.capStyle());
    stroker.setDashOffset (pen.dashOffset());
    stroker.setDashPattern(pen.dashPattern());
    stroker.setJoinStyle  (pen.joinStyle());
    stroker.setMiterLimit (pen.miterLimit());
    stroker.setWidth      (pen.widthF());

    return stroker.createStroke(path);
  };

  p->setPen  (s->pen());
  p->setBrush(s->brush());

  p->save();
  p->setWorldTransform(s->transform());

  switch (s->type()) {
    case CTkAppCanvasShapeType::RECTANGLE: {
      auto *r = static_cast<Rectangle *>(s);
      p->drawRect(r->rect());
      break;
    }
    case CTkAppCanvasShapeType::OVAL: {
      auto *o = static_cast<Oval *>(s);
      p->drawEllipse(o->rect());
      break;
    }
    case CTkAppCanvasShapeType::LINE: {
      auto *l = static_cast<Line *>(s);

      auto qpaintPath = calcStrokePath(l->qpath());
      l->setQStrokePath(qpaintPath);

      p->drawPath(l->qpath());

      break;
    }
#ifdef CTK_APP_TKPATH
    case CTkAppCanvasShapeType::PATH: {
      auto *path = static_cast<Path *>(s);

      auto qpaintPath = calcStrokePath(path->qpath());
      path->setQStrokePath(qpaintPath);

      p->drawPath(path->qpath());

      break;
    }
#endif
    case CTkAppCanvasShapeType::POLYGON: {
      auto *poly = static_cast<Polygon *>(s);
      p->drawPath(poly->qpath());
      break;
    }
    case CTkAppCanvasShapeType::ARC: {
      auto *a = static_cast<Arc *>(s);
      p->drawPath(a->qpath());
      break;
    }
#ifdef CTK_APP_TKPATH
    case CTkAppCanvasShapeType::CIRCLE: {
      auto *c = static_cast<Circle *>(s);
      p->drawPath(c->qpath());
      break;
    }
    case CTkAppCanvasShapeType::ELLIPSE: {
      auto *e = static_cast<Ellipse *>(s);
      p->drawPath(e->qpath());
      break;
    }
#endif
    case CTkAppCanvasShapeType::TEXT: {
      auto *t = static_cast<Text *>(s);

      p->setFont(t->font());

      auto text = QString::fromStdString(t->text());

      QFontMetricsF fm(p->font());
      auto tw = fm.horizontalAdvance(text);

      auto tx = t->pos().x;
      auto ty = t->pos().y;

      auto j = t->justify();

      double dtx = 0.0, dty = 0.0;
      if      (j & Qt::AlignLeft   ) { dtx = 0.0; }
      else if (j & Qt::AlignHCenter) { dtx = -tw/2.0; }
      else if (j & Qt::AlignRight  ) { dtx = -tw; }
      if      (j & Qt::AlignTop    ) { dty = fm.ascent(); }
      else if (j & Qt::AlignVCenter) { dty = (fm.ascent() - fm.descent())/2.0; }
      else if (j & Qt::AlignBottom ) { dty = -fm.descent(); }

      p->drawText(tx + dtx, ty + dty, QString::fromStdString(t->text()));

      break;
    }
    case CTkAppCanvasShapeType::IMAGE: {
      auto *i = static_cast<Image *>(s);

      auto qimage = i->getImage()->getQImage();

      auto iw = qimage.width();
      auto ih = qimage.height();

      auto a = i->anchor();

      double dtx = 0.0, dty = 0.0;
      if      (a & Qt::AlignLeft   ) { dtx = 0.0; }
      else if (a & Qt::AlignHCenter) { dtx = -iw/2.0; }
      else if (a & Qt::AlignRight  ) { dtx = -iw; }
      if      (a & Qt::AlignTop    ) { dty = 0.0; }
      else if (a & Qt::AlignVCenter) { dty = -ih/2.0; }
      else if (a & Qt::AlignBottom ) { dty = -ih; }

      auto rect = QRectF(i->pos().x + dtx, i->pos().y + dty, iw, ih);

      i->setDrawRect(rect);

      p->drawImage(rect, qimage);

      break;
    }
    case CTkAppCanvasShapeType::BITMAP: {
      auto *i = static_cast<Bitmap *>(s);

      auto qimage = i->getImage()->getQImage();

      p->drawImage(i->pos().x, i->pos().y, qimage);

      break;
    }
    default:
      break;
  }

  p->restore();
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

//---

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
  CQUtil::connectDisconnect(b, qcheck_,
    SIGNAL(stateChanged(int)), this, SLOT(stateChangedSlot(int)));
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
  else if (name == "-justify") {
    tk_->TODO(name);
  }
  else if (name == "-indicatoron") {
    bool b;
    if (! CTkAppUtil::stringToBool(value, b))
      return tk_->throwError("Invalid " + name + " \"" + value + "\"");

    showIndicator_ = b;
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
      setOverRaised(true);
    else
      setOverRaised(false);

    appearanceChanged();
  }
  else if (name == "-selectcolor") {
    QColor c;
    if (CTkAppUtil::stringToQColor(value, c))
      selectColor_ = c;
  }
  else if (name == "-selectimage") {
    tk_->TODO(name);
  }
  else if (name == "-state") {
    if (! CTkAppUtil::setWidgetState(this, value))
      return false;
  }
  else if (name == "-variable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasGlobalVar(varName_))
      updateToVar();

    updateFromVar();

    varProc_ = new CTkAppCheckButtonVarProc(tk_, this);

    tk_->traceGlobalVar(varName_, varProc_);
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
    tk_->TODO(args);
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
  CTkAppWidget::setText(text);

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
  if (varName_ != "" && tk_->hasGlobalVar(varName_)) {
    bool checked = false;

    if      (onValue() && tk_->getStringGlobalVar(varName_) == *onValue())
      checked = true;
    else if (offValue() && tk_->getStringGlobalVar(varName_) == *offValue())
      checked = true;
    else {
      bool checked1 = tk_->getBoolGlobalVar(varName_);

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
    if (varProc_)
      varProc_->setEnabled(false);

    if (qcheck_->isChecked()) {
      if (onValue())
        tk_->setStringGlobalVar(varName_, *onValue());
      else
        tk_->setBoolGlobalVar(varName_, true);
    }
    else {
      if (offValue())
        tk_->setStringGlobalVar(varName_, *offValue());
      else
        tk_->setBoolGlobalVar(varName_, false);
    }

    if (varProc_)
      varProc_->setEnabled(true);
  }
}

//---

CTkAppCheckButtonWidget::
CTkAppCheckButtonWidget(CTkAppCheckButton *check) :
 QCheckBox(check->getParent() ? check->getParent()->getQWidget() : nullptr)
{
  setObjectName("qcheck");
}

//----------

class CTkAppComboBoxVarProc : public CTclTraceProc {
 public:
  CTkAppComboBoxVarProc(CTkApp *tk, CTkAppComboBox *combo) :
   CTclTraceProc(tk), combo_(combo) {
  }

  void handleWrite(const char *) override {
    combo_->updateFromVar();
  }

 private:
  CTkAppComboBox *combo_ { nullptr };
};

//---

CTkAppComboBox::
CTkAppComboBox(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qcombo_ = new CTkAppComboBoxWidget(this);

  setQWidget(qcombo_);

  connectSlots(true);
}

void
CTkAppComboBox::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qcombo_,
    SIGNAL(currentIndexChanged(int)), this, SLOT(indexChangedSlot(int)));
}

bool
CTkAppComboBox::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-exportselection") {
    tk_->TODO(name);
  }
  else if (name == "-justify") {
    tk_->TODO(name);
  }
  else if (name == "-height") {
    tk_->TODO(name);
  }
  else if (name == "-postcommand") {
    tk_->TODO(name);
  }
  else if (name == "-state") {
    if (! CTkAppUtil::setWidgetState(this, value))
      return false;
  }
  else if (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasGlobalVar(varName_))
      tk_->setStringGlobalVar(varName_, "");

    if (tk_->hasGlobalVar(varName_))
      setText(tk_->getStringGlobalVar(varName_));

    varProc_ = new CTkAppComboBoxVarProc(tk_, this);

    tk_->traceGlobalVar(varName_, varProc_);
  }
  else if (name == "-values") {
    std::vector<std::string> strs;
    if (! tk_->splitList(value, strs))
      return tk_->throwError("Invalid values \"" + value + "\"");

    QStringList qstrs;

    for (const auto &str : strs)
      qstrs << QString::fromStdString(str);

    qcombo_->clear();
    qcombo_->addItems(qstrs);
  }
  else if (name == "-width") {
    long w;
    if (! CTkAppUtil::stringToInt(value, w))
      return tk_->throwError("Invalid width \"" + value + "\"");
    qcombo_->setWidth(w);
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppComboBox::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  auto arg = args[0];

  //--

  auto stringToIndex = [&](const std::string &str, int &ind) {
    long l;
    if (! CTkAppUtil::stringToInt(str, l))
      return tk_->throwError("Incorrect index \"" + str + "\"");
    ind = l;
    return true;
  };

  //--

  if      (arg == "bbox") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " bbox index");

    tk_->TODO(args);
  }
  else if (arg == "current") {
    if (numArgs != 1 && numArgs != 2)
      return tk_->wrongNumArgs(getName() + " current ?newIndex?");

    if (numArgs == 1) {
      auto i = qcombo_->currentIndex();

      tk_->setIntegerResult(i);
    }
    else {
      int i;
      if (! stringToIndex(args[1], i))
        return false;
      qcombo_->setCurrentIndex(i);
    }
  }
  else if (arg == "delete") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs(getName() + " delete firstIndex ?lastIndex?");

    int firstIndex;
    if (! stringToIndex(args[1], firstIndex))
      return false;

    int lastIndex = -1;
    if (numArgs == 3) {
      if (! stringToIndex(args[2], lastIndex))
        return false;
    }

    tk_->TODO(args); // delete items
  }
  else if (arg == "get") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " get");

    tk_->TODO(args);
  }
  else if (arg == "icursor") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " icursor pos");

    tk_->TODO(args);
  }
  else if (arg == "identify") {
    if (numArgs != 3 && numArgs != 4)
      return tk_->wrongNumArgs(getName() + " identify ?what? x y");

    tk_->TODO(args);
  }
  else if (arg == "index") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " index string");

    tk_->TODO(args);
  }
  else if (arg == "insert") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getName() + " insert index text");

    int ind;
    if (! stringToIndex(args[1], ind))
      return false;

    qcombo_->insertItem(ind, QString::fromStdString(args[2]));
  }
  else if (arg == "instate") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs(getName() + " instate state-spec ?script?");

    tk_->TODO(args);
  }
  else if (arg == "selection") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " selection option ?arg ...?");

    tk_->TODO(args);
  }
  else if (arg == "state") {
    if (numArgs > 2)
      return tk_->wrongNumArgs(getName() + " state state-spec");

    tk_->TODO(args);
  }
  else if (arg == "set") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " set value");

    tk_->TODO(args);
  }
  else if (arg == "validate") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " validate");

    tk_->TODO(args);
  }
  else if (arg == "xview") {
    tk_->TODO(args);
    tk_->setStringResult("0.0 1.0");
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppComboBox::
indexChangedSlot(int i)
{
  if (varName_ != "") {
    if (varProc_)
      varProc_->setEnabled(false);

    tk_->setIntegerGlobalVar(varName_, i);

    if (varProc_)
      varProc_->setEnabled(true);
  }
}

void
CTkAppComboBox::
updateFromVar()
{
  if (varName_ != "" && tk_->hasGlobalVar(varName_))
    qcombo_->setCurrentIndex(tk_->getIntegerGlobalVar(varName_));
}

//---

CTkAppComboBoxWidget::
CTkAppComboBoxWidget(CTkAppComboBox *combo) :
 QComboBox(combo->getParent() ? combo->getParent()->getQWidget() : nullptr), combo_(combo)
{
  setObjectName("qwcombo");
}

QSize
CTkAppComboBoxWidget::
sizeHint() const
{
  auto s = QComboBox::sizeHint();

  QFontMetrics fm(font());

  if (width_ > 0)
    s.setWidth(width_*fm.horizontalAdvance("0"));

  return s;
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

//---

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

//---

CTkAppEntry::
CTkAppEntry(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
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
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-show") {
    qedit_->setEchoMode(QLineEdit::Password);
  }
  else if (name == "-state") {
    if (value == "readonly")
      qedit_->setReadOnly(true);
    else {
      if (! CTkAppUtil::setWidgetState(this, value))
        return false;
    }
  }
  else if (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasGlobalVar(varName_))
      tk_->setStringGlobalVar(varName_, "");

    if (tk_->hasGlobalVar(varName_))
      setText(tk_->getStringGlobalVar(varName_));

    varProc_ = new CTkAppEntryVarProc(tk_, this);

    tk_->traceGlobalVar(varName_, varProc_);
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
  else if (name == "-width") {
    long w;
    if (! CTkAppUtil::stringToInt(value, w))
      return tk_->throwError("Invalid width \"" + value + "\"");
    qedit_->setWidth(w);
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

  auto stringToIndex = [&](const std::string &str, int &ind) {
    if (! CTkAppUtil::stringToIndex(str, ind))
      return false;
    if (ind == CTkAppUtil::END_INDEX)
      ind = qedit_->text().length() - 1;
    return true;
  };

  if      (arg == "bbox") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " bbox index");

    tk_->TODO(args);
  }
  else if (arg == "delete") {
    if (numArgs != 3 && numArgs != 4)
      return tk_->wrongNumArgs(getName() + " delete firstIndex ?lastIndex?");

    int startIndex, endIndex;
    if (! stringToIndex(args[1], startIndex))
      return false;
    if (! stringToIndex(args[2], endIndex))
      return false;

    qedit_->setSelection(startIndex, endIndex - startIndex);
    qedit_->del();
  }
  else if (arg == "get") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " get");

    tk_->setStringResult(qedit_->text().toStdString());
  }
  else if (arg == "icursor") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " icursor pos");

    tk_->TODO(args);
  }
  else if (arg == "index") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " index string");

    tk_->TODO(args);
  }
  else if (arg == "insert") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getName() + " insert index text");

    int ind;
    if (! stringToIndex(args[1], ind))
      return false;

    qedit_->setCursorPosition(ind);

    qedit_->insert(QString::fromStdString(args[2]));
  }
  else if (arg == "scan") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getName() + " scan mark|dragto x");

    tk_->TODO(args);
  }
  else if (arg == "selection") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " selection option ?index?");

    if      (args[1] == "adjust") {
      tk_->TODO(arg + " " + args[1]);
    }
    else if (args[1] == "clear") {
      if (numArgs != 2)
        return tk_->wrongNumArgs(getName() + " selection clear");

      qedit_->deselect();
    }
    else if (args[1] == "from") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getName() + " selection from index");

      int ind;
      if (! stringToIndex(args[2], ind))
        return false;

      qedit_->setCursorPosition(ind);
    }
    else if (args[1] == "present") {
      tk_->TODO(arg + " " + args[1]);
    }
    else if (args[1] == "range") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getName() + " selection clear");

      int startIndex, endIndex;
      if (! stringToIndex(args[2], startIndex))
        return false;
      if (! stringToIndex(args[3], endIndex))
        return false;

      qedit_->setSelection(startIndex, endIndex - startIndex);
    }
    else if (args[1] == "to") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getName() + " selection to index");

      int endIndex;
      if (! stringToIndex(args[2], endIndex))
        return false;

      int startIndex = qedit_->cursorPosition();

      qedit_->setSelection(startIndex, endIndex - startIndex);
    }
    else
      return false;
  }
  else if (arg == "validate") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " validate");

    tk_->TODO(args);
  }
  else if (arg == "xview") {
    tk_->TODO(args);
    tk_->setStringResult("0.0 1.0");
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppEntry::
setText(const std::string &text)
{
  CTkAppWidget::setText(text);

  qedit_->setText(QString::fromStdString(text));
}

void
CTkAppEntry::
valueChangedSlot()
{
  if (varName_ != "") {
    if (varProc_)
      varProc_->setEnabled(false);

    if (! tk_->hasGlobalVar(varName_))
      tk_->setStringGlobalVar(varName_, "");

    tk_->setStringGlobalVar(varName_, qedit_->text().toStdString());

    if (varProc_)
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

    bool b = false;
    std::string res;
    if (tk_->getStringResult(res)) {
      if (! CTkAppUtil::stringToBool(res, b))
        return false;
    }
    return b;
  }

  return true;
}

void
CTkAppEntry::
updateFromVar()
{
  if (varName_ != "" && tk_->hasGlobalVar(varName_))
    setText(tk_->getStringGlobalVar(varName_));
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

//----------

CTkAppFrame::
CTkAppFrame(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qframe_ = new QFrame(parent ? parent->getQWidget() : nullptr);

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

//---

CTkAppLabel::
CTkAppLabel(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qlabel_ = new CQLabelImage(parent ? parent->getQWidget() : nullptr);

  setQWidget(qlabel_);
}

bool
CTkAppLabel::
execConfig(const std::string &name, const std::string &value)
{
  long underlinePos = -1;

  if      (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasGlobalVar(varName_))
      tk_->setStringGlobalVar(varName_, "");

    if (tk_->hasGlobalVar(varName_))
      setText(tk_->getStringGlobalVar(varName_));

    varProc_ = new CTkAppLabelVarProc(tk_, this);

    tk_->traceGlobalVar(varName_, varProc_);
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
    if (! CTkAppUtil::setWidgetState(this, value))
      return false;
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
  CTkAppWidget::setText(text);

  qlabel_->setLabel(QString::fromStdString(text));
}

void
CTkAppLabel::
setImage(const CTkAppImageRef &image)
{
  CTkAppWidget::setImage(image);

  auto qimage = image->getQImage();

  qlabel_->setImage(qimage);
}

void
CTkAppLabel::
updateFromVar()
{
  if (varName_ != "" && tk_->hasGlobalVar(varName_))
    setText(tk_->getStringGlobalVar(varName_));
}

//----------

CTkAppLabelFrame::
CTkAppLabelFrame(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qframe_ = new QGroupBox(parent ? parent->getQWidget() : nullptr);

  setQWidget(qframe_);
}

bool
CTkAppLabelFrame::
execConfig(const std::string &name, const std::string &value)
{
  return CTkAppWidget::execConfig(name, value);
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
  CTkAppWidget::setText(text);

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

//---

CTkAppListBox::
CTkAppListBox(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qlist_ = new CTkAppListBoxWidget(this);

  qlist_->verticalScrollBar  ()->setVisible(false);
  qlist_->horizontalScrollBar()->setVisible(false);

  setQWidget(qlist_);

  connectSlots(true);
}

void
CTkAppListBox::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qlist_,
    SIGNAL(itemSelectionChanged()), this, SLOT(selectionSlot()));
  CQUtil::connectDisconnect(b, qlist_->verticalScrollBar(),
    SIGNAL(valueChanged(int)), this, SLOT(vscrollSlot(int)));
  CQUtil::connectDisconnect(b, qlist_->horizontalScrollBar(),
    SIGNAL(valueChanged(int)), this, SLOT(hscrollSlot(int)));
}

bool
CTkAppListBox::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-activestyle") {
    tk_->TODO(name);
  }
  else if (name == "-exportselection") {
    bool b;
    if (! CTkAppUtil::stringToBool(value, b))
      return false;
    exportSelection_ = b;
  }
  else if (name == "-height") {
    double h;
    if (! CTkAppUtil::stringToDistance(value, h))
      return tk_->throwError("Invalid height \"" + value + "\"");
    qlist_->setHeight(CTkAppListBoxWidget::OptReal(h));
  }
  else if (name == "-listvariable") {
    varName_ = value;

    updateFromVar();

    varProc_ = new CTkAppListBoxVarProc(tk_, this);

    tk_->traceGlobalVar(varName_, varProc_);
  }
  else if (name == "-selectmode") {
    if      (value == "single") {
      qlist_->setSelectionMode(QAbstractItemView::SingleSelection);
    }
    else if (value == "browse") {
      qlist_->setSelectionMode(QAbstractItemView::SingleSelection); // TODO
    }
    else if (value == "multiple") {
      qlist_->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    else if (value == "extended") {
      qlist_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
    else
      return false;
  }
  else if (name == "-width") {
    double w;
    if (! CTkAppUtil::stringToDistance(value, w))
      return tk_->throwError("Invalid width \"" + value + "\"");

    qlist_->setWidth(CTkAppListBoxWidget::OptReal(w));
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

  //---

  auto stringToIndex = [&](const std::string &str, int &ind) {
    ind = -1;

    if (str == "active") return false;
    if (str == "anchor") return false;

    if (str == "end") {
      if (qlist_->count() == 0)
        return false;

      ind = qlist_->count() - 1;

      return true;
    }

    if (str[0] == '@') {
      return false; // @x,y
    }

    long i;
    if (! CTkAppUtil::stringToInt(str, i))
      return false;
    if (i < 0 || i >= qlist_->count())
      return false;
    ind = int(i);
    return true;
  };

  auto stringToItem = [&](const std::string &str) {
    int i;
    if (! stringToIndex(str, i))
      return static_cast<QListWidgetItem *>(nullptr);

    return qlist_->item(i);
  };

  //---

  const auto &arg = args[0];

  if      (arg == "activate") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " activate index");

    tk_->TODO(args);
  }
  else if (arg == "bbox") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " bbox index");

    auto *item = stringToItem(args[1]);
    if (! item) return false;

    auto r = qlist_->visualItemRect(item);

    tk_->setIntegerArrayResult({{r.x(), r.y(), r.width(), r.height()}});
  }
  else if (arg == "curselection") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " curselection");

    auto items = qlist_->selectedItems();

    std::vector<int> rows;

    for (auto *item : items)
      rows.push_back(qlist_->row(item));

    tk_->setIntegerArrayResult(rows);
  }
  else if (arg == "delete") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs(getName() + " delete firstIndex ?lastIndex?");

    int startIndex, endIndex;

    if (! stringToIndex(args[1], startIndex))
      return false;

    if (numArgs == 3) {
      if (! stringToIndex(args[2], endIndex))
        return false;
    }
    else
      endIndex = CTkAppUtil::END_INDEX;

    for (int i = endIndex; i >= startIndex; --i)
      delete qlist_->item(i);
  }
  else if (arg == "get") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs(getName() + " get firstIndex ?lastIndex?");

    int startIndex, endIndex;

    if (! stringToIndex(args[1], startIndex))
      return false;

    if (numArgs == 3) {
      if (! stringToIndex(args[2], endIndex))
        return false;
    }
    else
      endIndex = CTkAppUtil::END_INDEX;

    std::vector<std::string> strs;

    for (int i = endIndex; i >= startIndex; --i)
      strs.push_back(qlist_->item(i)->text().toStdString());

    tk_->setStringArrayResult(strs);
  }
  else if (arg == "index") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " index index");

    tk_->TODO(args);
  }
  else if (arg == "insert") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " insert index ?element ...?");

    int ind;
    if (! stringToIndex(args[1], ind))
      return false;

    uint i = 2;

    std::vector<std::string> strs;

    for ( ; i < numArgs; ++i) {
      qlist_->insertItem(ind++, QString::fromStdString(args[i]));
    }
  }
  else if (arg == "itemcget") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getName() + " itemcget index option");

    int ind;
    if (! stringToIndex(args[1], ind))
      return false;

    auto opt = args[2];

    if      (opt == "-background") {
      tk_->TODO(arg + " " + opt);
    }
    else if (opt == "-foreground") {
      tk_->TODO(arg + " " + opt);
    }
    else if (opt == "-selectbackground") {
      tk_->TODO(arg + " " + opt);
    }
    else if (opt == "-selectforeground") {
      tk_->TODO(arg + " " + opt);
    }
    else
      return false;
  }
  else if (arg == "itemconfigure") {
    if (numArgs <= 2)
      return tk_->wrongNumArgs(getName() + " itemcget index option");

    int ind;
    if (! stringToIndex(args[1], ind))
      return false;

    if      (numArgs == 2) {
      tk_->TODO(args);
    }
    else if (numArgs == 3) {
      tk_->TODO(arg + " " + args[2]);
    }
    else {
      uint i = 2;

      for ( ; i < numArgs; ++i) {
        auto name  = args[i++];
      //auto value = (i < numArgs ? args[i] : "");

        if      (name == "-background") {
          tk_->TODO(args);
        }
        else if (name == "-foreground") {
          tk_->TODO(args);
        }
        else if (name == "-selectbackground") {
          tk_->TODO(args);
        }
        else if (name == "-selectforeground") {
          tk_->TODO(args);
        }
        else
          return false;
      }
    }
  }
  else if (arg == "nearest") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " nearest y");

    long y;
    if (! CTkAppUtil::stringToInt(args[1], y))
      return false;

    tk_->TODO(args);
  }
  else if (arg == "scan") {
    if (numArgs != 4)
      return tk_->wrongNumArgs(getName() + " scan mark|dragto x y");

    tk_->TODO(args);
  }
  else if (arg == "see") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " see index");

    tk_->TODO(args);
  }
  else if (arg == "selection") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getName() + " selection option index ?index?");

    tk_->TODO(args);
  }
  else if (arg == "size") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " size");

    tk_->setIntegerResult(qlist_->count());
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
          return tk_->wrongNumArgs(getName() + " xview moveto number");

        double x;

        if (! tk_->getOptionReal(opt, args[2], x))
          return false;

        qlist_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
      else if (opt == "scroll") {
        if (numArgs < 4)
          return tk_->wrongNumArgs(getName() + " xview scroll number pages|units");

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
          return tk_->wrongNumArgs(getName() + " yview moveto number");

        double y;

        if (! tk_->getOptionReal(opt, args[2], y))
          return false;

        qlist_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
      else if (opt == "scroll") {
        if (numArgs < 4)
          return tk_->wrongNumArgs(getName() + " yview scroll number pages|units");

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
selectionSlot()
{
  CTkAppEventData eventData;

  eventData.type  = CTkAppEventType::Virtual;
  eventData.vtype = CTkAppVirtualEventType::ListboxSelect;

  triggerVirtualEvents(eventData);
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
  if (varName_ != "" && tk_->hasGlobalVar(varName_)) {
    std::vector<std::string> strs;

    if (! tk_->getStringArrayGlobalVar(varName_, strs))
      return;

     qlist_->clear();

     for (const auto &str : strs)
       qlist_->addItem(QString::fromStdString(str));
  }
}

//---

CTkAppListBoxWidget::
CTkAppListBoxWidget(CTkAppListBox *listbox) :
 QListWidget(listbox->getParent() ? listbox->getParent()->getQWidget() : nullptr),
 listbox_(listbox)
{
  setObjectName("qlistbox");
}

void
CTkAppListBoxWidget::
setWidth(const OptReal &w)
{
  width_ = w;
}

void
CTkAppListBoxWidget::
setHeight(const OptReal &h)
{
  height_ = h;
}

QSize
CTkAppListBoxWidget::
sizeHint() const
{
  auto s = QListWidget::sizeHint();

  QFontMetrics fm(font());

  if (width_)
    s.setWidth((*width_)*fm.horizontalAdvance("0"));

  if (height_)
    s.setHeight((*height_)*fm.height());

  return s;
}

//----------

CTkAppMenu::
CTkAppMenu(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qmenu_ = new QMenu(parent ? parent->getQWidget() : nullptr);

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
        auto label1 = CTkAppUtil::underlineLabel(label, underlinePos);

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
  else if (arg == "entrycget") {
  }
  else if (arg == "entryconfig" || arg == "entryconfigure") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getName() + " entryconfigure index ?-option value ...?");

    auto pattern = args[1];

    uint i = 2;

    std::string state;

    for ( ; i < numArgs; ++i) {
      auto opt = args[i];

      if (opt == "-state") {
        ++i;

        if (i >= numArgs)
          return false;

        state = args[i];
      }
      else {
        tk_->TODO(getName() + " entryconfigure " + pattern + " " + opt);
      }
    }

    CGlob glob(pattern);

    for (auto *action : qmenu_->actions()) {
      if (glob.compare(action->text().toStdString())) {
        if (state != "") {
          if      (state == "normal")
            action->setEnabled(true);
          else if (state == "disabled")
            action->setEnabled(false);
          else if (state != "")
            tk_->TODO(getName() + " entryconfigure -state " + state);
        }
      }
    }
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
    tk_->setBoolGlobalVar(varName_, check_->isChecked());
  }
}

void
CTkAppRadioAction::
updateFromVar()
{
  if (varName_ != "") {
    if (value_ != "")
      tk_->setStringGlobalVar(varName_, value_);
    else
      tk_->setBoolGlobalVar(varName_, radio_->isChecked());
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

//---

CTkAppMenuButton::
CTkAppMenuButton(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qbutton_ = new QToolButton(parent ? parent->getQWidget() : nullptr);

  qbutton_->setToolButtonStyle(Qt::ToolButtonTextOnly);

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
execConfig(const std::string &name, const std::string &value)
{
  long underlinePos = -1;

  if      (name == "-command") {
    setCommand(value);
  }
  else if (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasGlobalVar(varName_))
      tk_->setStringGlobalVar(varName_, "");

    if (tk_->hasGlobalVar(varName_))
      setText(tk_->getStringGlobalVar(varName_));

    varProc_ = new CTkAppMenuButtonVarProc(tk_, this);

    tk_->traceGlobalVar(varName_, varProc_);
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
    bool b;
    if (! CTkAppUtil::stringToBool(value, b))
      return tk_->throwError("Invalid " + name + " \"" + value + "\"");

    showIndicator_ = b;
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

  if (arg == "invoke") {
    runCommand();
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppMenuButton::
setText(const std::string &text)
{
  CTkAppWidget::setText(text);

  qbutton_->setText(QString::fromStdString(text));
}

void
CTkAppMenuButton::
setImage(const CTkAppImageRef &image)
{
  CTkAppWidget::setImage(image);

  auto pixmap = image->getQPixmap();

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
  if (varName_ != "" && tk_->hasGlobalVar(varName_))
    setText(tk_->getStringGlobalVar(varName_));
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

//---

CTkAppMessage::
CTkAppMessage(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qedit_ = new QTextEdit(parent ? parent->getQWidget() : nullptr);

  qedit_->setReadOnly(true);

  setQWidget(qedit_);
}

bool
CTkAppMessage::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasGlobalVar(varName_))
      tk_->setStringGlobalVar(varName_, "");

    if (tk_->hasGlobalVar(varName_))
      setText(tk_->getStringGlobalVar(varName_));

    varProc_ = new CTkAppMessageVarProc(tk_, this);

    tk_->traceGlobalVar(varName_, varProc_);
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
  CTkAppWidget::setText(text);

  qedit_->setText(QString::fromStdString(text));
}

void
CTkAppMessage::
updateFromVar()
{
  if (varName_ != "" && tk_->hasGlobalVar(varName_))
    setText(tk_->getStringGlobalVar(varName_));
}

//----------

CTkAppNoteBook::
CTkAppNoteBook(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qtab_ = new QTabWidget(parent ? parent->getQWidget() : nullptr);

  setQWidget(qtab_);
}

bool
CTkAppNoteBook::
execConfig(const std::string &name, const std::string &value)
{
  return CTkAppWidget::execConfig(name, value);
}

bool
CTkAppNoteBook::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  auto arg = args[0];

  auto stringToIndex = [&](const std::string &str, int &ind) {
    if (! CTkAppUtil::stringToIndex(str, ind))
      return false;
    if (ind == CTkAppUtil::END_INDEX)
      ind = qtab_->count() - 1;
    return true;
  };

  if      (arg == "add") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " add window ?-option value ...?");

    auto *w = tk_->lookupWidgetByName(args[1], /*quiet*/true);
    if (! w) return false;

    qtab_->addTab(w->qwidget(), "");
  }
  else if (arg == "configure" || arg == "config") {
    if (numArgs == 1)
      tk_->setStringResult("{-width width Width 0 0} "
                           "{-height height Height 0 0} "
                           "{-padding padding Padding {} {}} "
                           "{-takefocus takeFocus TakeFocus ttk::takefocus ttk::takefocus} "
                           "{-cursor cursor Cursor {} {}} "
                           "{-style style Style {} {}} "
                           "{-class {} {} {} {}}");
    else
      tk_->TODO(args);
  }
  else if (arg == "cget") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " cget option");

    tk_->TODO(args);
  }
  else if (arg == "forget") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " forget tab");

    tk_->TODO(args);
  }
  else if (arg == "hide") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " hide tab");

    tk_->TODO(args);
  }
  else if (arg == "identify") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " identify ?what? x y");

    tk_->TODO(args);
  }
  else if (arg == "index") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " index tab");

    tk_->TODO(args);
  }
  else if (arg == "insert") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " insert index slave ?-option value ...?");

    int ind;
    if (! stringToIndex(args[1], ind))
      return false;

    tk_->TODO(args);
  }
  else if (arg == "instate") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " instate state-spec ?script?");

    tk_->TODO(args);
  }
  else if (arg == "select") {
    tk_->TODO(args);
  }
  else if (arg == "state") {
    tk_->TODO(args);
  }
  else if (arg == "tab") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " tab tab ?-option ?value??...");

    tk_->TODO(args);
  }
  else if (arg == "tabs") {
    tk_->TODO(args);
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

//----------

CTkAppPanedWindow::
CTkAppPanedWindow(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qpane_ = new QSplitter(parent ? parent->getQWidget() : nullptr);

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
    tk_->TODO(args);
  }
  else if (arg == "identify") {
    tk_->TODO(args);
  }
  else if (arg == "panecget") {
    tk_->TODO(args);
  }
  else if (arg == "paneconfigure") {
    tk_->TODO(args);
  }
  else if (arg == "panes") {
    tk_->TODO(args);
  }
  else if (arg == "proxy") {
    tk_->TODO(args);
  }
  else if (arg == "sash") {
    tk_->TODO(args);
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

//---

CTkAppRadioButton::
CTkAppRadioButton(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
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
    tk_->untraceGlobalVar(varName_, varProc_);
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
execConfig(const std::string &name, const std::string &value)
{
  long underlinePos = -1;

  if      (name == "-variable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasGlobalVar(varName_))
      updateToVar();

    updateFromVar();

    varProc_ = new CTkAppRadioButtonVarProc(tk_, this);

    tk_->traceGlobalVar(varName_, varProc_);
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
    bool b;
    if (! CTkAppUtil::stringToBool(value, b))
      return tk_->throwError("Invalid " + name + " \"" + value + "\"");

    showIndicator_ = b;
  }
  else if (name == "-selectcolor") {
    QColor c;
    if (CTkAppUtil::stringToQColor(value, c))
      selectColor_ = c;
  }
  else if (name == "-state") {
    if (! CTkAppUtil::setWidgetState(this, value))
      return false;
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
    tk_->TODO(args);
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
  CTkAppWidget::setText(text);

  qradio_->setText(QString::fromStdString(text));
}

void
CTkAppRadioButton::
setImage(const CTkAppImageRef &image)
{
  CTkAppWidget::setImage(image);

  auto qimage = image->getQImage();

  qradio_->setImage(qimage);
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
  if (varName_ != "" && tk_->hasGlobalVar(varName_))
    setValue(tk_->getStringGlobalVar(varName_));
}

void
CTkAppRadioButton::
updateToVar()
{
  if (varName_ != "") {
    if (varProc_)
      varProc_->setEnabled(false);

    if (qradio_->isChecked()) {
      if (value_ != "")
        tk_->setStringGlobalVar(varName_, value_);
      else
        tk_->setBoolGlobalVar(varName_, qradio_->isChecked());
    }

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
  else
    return s;
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

//---

CTkAppScale::
CTkAppScale(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
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
    SIGNAL(valueChanged(int)), this, SLOT(valueSlot(int)));
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
    if (! CTkAppUtil::setWidgetState(this, value))
      return false;
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

    if (! isInitNotify() && ! tk_->hasGlobalVar(varName_))
      tk_->setIntegerGlobalVar(varName_, 0);

    updateFromVar();

    varProc_ = new CTkAppScaleVarProc(tk_, this);

    tk_->traceGlobalVar(varName_, varProc_);
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
    tk_->TODO(args);
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
  if (varName_ != "" && tk_->hasGlobalVar(varName_))
    qscale_->setValue(tk_->getIntegerGlobalVar(varName_));
}

void
CTkAppScale::
updateToVar()
{
  if (varName_ != "" && tk_->hasGlobalVar(varName_)) {
    if (varProc_)
      varProc_->setEnabled(false);

    tk_->setIntegerGlobalVar(varName_, qscale_->value());

    if (varProc_)
      varProc_->setEnabled(true);
  }
}

//----------

CTkAppScrollBar::
CTkAppScrollBar(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qscrollbar_ = new QScrollBar(parent ? parent->getQWidget() : nullptr);

  qscrollbar_->setRange(0, 1000);

  setQWidget(qscrollbar_);

  connectSlots(true);
}

void
CTkAppScrollBar::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qscrollbar_,
    SIGNAL(valueChanged(int)), this, SLOT(scrollSlot(int)));
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

//---

class CTkAppSpinBoxValidator : public QValidator {
 public:
  CTkAppSpinBoxValidator(CTkApp *tk, CTkAppSpinBox *spin) :
   QValidator(spin), tk_(tk), spin_(spin) {
  }

  State validate(QString &input, int & /*pos*/) const override {
    if (spin_->validate(input.toStdString()))
      return Acceptable;

    return Invalid;
  }

 private:
  CTkApp*        tk_   { nullptr };
  CTkAppSpinBox* spin_ { nullptr };
};

//---

CTkAppSpinBox::
CTkAppSpinBox(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qspin_ = new CTkAppSpinBoxWidget(this);

  qspin_->setFocusPolicy(Qt::StrongFocus);

  setQWidget(qspin_);

  connectSlots(true);
}

void
CTkAppSpinBox::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qspin_,
    SIGNAL(editingFinished()), this, SLOT(valueChangedSlot()));
}

bool
CTkAppSpinBox::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-buttonbackground") {
    tk_->TODO(name);
  }
  else if (name == "-buttoncursor") {
    tk_->TODO(name);
  }
  else if (name == "-buttondownrelief") {
    tk_->TODO(name);
  }
  else if (name == "-buttonuprelief") {
    tk_->TODO(name);
  }
  else if (name == "-command") {
    setCommand(value);
  }
  else if (name == "-disabledbackground") {
    tk_->TODO(name);
  }
  else if (name == "-disabledforeground") {
    tk_->TODO(name);
  }
  else if (name == "-format") {
    tk_->TODO(name);
  }
  else if (name == "-from") {
    long from;
    if (! tk_->getOptionInt(name, value, from))
      return false;

    qspin_->setMinimum(from);
  }
  else if (name == "-invalidcommand") {
    setInvalidCommand(value);
  }
  else if (name == "-increment") {
    long step;
    if (! tk_->getOptionInt(name, value, step))
      return false;

    qspin_->setSingleStep(step);
  }
  else if (name == "-readonlybackground") {
    tk_->TODO(name);
  }
  else if (name == "-state") {
    if (value == "readonly")
      qspin_->setReadOnly(true);
    else {
      if (! CTkAppUtil::setWidgetState(this, value))
        return false;
    }
  }
  else if (name == "-textvariable") {
    varName_ = value;

    if (! isInitNotify() && ! tk_->hasGlobalVar(varName_))
      tk_->setStringGlobalVar(varName_, "");

    if (tk_->hasGlobalVar(varName_))
      setText(tk_->getStringGlobalVar(varName_));

    varProc_ = new CTkAppSpinBoxVarProc(tk_, this);

    tk_->traceGlobalVar(varName_, varProc_);
  }
  else if (name == "-to") {
    long to;
    if (! tk_->getOptionInt(name, value, to))
      return false;

    qspin_->setMaximum(to);
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
      validateProc_ = new CTkAppSpinBoxValidator(tk_, this);

      qspin_->setValidator(validateProc_);
    }
  }
  else if (name == "-validatecommand") {
    validateCmd_ = value;
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
  else if (name == "-width") {
    long w;
    if (! CTkAppUtil::stringToInt(value, w))
      return tk_->throwError("Invalid width \"" + value + "\"");
    qspin_->setWidth(w);
  }
  else if (name == "-wrap") {
    tk_->TODO(name);
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

  auto stringToIndex = [&](const std::string &str, int &ind) {
    if (! CTkAppUtil::stringToIndex(str, ind))
      return false;
    //if (ind == CTkAppUtil::END_INDEX)
    //  ind = qedit_->text().length() - 1;
    return true;
  };

  if      (arg == "bbox") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " bbox index");

    int index;
    if (! stringToIndex(args[1], index))
      return false;
  }
  else if (arg == "delete") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs(getName() + " delete firstIndex ?lastIndex?");

    int startIndex, endIndex;
    if (! stringToIndex(args[1], startIndex))
      return false;
    if (! stringToIndex(args[2], endIndex))
      return false;

    qspin_->deleteChars(startIndex, endIndex - startIndex);
  }
  else if (arg == "get") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " get");

    tk_->setStringResult(qspin_->text().toStdString());
  }
  else if (arg == "icursor") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " icursor pos");

    tk_->TODO(args);
  }
  else if (arg == "identify") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getName() + " identify x y");

    tk_->TODO(args);
  }
  else if (arg == "index") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " index string");

    tk_->TODO(args);
  }
  else if (arg == "insert") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getName() + " insert index text");

    int ind;
    if (! stringToIndex(args[1], ind))
      return false;

    qspin_->insertChars(ind, QString::fromStdString(args[2]));
  }
  else if (arg == "invoke") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " invoke elemName");

    tk_->TODO(args);
  }
  else if (arg == "scan") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getName() + " scan mark|dragto x");

    tk_->TODO(args);
  }
  else if (arg == "selection") {
    if (numArgs != 2 && numArgs != 2)
      return tk_->wrongNumArgs(getName() + " selection option ?index?");

    if      (args[1] == "adjust") {
      tk_->TODO(arg + " " + args[1]);
    }
    else if (args[1] == "clear") {
      if (numArgs != 2)
        return tk_->wrongNumArgs(getName() + " selection clear");

      qspin_->deselectAll();
    }
    else if (args[1] == "from") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getName() + " selection from index");

      int ind;
      if (! stringToIndex(args[2], ind))
        return false;

      qspin_->setCursorPos(ind);
    }
    else if (args[1] == "present") {
      tk_->TODO(arg + " " + args[1]);
    }
    else if (args[1] == "range") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getName() + " selection clear");

      int startIndex, endIndex;
      if (! stringToIndex(args[2], startIndex))
        return false;
      if (! stringToIndex(args[3], endIndex))
        return false;

      //qedit_->setSelection(startIndex, endIndex - startIndex);
    }
    else if (args[1] == "to") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getName() + " selection to index");

      int endIndex;
      if (! stringToIndex(args[2], endIndex))
        return false;

      int startIndex = qspin_->cursorPos();

      qspin_->selectChars(startIndex, endIndex - startIndex);
    }
    else
      return false;

    tk_->TODO(args);
  }
  else if (arg == "set") {
    if (numArgs != 1 && numArgs != 1)
      return tk_->wrongNumArgs(getName() + " set ?string?");

    tk_->TODO(args);
  }
  else if (arg == "validate") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " validate");

    tk_->TODO(args);
  }
  else if (arg == "xview") {
    if (numArgs > 4)
      return tk_->wrongNumArgs(getName() + "xview ?moveto|scroll index?");

    tk_->TODO(args);
    tk_->setStringResult("0.0 1.0");
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppSpinBox::
setText(const std::string &)
{
  if (qspin_->mode() == CQSpinList::Mode::INTEGER)
    qspin_->setValue(tk_->getIntegerGlobalVar(varName_));
  else
    qspin_->setText(QString::fromStdString(tk_->getStringGlobalVar(varName_)));
}

void
CTkAppSpinBox::
valueChangedSlot()
{
  if (varName_ != "") {
    if (varProc_)
      varProc_->setEnabled(false);

    if (! tk_->hasGlobalVar(varName_))
      tk_->setStringGlobalVar(varName_, "");

    //tk_->setStringGlobalVar(varName_, qedit_->text().toStdString());

    if (varProc_)
      varProc_->setEnabled(true);
  }
}

bool
CTkAppSpinBox::
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

    bool b = false;
    std::string res;
    if (tk_->getStringResult(res)) {
      if (! CTkAppUtil::stringToBool(res, b))
        return false;
    }
    return b;
  }

  return true;
}

void
CTkAppSpinBox::
updateFromVar()
{
  if (varName_ != "" && tk_->hasGlobalVar(varName_))
    qspin_->setValue(tk_->getIntegerGlobalVar(varName_));
}

//---

CTkAppSpinBoxWidget::
CTkAppSpinBoxWidget(CTkAppSpinBox *spin) :
 CQSpinList(spin->getParent() ? spin->getParent()->getQWidget() : nullptr), spin_(spin)
{
  setObjectName("qspin");
}

QSize
CTkAppSpinBoxWidget::
sizeHint() const
{
  auto s = CQSpinList::sizeHint();

  QFontMetrics fm(font());

  if (width_ > 0)
    s.setWidth(width_*fm.horizontalAdvance("0"));

  return s;
}

//----------

CTkAppText::
CTkAppText(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qtext_ = new CTkAppTextWidget(this);

  qtext_->verticalScrollBar  ()->setVisible(false);
  qtext_->horizontalScrollBar()->setVisible(false);

  setQWidget(qtext_);

  connectSlots(true);
}

void
CTkAppText::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qtext_->verticalScrollBar(),
    SIGNAL(valueChanged(int)), this, SLOT(vscrollSlot(int)));
  CQUtil::connectDisconnect(b, qtext_->horizontalScrollBar(),
    SIGNAL(valueChanged(int)), this, SLOT(hscrollSlot(int)));
}

bool
CTkAppText::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-autoseparators") {
    tk_->TODO(name);
  }
  else if (name == "-blockcursor") {
    tk_->TODO(name);
  }
  else if (name == "-command") {
    setCommand(value);
  }
  else if (name == "-endline") {
    tk_->TODO(name);
  }
  else if (name == "-height") {
    long h;
    if (! CTkAppUtil::stringToInt(value, h))
      return tk_->throwError("Invalid height \"" + value + "\"");
    qtext_->setHeight(h);
  }
  else if (name == "-inactiveselectbackground") {
    tk_->TODO(name);
  }
  else if (name == "-insertunfocused") {
    tk_->TODO(name);
  }
  else if (name == "-maxundo") {
    tk_->TODO(name);
  }
  else if (name == "-spacing1") {
    tk_->TODO(name);
  }
  else if (name == "-spacing2") {
    tk_->TODO(name);
  }
  else if (name == "-spacing3") {
    tk_->TODO(name);
  }
  else if (name == "-startline") {
    setText(value);
  }
  else if (name == "-state") {
    if (! CTkAppUtil::setWidgetState(this, value))
      return false;
  }
  else if (name == "-tabs") {
    tk_->TODO(name);
  }
  else if (name == "-tabstyle") {
    tk_->TODO(name);
  }
  else if (name == "-undo") {
    tk_->TODO(name);
  }
  else if (name == "-width") {
    long w;
    if (! CTkAppUtil::stringToInt(value, w))
      return tk_->throwError("Invalid width \"" + value + "\"");
    qtext_->setWidth(w);
  }
  else if (name == "-wrap") {
    if      (value == "char") {
      qtext_->setLineWrapMode(QTextEdit::WidgetWidth);
      qtext_->setWordWrapMode(QTextOption::WrapAnywhere);
    }
    else if (value == "none") {
      qtext_->setLineWrapMode(QTextEdit::NoWrap);
    }
    else if (value == "word") {
      qtext_->setLineWrapMode(QTextEdit::WidgetWidth);
      qtext_->setWordWrapMode(QTextOption::WordWrap);
    }
    else
      return false;
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
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " bbox index");

    tk_->TODO(args);
  }
  else if (arg == "compare") {
    if (numArgs != 4)
      return tk_->wrongNumArgs(getName() + " compare index1 op index2");

    TextInd ind1;
    if (! stringToTextInd(args[1], ind1))
      return false;

    auto op = args[2];

    TextInd ind2;
    if (! stringToTextInd(args[3], ind2))
      return false;

    int cmp = TextInd::cmp(ind1, ind2);

    int res = 0;

    if (op == ">" ) res = (cmp >  0);
    if (op == ">=") res = (cmp >= 0);
    if (op == "<" ) res = (cmp <  0);
    if (op == "<=") res = (cmp <= 0);
    if (op == "==") res = (cmp == 0);
    if (op == "!=") res = (cmp != 0);

    tk_->setIntegerResult(res);
  }
  else if (arg == "count") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getName() + " count ?-option value ...? index1 index2");

    tk_->TODO(args);
  }
  else if (arg == "debug") {
    if (numArgs > 2)
      return tk_->wrongNumArgs(getName() + " debug boolean");

    tk_->TODO(args);
  }
  else if (arg == "delete") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " delete index1 ?index2 ...?");

    std::vector<TextInd> inds;

    for (uint i = 1; i < numArgs; ++i) {
      TextInd ind;
      if (! stringToTextInd(args[i], ind))
        return false;

      inds.push_back(ind);
    }

    if (inds.size() & 1)
      inds.push_back(TextInd::end());

    TextIndRanges indRanges;

    uint i = 0;

    for ( ; i < inds.size(); i += 2)
      indRanges.push_back(TextIndRange(inds[i], inds[i + 1]));

    for (const auto &indRange : indRanges) {
      auto indRange1 = remapIndRange(indRange);

      qtext_->deleteText(indRange1.start.lineNum, indRange1.start.charNum,
                         indRange1.end  .lineNum, indRange1.end  .charNum);
    }
  }
  else if (arg == "dlineinfo") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " dlineinfo index");

    tk_->TODO(args);
  }
  else if (arg == "dump") {
    if (numArgs < 1)
      return tk_->wrongNumArgs(getName() +
        " dump ?-all -image -text -mark -tag -window? ?-command script? index ?index2?");

    tk_->TODO(args);
  }
  else if (arg == "edit") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " edit option ?arg ...?");

    tk_->TODO(args);
  }
  else if (arg == "get") {
    uint i = 1;

    if (args[i] == "-displaychars")
      ++i;

    if (args[i] == "--")
      ++i;

    if (i >= numArgs - 1)
      return tk_->wrongNumArgs(getName() + " get ?-displaychars? ?--? index1 ?index2 ...?");

    std::vector<std::string> strs;

    for ( ; i < numArgs; ++i) {
      TextInd ind1;
      if (! stringToTextInd(args[i], ind1))
        return false;

      auto cursor = qtext_->textCursor();
      setCurrentInd(cursor, ind1);

      if (i < numArgs - 1) {
        ++i;

        TextInd ind2;
        if (! stringToTextInd(args[i], ind2))
          return false;

        setCurrentInd(cursor, ind2, QTextCursor::KeepAnchor);
      }
      else
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);

      strs.push_back(cursor.selectedText().toStdString());
    }

    tk_->setStringArrayResult(strs);
  }
  else if (arg == "image") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " image option ?arg ...?");

    tk_->TODO(args);
  }
  else if (arg == "index") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " index index");

    CStrParse parse(args[1]);

    parse.skipSpace();

    auto parseInd = [&](TextInd &ind) {
      if      (parse.isString("end")) {
        parse.skipLastString();
        ind.lineNum = TextInd::END;
        ind.charNum = TextInd::END;
      }
      else if (parse.isDigit()) {
        if (! parse.readInteger(&ind.lineNum))
          return false;
      }
      else
        return false;

      if (parse.isChar('.')) {
        parse.skipChar();

        if      (parse.isString("end")) {
          parse.skipLastString();
          ind.charNum = TextInd::END;
        }
        else if (parse.isDigit()) {
          if (! parse.readInteger(&ind.charNum))
            return false;
        }
        else
          return false;
      }
      return true;
    };

    TextInd ind1;
    if (! parseInd(ind1))
      return false;

    parse.skipSpace();

    if (parse.isChar('+') || parse.isChar('-')) {
      auto sign = (parse.isChar('+') ? 1 : -1);

      parse.skipChar();
      parse.skipSpace();

      if (parse.isDigit()) {
        long n;
        if (! parse.readInteger(&n))
          return false;
        n *= sign;

        parse.skipSpace();

        if      (parse.isString("chars")) {
          parse.skipLastString();

          auto cursor = qtext_->textCursor();
          setCurrentInd(cursor, ind1);
          cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, n);
          getCurrentInd(cursor, ind1);
        }
        else if (parse.isString("lines")) {
          parse.skipLastString();

          auto cursor = qtext_->textCursor();
          setCurrentInd(cursor, ind1);
          cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, n);
          getCurrentInd(cursor, ind1);
        }
        else
          return false;
      }
      else
        return false;

      parse.skipSpace();

      if      (parse.isString("linestart")) {
        parse.skipLastString();
        tk_->TODO(args);
      }
      else if (parse.isString("lineend")) {
        parse.skipLastString();
        tk_->TODO(args);
      }
      else if (parse.isString("wordstart")) {
        parse.skipLastString();
        tk_->TODO(args);
      }
      else if (parse.isString("wordend")) {
        parse.skipLastString();
        tk_->TODO(args);
      }
    }

    parse.skipSpace();
    if (! parse.eof())
      return false;

    tk_->setStringResult(ind1.toString());
  }
  else if (arg == "insert") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getName() + " insert index chars ?tagList chars tagList ...?");

    TextInd ind;
    if (! stringToTextInd(args[1], ind))
      return false;

    setCurrentInd(ind);

    qtext_->insertPlainText(QString::fromStdString(args[2]));
  }
  else if (arg == "mark") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " mark option ?arg ...?");

    auto opt = args[1];

    if      (opt == "gravity") {
      tk_->TODO(args);
    }
    else if (opt == "names") {
      std::vector<std::string> names;
      getMarkNames(names);

      tk_->setStringArrayResult(names);
    }
    else if (opt == "next") {
      tk_->TODO(args);
    }
    else if (opt == "previous") {
      tk_->TODO(args);
    }
    else if (opt == "set") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getName() + " mark set markName index");

      auto mark = args[2];

      TextInd ind;
      if (! stringToTextInd(args[3], ind))
        return false;

      setMark(mark, ind);
    }
    else if (opt == "unset") {
      tk_->TODO(args);
    }
    else
      return false;
  }
  else if (arg == "peer") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " peer option ?arg ...?");

    tk_->TODO(args);
  }
  else if (arg == "pendingsync") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " pendingsync");

    tk_->TODO(args);
  }
  else if (arg == "replace") {
    if (numArgs < 4)
      return tk_->wrongNumArgs(getName() +
        " replace index1 index2 chars ?tagList chars tagList ...?");

    tk_->TODO(args);
  }
  else if (arg == "scan") {
    if (numArgs < 4)
      return tk_->wrongNumArgs(getName() + " scan mark x y\" or \"" +
                               getName() + " scan dragto x y ?gain?");

    tk_->TODO(args);
  }
  else if (arg == "search") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getName() + " search ?switches? pattern index ?stopIndex?");

    bool        forwards     = true;
    bool        exact        = false;
    bool        regexp       = false;
    bool        nolinestop   = false;
    bool        nocase       = false;
    bool        all          = false;
    bool        overlap      = false;
    bool        strictlimits = false;
    bool        elide        = false;
    std::string countVar;

    uint i = 1;

    for ( ; i < numArgs; ++i) {
      if      (args[i] == "-forwards")
        forwards = true;
      else if (args[i] == "-backwards")
        forwards = false;
      else if (args[i] == "-exact")
        exact = true;
      else if (args[i] == "-regexp")
        regexp = true;
      else if (args[i] == "-nolinestop")
        nolinestop = true;
      else if (args[i] == "-nocase")
        nocase = true;
      else if (args[i] == "-count") {
        ++i;

        if (i < numArgs)
          countVar = args[i];
      }
      else if (args[i] == "-all")
        all = true;
      else if (args[i] == "-overlap")
        overlap = true;
      else if (args[i] == "-strictlimits")
        strictlimits = true;
      else if (args[i] == "-elide")
        elide = true;
      else
        break;
    }

    std::string pattern;

    if (i < numArgs)
      pattern = args[i++];
    else
      return tk_->wrongNumArgs(getName() + " search ?switches? pattern index ?stopIndex?");

    TextInd startInd, endInd;

    if (i < numArgs) {
      if (! stringToTextInd(args[i], startInd))
        return false;

      ++i;
    }
    else
      return tk_->wrongNumArgs(getName() + " search ?switches? pattern index ?stopIndex?");

    if (i < numArgs) {
      if (! stringToTextInd(args[i], endInd))
        return false;

      ++i;
    }

    std::cerr << "search";
    if (! forwards) std::cerr << " -backwards";
    if (exact) std::cerr << " -exact";
    if (regexp) std::cerr << " -regexp";
    if (nolinestop) std::cerr << " -nolinestop";
    if (nocase) std::cerr << " -nocase";
    if (countVar != "") std::cerr << " -count " << countVar;
    if (all) std::cerr << " -all";
    if (overlap) std::cerr << " -overlap";
    if (strictlimits) std::cerr << " -strictlimits";
    if (elide) std::cerr << " -elide";
    std::cerr << " " << pattern;
    std::cerr << " " << startInd;
    std::cerr << " " << endInd;
    std::cerr << "\n";

    setCurrentInd(startInd);

    std::vector<std::string> startInds;
    std::vector<int>         counts;

    while (true) {
      bool rc;

      if (regexp)
        rc = qtext_->find(QRegExp(pattern.c_str()));
      else
        rc = qtext_->find(pattern.c_str());

      if (! rc)
        break;

      auto cursor = qtext_->textCursor();

      int pos1 = cursor.selectionStart();
      int pos2 = cursor.selectionEnd();

      counts.push_back(pos2 - pos1);

      cursor.setPosition(pos1);
      TextInd ind;
      getCurrentInd(cursor, ind);

      startInds.push_back(ind.toString());

      cursor.setPosition(pos2);

      if (! all)
        break;
    }

    if (countVar != "")
      tk_->setIntegerArrayVar(countVar, counts);

    tk_->setStringArrayResult(startInds);
  }
  else if (arg == "see") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " see index");

    tk_->TODO(args);
  }
  else if (arg == "sync") {
    uint i = 1;

    if (i < numArgs && args[i] == "-command")
      ++i;

    if (i != numArgs)
      return tk_->wrongNumArgs(getName() + " sync ?-command command?");

    tk_->TODO(args);
  }
  else if (arg == "tag") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " tag option ?arg ...?");

    auto opt = args[1];

    if      (opt == "add") {
      if (numArgs < 4)
        return tk_->wrongNumArgs(getName() + " tag add tagName index1 ?index2 index1 index2 ...?");

      auto tag = args[2];

      std::vector<TextInd> inds;

      uint i = 3;

      for ( ; i < numArgs; ++i) {
        TextInd ind;
        if (! stringToTextInd(args[i], ind))
          return false;
        inds.push_back(ind);
      }

      if (inds.size() & 1)
        inds.push_back(TextInd::end());

      TextIndRanges indRanges;

      i = 0;

      for ( ; i < inds.size(); i += 2)
        indRanges.push_back(TextIndRange(inds[i], inds[i + 1]));

      addTagRanges(tag, indRanges);
    }
    else if (opt == "bind") {
      tk_->TODO(args);
    }
    else if (opt == "cget") {
      tk_->TODO(args);
    }
    else if (opt == "configure" || opt == "config") {
      if (numArgs < 3)
        return tk_->wrongNumArgs(getName() +
          "tag configure tagName ?-option? ?value? ?-option value ...?");

      auto tag = args[2];

      auto &tagData = this->tagData(tag);

      if (numArgs == 3)
        tk_->TODO(args);
      else {
        uint i = 3;

        for ( ; i < numArgs; ++i) {
          auto arg1 = args[i];

          if      (arg1 == "-background") {
            ++i;
            if (i >= numArgs)
              return false;
            auto value1 = args[i];
            QColor c;
            if (CTkAppUtil::stringToQColor(value1, c))
              tagData.background = c;
          }
          else if (arg1 == "-foreground") {
            ++i;
            if (i >= numArgs)
              return false;
            auto value1 = args[i];
            QColor c;
            if (CTkAppUtil::stringToQColor(value1, c))
              tagData.foreground = c;
          }
          else if (arg1 == "-font") {
            ++i;
            if (i >= numArgs)
              return false;
            auto value1 = args[i];
            tagData.font = tk_->getQFont(value1);
          }
          else if (arg1 == "-borderwidth") {
            tk_->TODO(args);
            ++i;
          }
          else if (arg1 == "-relief") {
            tk_->TODO(args);
            ++i;
          }
          else
            tk_->TODO(args);
        }

        applyTag(tagData);
      }
    }
    else if (opt == "delete") {
      if (numArgs < 3)
        return tk_->wrongNumArgs(getName() + " tag delete tagName ?tagName ...?");

      uint i = 2;

      for ( ; i < numArgs; ++i) {
        auto tag = args[i];

        deleteTag(tag);
      }
    }
    else if (opt == "lower") {
      if (numArgs != 3 && numArgs != 4)
        return tk_->wrongNumArgs(getName() + " tag lower tagName ?aboveThis?");

      auto tag = args[2];

      std::string aboveTag;

      if (numArgs == 4)
        aboveTag = args[3];

      lowerTag(tag, aboveTag);
    }
    else if (opt == "names") {
      std::vector<std::string> names;
      getTagNames(names);

      tk_->setStringArrayResult(names);
    }
    else if (opt == "nextrange") {
      tk_->TODO(args);
    }
    else if (opt == "prevrange") {
      tk_->TODO(args);
    }
    else if (opt == "raise") {
      if (numArgs != 3 && numArgs != 4)
        return tk_->wrongNumArgs(getName() + " tag raise tagName ?aboveThis?");

      auto tag = args[2];

      std::string aboveTag;

      if (numArgs == 4)
        aboveTag = args[3];

      raiseTag(tag, aboveTag);
    }
    else if (opt == "ranges") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getName() + " tag ranges tagName");

      auto tag = args[2];

      TextIndRanges indRanges;

      getTagRanges(tag, indRanges);

      std::vector<std::string> strs;

      for (const auto &indRange : indRanges) {
        auto indRange1 = remapIndRange(indRange);

        strs.push_back(indRange1.toString());
      }

      tk_->setStringArrayResult(strs);
    }
    else if (opt == "remove") {
      if (numArgs < 4)
        return tk_->wrongNumArgs(getName() +
          "tag remove tagName index1 ?index2 index1 index2 ...?");

      auto tag = args[2];

      tagRemove(tag); // remove tag from chars between index pairs
    }
    else
      tk_->throwError("bad tag option \"" + opt + "\": must be "
        "add, bind, cget, configure, delete, lower, names, nextrange, prevrange, "
        "raise, ranges, or remove");
  }
  else if (arg == "window") {
    if (numArgs < 1)
      return tk_->wrongNumArgs(getName() + " window option ?arg ...?");

    auto opt = args[1];

    if      (opt == "cget") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getName() + " window cget index option");

      tk_->TODO(args);
    }
    else if (opt == "configure" || opt == "config") {
      if (numArgs < 3)
        return tk_->wrongNumArgs(getName() + " window configure index ?-option value ...?");

      tk_->TODO(args);
    }
    else if (opt == "create") {
      if (numArgs < 3)
        return tk_->wrongNumArgs(getName() + " window create index ?-option value ...?");

      TextInd ind;
      if (! stringToTextInd(args[2], ind))
        return false;

      uint i = 3;

      for ( ; i < numArgs; ++i) {
        if      (args[i] == "-align") {
          tk_->TODO(args);
          ++i;
        }
        else if (args[i] == "-create") {
          tk_->TODO(args);
          ++i;
        }
        else if (args[i] == "-padx") {
          tk_->TODO(args);
          ++i;
        }
        else if (args[i] == "-pady") {
          tk_->TODO(args);
          ++i;
        }
        else if (args[i] == "-stretch") {
          tk_->TODO(args);
          ++i;
        }
        else if (args[i] == "-window") {
          if (i + 1 >= numArgs)
            return false;

          auto window = args[++i];

          auto *w = tk_->lookupWidgetByName(window);
          if (! w) return false;

          qtext_->setCurrentInd(ind.lineNum, ind.charNum);

          qtext_->insertWidget(w->getQWidget());
        }
        else
          return false;
      }
    }
    else if (opt == "names") {
      if (numArgs != 2)
        return tk_->wrongNumArgs(getName() + " window names");

      tk_->TODO(args);
    }
    else
      return false;
  }
  else if (arg == "xview") {
    int step  = qtext_->horizontalScrollBar()->pageStep();
    int min   = qtext_->horizontalScrollBar()->minimum();
    int max   = qtext_->horizontalScrollBar()->maximum() - step;
    int value = qtext_->horizontalScrollBar()->value();

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
          return tk_->wrongNumArgs(getName() + " xview moveto number");

        double x;

        if (! tk_->getOptionReal(opt, args[2], x))
          return false;

        qtext_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
      else if (opt == "scroll") {
        if (numArgs < 4)
          return tk_->wrongNumArgs(getName() + " xview scroll number pages|units");

        double x;

        if (! tk_->getOptionReal(opt, args[2], x))
          return false;

        if      (args[3] == "pages")
          value += x*1000*step;
        else if (args[3] == "units")
          value += x*1000;
        else
          return tk_->throwError("bad argument '" + args[3] + "' ust be pages or units");

        qtext_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
      else {
        double x;

        if (! CTkAppUtil::stringToReal(opt, x))
          return tk_->throwError("Invalid xview value '" + opt + "'");

        qtext_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
    }
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
          return tk_->wrongNumArgs(getName() + " yview moveto number");

        double y;

        if (! tk_->getOptionReal(opt, args[2], y))
          return false;

        qtext_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
      else if (opt == "scroll") {
        if (numArgs < 4)
          return tk_->wrongNumArgs(getName() + " yview scroll number pages|units");

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
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppText::
setCurrentInd(const TextInd &ind, const QTextCursor::MoveMode &moveMode) const
{
  qtext_->setCurrentInd(ind.lineNum, ind.charNum, moveMode);
}

void
CTkAppText::
setCurrentInd(QTextCursor &cursor, const TextInd &ind,
              const QTextCursor::MoveMode &moveMode) const
{
  qtext_->setCurrentInd(cursor, ind.lineNum, ind.charNum, moveMode);
}

void
CTkAppText::
getCurrentInd(TextInd &ind) const
{
  auto cursor = qtext_->textCursor();
  getCurrentInd(cursor, ind);
}

void
CTkAppText::
getCurrentInd(QTextCursor &cursor, TextInd &ind) const
{
  ind.lineNum = cursor.blockNumber() + 1;
  int pos = cursor.position();
  cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
  ind.charNum = pos - cursor.position();
}

bool
CTkAppText::
stringToTextInd(const std::string &str, TextInd &ind) const
{
  auto errorMsg = [&]() {
    std::cerr << "Invalid TextInd '" << str << "'\n";
    return false;
  };

  ind = TextInd::end();

  CStrParse parse(str);

  parse.skipSpace();

  if      (parse.isString("end")) {
    parse.skipLastString();
  }
  else if (parse.isAlpha()) {
    std::string mark;

    while (! parse.eof() && parse.isAlnum())
      mark += parse.readChar();

    if (! getMark(mark, ind))
      return errorMsg();
  }
  else if (parse.isDigit()) {
    int lineNum = ind.lineNum;
    int charNum = ind.charNum;

    if (! parse.readInteger(&lineNum))
      return errorMsg();

    if (parse.isChar('.')) {
      parse.skipChar();

      if (! parse.readInteger(&charNum))
        return errorMsg();
    }

    ind = TextInd(lineNum, charNum);
  }

  if (parse.isChar('+') || parse.isChar('-')) {
    auto sign = (parse.isChar('+') ? 1 : -1);

    parse.skipChar();
    parse.skipSpace();

    if (parse.isDigit()) {
      long n;
      if (! parse.readInteger(&n))
        return errorMsg();
      n *= sign;

      parse.skipSpace();

      if      (parse.isString("chars")) {
        parse.skipLastString();

        auto cursor = qtext_->textCursor();
        setCurrentInd(cursor, ind);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, n);
        getCurrentInd(cursor, ind);
      }
      else if (parse.isString("lines")) {
        parse.skipLastString();

        auto cursor = qtext_->textCursor();
        setCurrentInd(cursor, ind);
        cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, n);
        getCurrentInd(cursor, ind);
      }
      else
        return errorMsg();
    }
    else
      return errorMsg();
  }

  parse.skipSpace();

  if      (parse.isString("linestart")) {
    parse.skipLastString();
    tk_->TODO("linestart");
  }
  else if (parse.isString("lineend")) {
    parse.skipLastString();
    tk_->TODO("lineend");
  }
  else if (parse.isString("wordstart")) {
    parse.skipLastString();
    tk_->TODO("wordstart");
  }
  else if (parse.isString("wordend")) {
    parse.skipLastString();
    tk_->TODO("wordend");
  }

  parse.skipSpace();

  if (! parse.eof())
    return errorMsg();

  return true;
}

void
CTkAppText::
setMark(const std::string &mark, const TextInd &ind)
{
  marks_[mark] = ind;
}

bool
CTkAppText::
getMark(const std::string &mark, TextInd &ind) const
{
  auto pm = marks_.find(mark);
  if (pm == marks_.end()) return false;

  ind = (*pm).second;

  return true;
}

void
CTkAppText::
getMarkNames(std::vector<std::string> &names) const
{
  for (const auto &pm : marks_)
    names.push_back(pm.first);
}

CTkAppText::TextIndRange
CTkAppText::
remapIndRange(const TextIndRange &ind) const
{
  TextIndRange ind1;
  ind1.start = remapInd(ind.start);
  ind1.end   = remapInd(ind.end);
  return ind1;
}

CTkAppText::TextInd
CTkAppText::
remapInd(const TextInd &ind) const
{
  TextInd ind1 = ind;

  auto cursor = qtext_->textCursor();

  if (ind.lineNum == CTkAppText::TextInd::END) {
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    ind1.lineNum = cursor.blockNumber() + 1;
    int pos1 = cursor.position();
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
    int pos2 = cursor.position();
    ind1.charNum = pos1 - pos2;
  }

  if (ind.charNum == CTkAppText::TextInd::END) {
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, ind1.lineNum - 1);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
    int pos1 = cursor.position();
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
    int pos2 = cursor.position();
    ind1.charNum = pos2 - pos1;
  }

  return ind1;
}

void
CTkAppText::
setText(const std::string &text)
{
  CTkAppWidget::setText(text);

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

void
CTkAppText::
raiseTag(const std::string & /*tag*/, const std::string & /*aboveTag*/)
{
  // TODO
}

void
CTkAppText::
lowerTag(const std::string & /*tag*/, const std::string & /*aboveTag*/)
{
  // TODO
}

void
CTkAppText::
applyTag(const TagData &tagData)
{
  for (const auto &indRange : tagData.indRanges) {
    setCurrentInd(indRange.start);
    setCurrentInd(indRange.end, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;

    fmt.setForeground(tagData.foreground);
    fmt.setBackground(tagData.background);
    fmt.setFont      (tagData.font);

    auto cursor = qtext_->textCursor();
    cursor.setCharFormat(fmt);
  }
}

void
CTkAppText::
deleteTag(const std::string &tag)
{
  auto pt = tags_.find(tag);

  if (pt != tags_.end())
    tags_.erase(pt);
}

void
CTkAppText::
addTagRanges(const std::string &tag, const std::vector<TextIndRange> &indRanges)
{
  auto &tagData = this->tagData(tag);

  for (const auto &indRange : indRanges)
    tagData.indRanges.push_back(indRange);
}

void
CTkAppText::
getTagRanges(const std::string &tag, TextIndRanges &indRanges) const
{
  auto &tagData = const_cast<CTkAppText *>(this)->tagData(tag);

  indRanges = tagData.indRanges;
}

void
CTkAppText::
tagRemove(const std::string & /*tag*/)
{
  // TODO
}

void
CTkAppText::
getTagNames(std::vector<std::string> &names) const
{
  for (const auto &pt : tags_)
    names.push_back(pt.first);
}

CTkAppText::TagData &
CTkAppText::
tagData(const std::string &name)
{
  auto pt = tags_.find(name);

  if (pt == tags_.end()) {
    TagData tagData;

    tagData.background = qtext_->palette().window().color();
    tagData.foreground = qtext_->palette().windowText().color();
    tagData.font       = qtext_->font();

    pt = tags_.insert(pt, Tags::value_type(name, tagData));
  }

  return (*pt).second;
}

//---

CTkAppTextWidget::
CTkAppTextWidget(CTkAppText *text) :
 CQTextWidget(text->getParent() ? text->getParent()->getQWidget() : nullptr), text_(text)
{
  setObjectName("qtext");

  setFocusPolicy(Qt::StrongFocus);
}

QSize
CTkAppTextWidget::
sizeHint() const
{
  auto s = CQTextWidget::sizeHint();

  QFontMetrics fm(font());

  if (width_ > 0)
    s.setWidth(width_*fm.horizontalAdvance("0"));

  if (height_ > 0)
    s.setHeight(height_*fm.height());

  return s;
}

//----------

CTkAppTopLevel::
CTkAppTopLevel(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qframe_ = new QFrame(nullptr);

  qframe_->setWindowTitle(QString::fromStdString(name));

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

CTkAppTreeView::
CTkAppTreeView(CTkApp *tk, CTkAppWidget *parent, const std::string &name) :
 CTkAppWidget(tk, parent, name)
{
  qtree_ = new CTkAppTreeViewWidget(this);

  qtree_->setColumnCount(1);

  setQWidget(qtree_);

  connectSlots(true);
}

void
CTkAppTreeView::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qtree_,
    SIGNAL(itemSelectionChanged()), this, SLOT(selectionSlot()));
}

bool
CTkAppTreeView::
execConfig(const std::string &name, const std::string &value)
{
  if      (name == "-columns") {
    std::vector<std::string> strs;
    if (! tk_->splitList(value, strs))
      return tk_->throwError("Invalid columns \"" + value + "\"");

    qtree_->setColumnCount(strs.size() + 1);
  }
  else if (name == "-displaycolumns") {
    // TODO: reorder columns
    tk_->TODO(name);
  }
  else if (name == "-height") {
    long h;
    if (! CTkAppUtil::stringToInt(value, h))
      return tk_->throwError("Invalid height \"" + value + "\"");
    qtree_->setHeight(h);
  }
  else if (name == "-selectmode") {
    if      (value == "single") {
      qtree_->setSelectionMode(QAbstractItemView::SingleSelection);
    }
    else if (value == "browse") {
      qtree_->setSelectionMode(QAbstractItemView::SingleSelection); // TODO
    }
    else if (value == "multiple") {
      qtree_->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    else if (value == "extended") {
      qtree_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
    else
      return false;
  }
  else if (name == "-show") {
    if      (value == "tree") {
      tk_->TODO(name);
    }
    else if (value == "headings") {
      tk_->TODO(name);
    }
    else
      return false;
  }
  else
    return CTkAppWidget::execConfig(name, value);

  return true;
}

bool
CTkAppTreeView::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  const auto &arg = args[0];

  //---

  auto stringToColumn = [&](const std::string &arg, int &c) {
    if (arg == "#0") {
      c = 0;
      return true;
    }

    long l;
    if (! CTkAppUtil::stringToInt(arg, l))
      return false;
    c = int(l + 1);

    if (c < 0 || c >= qtree_->columnCount())
      return false;

    return true;
  };

  auto stringToRootIndex = [&](const std::string &arg, Index &ind) {
    if (arg == "") {
      ind.setRoot(true);
      return true;
    }

    if (getIdIndex(arg, ind))
      return true;

    CStrParse parse(arg);

    if (parse.isString("IND")) {
      parse.skipLastString();

      auto parent = Index::root();

      while (true) {
        long r;
        if (! parse.readInteger(&r))
          return false;

        if (! parse.isChar('.'))
          return false;

        parse.skipChar();

        long c;
        if (! parse.readInteger(&c))
          return false;

        ind = Index();

        ind.setRow(r);
        ind.setCol(c);

        if (! parent.isRoot())
          ind.setParent(parent);

        parent = ind;

        if (parse.eof())
          break;

        if (! parse.isChar('|'))
          return false;

        parse.skipChar();
      }
    }
    else {
      long l;
      if (! parse.readInteger(&l))
        return false;

      ind.setRow(l);
      ind.setCol(0);
    }

    if (! parse.eof())
      return false;

    return true;
  };

  auto stringToChildIndex = [&](const std::string &arg, Index &parentInd,
                                Index &ind, bool quiet=false) {
    if (arg == "")
      return (quiet ? true : false);

    long l;
    if (! CTkAppUtil::stringToInt(arg, l))
      return (quiet ? true : false);

    ind.setRow(l);
    ind.setCol(0);

    if (! parentInd.isRoot())
      ind.setParent(parentInd);

    return true;
  };

  auto getIndexItem = [&](const Index &ind, bool create) {
    auto ind1 = ind;

    std::deque<Index> inds;

    while (ind1.hasParent()) {
      inds.push_front(ind1);

      ind1 = ind1.parent();
    }

    assert(ind1.row() >= 0);
    auto *item = qtree_->topLevelItem(ind1.row());

    if (! item) {
      if (! create)
        return static_cast<QTreeWidgetItem *>(nullptr);

      item = new QTreeWidgetItem(QStringList() << "");

      qtree_->addTopLevelItem(item);
    }

    for (auto &ind2 : inds) {
      assert(ind2.row() >= 0);
      auto *child = item->child(ind2.row());

      if (! child) {
        if (! create)
          return static_cast<QTreeWidgetItem *>(nullptr);

        child = new QTreeWidgetItem(QStringList() << "");

        item->addChild(child);
      }

      item = child;
    }

    return item;
  };

  auto indexToString = [&](const Index &ind) {
    std::string str;

    if (getIndexId(ind, str))
      return str;

    str = "IND";

    auto ind1 = ind;

    std::deque<Index> inds;

    while (ind1.hasParent()) {
      inds.push_front(ind1);

      ind1 = ind1.parent();
    }

    str += std::to_string(ind1.row()) + "." + std::to_string(ind1.col());

    for (auto &ind2 : inds)
      str += "|" + std::to_string(ind2.row()) + "." + std::to_string(ind2.col());

    return str;
  };

  auto itemToIndex = [&](QTreeWidgetItem  *item) {
    std::deque<int> inds;

    auto *parent = item->parent();

    while (parent) {
      auto ind = parent->indexOfChild(item);

      item   = parent;
      parent = item->parent();

      inds.push_front(ind);
    }

    auto ind = qtree_->indexOfTopLevelItem(item);

    inds.push_front(ind);

    auto parentInd = Index::root();

    auto itemInd = parentInd;

    for (auto &ind : inds) {
      Index ind1;

      ind1.setRow(ind);
      ind1.setCol(0);

      if (! parentInd.isRoot())
        ind1.setParent(parentInd);

      itemInd   = ind1;
      parentInd = ind1;
    }

    return itemInd;
  };

  auto itemToString = [&](QTreeWidgetItem  *item) {
    return indexToString(itemToIndex(item));
  };

  //---

  if      (arg == "bbox") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs(getName() + " bbox itemid ?column");

    Index ind;
    if (! stringToRootIndex(args[1], ind))
      return false;

    if (numArgs == 3) {
      int c;
      if (! stringToColumn(args[2], c))
        return false;
      ind.setCol(c);
    }

    auto *item = getIndexItem(ind, /*create*/false);
    if (! item) return false;

    auto r = qtree_->visualItemRect(item);

    tk_->setIntegerArrayResult({{r.x(), r.y(), r.width(), r.height()}});
  }
  else if (arg == "children") {
   if (numArgs != 2 && numArgs != 3)
     return tk_->wrongNumArgs(getName() + " children item ?newchildren?");

    tk_->TODO(args);
  }
  else if (arg == "column") {
    tk_->TODO(args);
  }
  else if (arg == "delete") {
    tk_->TODO(args);
  }
  else if (arg == "detach") {
    tk_->TODO(args);
  }
  else if (arg == "drag") {
    tk_->TODO(args);
  }
  else if (arg == "drop") {
    tk_->TODO(args);
  }
  else if (arg == "exists") {
    tk_->TODO(args);
  }
  else if (arg == "focus") {
    tk_->TODO(args);
  }
  else if (arg == "heading") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " heading column -option value...");

    int c;
    if (! stringToColumn(args[1], c))
      return false;

    uint i = 2;

    for ( ; i < numArgs; ++i) {
      if (args[i] == "-text") {
        auto text = args[++i];

        auto *item = qtree_->headerItem();

        item->setText(c, QString::fromStdString(text));
      }
      else
        return false;
    }
  }
  else if (arg == "identify") {
    tk_->TODO(args);
  }
  else if (arg == "index") {
    tk_->TODO(args);
  }
  else if (arg == "instate") {
    tk_->TODO(args);
  }
  else if (arg == "insert") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getName() + " insert parent index ?-id id? -options...");

    Index parentInd;
    if (! stringToRootIndex(args[1], parentInd))
      return false;

    Index ind;
    if (! stringToChildIndex(args[2], parentInd, ind))
      return false;

    auto *item = getIndexItem(ind, /*create*/true);

    uint i = 3;

    for ( ; i < numArgs; ++i) {
      if      (args[i] == "-id") {
        auto id = args[++i];

        setIdIndex(id, ind);
      }
      else if (args[i] == "-text") {
        auto text = args[++i];

        item->setText(ind.col(), QString::fromStdString(text));
      }
      else if (args[i] == "-values") {
        auto value = args[++i];

        std::vector<std::string> strs;
        if (! tk_->splitList(value, strs))
          return tk_->throwError("Invalid values \"" + value + "\"");

        int col = 1;

        for (auto &str : strs)
          item->setText(col++, QString::fromStdString(str));
      }
      else
        return false;
    }

    tk_->setStringResult(indexToString(ind));
  }
  else if (arg == "item") {
    tk_->TODO(args);
  }
  else if (arg == "move") {
    tk_->TODO(args);
  }
  else if (arg == "next") {
    tk_->TODO(args);
  }
  else if (arg == "parent") {
    tk_->TODO(args);
  }
  else if (arg == "prev") {
    tk_->TODO(args);
  }
  else if (arg == "see") {
    tk_->TODO(args);
  }
  else if (arg == "selection") {
    if (numArgs == 1) {
      auto items = qtree_->selectedItems();

      std::vector<std::string> strs;

      for (auto *item : items) {
        strs.push_back(itemToString(item));
      }

      tk_->setStringArrayResult(strs);
    }
    else {
      auto arg = args[1];

      if      (arg == "add") {
        tk_->TODO(args);
      }
      else if (arg == "remove") {
        tk_->TODO(args);
      }
      else if (arg == "set") {
        tk_->TODO(args);
      }
      else if (arg == "toggle") {
        tk_->TODO(args);
      }
      else {
        return tk_->wrongNumArgs(getName() + " selection ?add|remove|set|toggle items?");
      }
    }
  }
  else if (arg == "set") {
    tk_->TODO(args);
  }
  else if (arg == "state") {
    tk_->TODO(args);
  }
  else if (arg == "tag") {
    tk_->TODO(args);
  }
  else if (arg == "xview") {
    int step  = qtree_->horizontalScrollBar()->pageStep();
    int min   = qtree_->horizontalScrollBar()->minimum();
    int max   = qtree_->horizontalScrollBar()->maximum() - step;
    int value = qtree_->horizontalScrollBar()->value();

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
          return tk_->wrongNumArgs(getName() + " xview moveto number");

        double x;

        if (! tk_->getOptionReal(opt, args[2], x))
          return false;

        qtree_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
      else if (opt == "scroll") {
        if (numArgs < 4)
          return tk_->wrongNumArgs(getName() + " xview scroll number pages|units");

        double x;

        if (! tk_->getOptionReal(opt, args[2], x))
          return false;

        if      (args[3] == "pages")
          value += x*1000*step;
        else if (args[3] == "units")
          value += x*1000;
        else
          return tk_->throwError("bad argument '" + args[3] + "' ust be pages or units");

        qtree_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
      else {
        double x;

        if (! CTkAppUtil::stringToReal(opt, x))
          return tk_->throwError("Invalid xview value '" + opt + "'");

        qtree_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
    }
  }
  else if (arg == "yview") {
    int step  = qtree_->verticalScrollBar()->pageStep();
    int min   = qtree_->verticalScrollBar()->minimum();
    int max   = qtree_->verticalScrollBar()->maximum() - step;
    int value = qtree_->verticalScrollBar()->value();

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
          return tk_->wrongNumArgs(getName() + " yview moveto number");

        double y;

        if (! tk_->getOptionReal(opt, args[2], y))
          return false;

        qtree_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
      else if (opt == "scroll") {
        if (numArgs < 4)
          return tk_->wrongNumArgs(getName() + " yview scroll number pages|units");

        double y;

        if (! tk_->getOptionReal(opt, args[2], y))
          return false;

        if      (args[3] == "pages")
          value += y*1000*step;
        else if (args[3] == "units")
          value += y*1000;
        else
          return tk_->throwError("bad argument '" + args[3] + "' ust be pages or units");

        qtree_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
      else {
        double y;

        if (! CTkAppUtil::stringToReal(opt, y))
          return tk_->throwError("Invalid yview value '" + opt + "'");

        qtree_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
    }
  }
  else
    return CTkAppWidget::execOp(args);

  return true;
}

void
CTkAppTreeView::
selectionSlot()
{
  CTkAppEventData eventData;

  eventData.type  = CTkAppEventType::Virtual;
  eventData.vtype = CTkAppVirtualEventType::TreeviewSelect;

  triggerVirtualEvents(eventData);
}

void
CTkAppTreeView::
setIdIndex(const std::string &id, const Index &ind)
{
  ids_[id] = ind;
}

bool
CTkAppTreeView::
getIdIndex(const std::string &id, Index &ind) const
{
  auto p = ids_.find(id);
  if (p == ids_.end()) return false;

  ind = (*p).second;

  return true;
}

bool
CTkAppTreeView::
getIndexId(const Index &ind, std::string &id) const
{
  for (const auto &pi : ids_) {
    if (pi.second == ind) {
      id = pi.first;
      return true;
    }
  }

  return false;
}

//---

CTkAppTreeViewWidget::
CTkAppTreeViewWidget(CTkAppTreeView *treeview) :
 QTreeWidget(treeview->getParent() ? treeview->getParent()->getQWidget() : nullptr),
 treeview_(treeview)
{
  setObjectName("qtreeview");
}

void
CTkAppTreeViewWidget::
setHeight(const OptReal &h)
{
  height_ = h;
}

QSize
CTkAppTreeViewWidget::
sizeHint() const
{
  auto s = QTreeWidget::sizeHint();

  QFontMetrics fm(font());

  if (height_)
    s.setHeight((*height_)*fm.height());

  return s;
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
  deleted_ = true;

  if (parent_)
    parent_->removeChild(this);

  tk_->removeWidget(this);

  for (auto &pc : children_)
    delete pc.second;

  delete qwidget_;

  delete getWidgetCommand();

  delete eventFilter_;
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

  eventFilter_ = new CTkAppWidgetEventFilter(this);

  qwidget_->installEventFilter(eventFilter_);

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
  for (const auto &pc : children_)
    children.push_back(pc.second);
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

CTkAppLayout *
CTkAppWidget::
getTkLayout()
{
  auto *pack  = getTkPackLayout (/*create*/false); if (pack ) return pack;
  auto *grid  = getTkGridLayout (/*create*/false); if (grid ) return grid;
  auto *place = getTkPlaceLayout(/*create*/false); if (place) return place;

  return nullptr;
}

CTkAppPackLayout *
CTkAppWidget::
getTkPackLayout(bool create)
{
  auto *w = getQWidget();
  assert(w);

  auto *l = w->layout();

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

  auto *l = w->layout();

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

  auto *l = w->layout();

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
    QColor c;
    if (CTkAppUtil::stringToQColor(value, c)) {
      auto pal = qwidget_->palette();
      pal.setColor(QPalette::Active, qwidget()->backgroundRole(), c);
      qwidget_->setPalette(pal);
    }
  }
  else if (name == "-activeforeground") {
    QColor c;
    if (CTkAppUtil::stringToQColor(value, c)) {
      auto pal = qwidget_->palette();
      pal.setColor(QPalette::Active, qwidget()->foregroundRole(), c);
      qwidget_->setPalette(pal);
    }
  }
  else if (name == "-anchor") {
    setWidgetAnchor(value);
  }
  else if (name == "-background" || name == "-bg") {
    QColor c;
    if (CTkAppUtil::stringToQColor(value, c))
      setBackground(c);

    qwidget_->update();
  }
  else if (name == "-bitmap") {
    auto image = tk_->getBitmap(value);

    if (! image)
      return tk_->throwError("Failed to load bitmap '" + value + "'");

    setImage(image);
  }
  else if (name == "-borderwidth" || name == "-bd") {
    long w;
    if (! tk_->getOptionInt(name, value, w))
      return false;
    setBorderWidth(w);
  }
  else if (name == "-compound") {
    CTkAppCompoundType compoundType;
    if (! CTkAppUtil::stringToCompound(value, compoundType))
      return false;
    compoundType_ = compoundType;
  }
  else if (name == "-cursor") {
    //auto c = CTkAppUtil::stringToCursor(value);
    //qwidget_->setCursor(c);
    cursor_ = value;
  }
  else if (name == "-disabledbackground") {
    QColor c;
    if (CTkAppUtil::stringToQColor(value, c)) {
      auto pal = qwidget_->palette();
      pal.setColor(QPalette::Disabled, qwidget()->backgroundRole(), c);
      qwidget_->setPalette(pal);
    }
  }
  else if (name == "-disabledforeground") {
    QColor c;
    if (CTkAppUtil::stringToQColor(value, c)) {
      auto pal = qwidget_->palette();
      pal.setColor(QPalette::Disabled, qwidget()->foregroundRole(), c);
      qwidget_->setPalette(pal);
    }
  }
  else if (name == "-foreground" || name == "-fg") {
    QColor c;
    if (CTkAppUtil::stringToQColor(value, c))
      setForeground(c);

    qwidget_->update();
  }
  else if (name == "-highlightbackground") {
    QColor c;
    if (CTkAppUtil::stringToQColor(value, c)) {
      auto pal = qwidget_->palette();
      pal.setColor(QPalette::Active, qwidget()->backgroundRole(), c);
      qwidget_->setPalette(pal);
    }
  }
  else if (name == "-highlightcolor") {
    QColor c;
    if (CTkAppUtil::stringToQColor(value, c)) {
      auto pal = qwidget_->palette();
      pal.setColor(QPalette::Active, qwidget()->foregroundRole(), c);
      qwidget_->setPalette(pal);
    }
  }
  else if (name == "-highlightthickness") {
    double thickness;
    if (! CTkAppUtil::stringToDistance(value, thickness))
      return false;
    setHighlightThickness(thickness);
  }
  else if (name == "-image") {
    auto image = tk_->getImage(value);

    if (! image)
      return tk_->throwError("Failed to get image '" + value + "'");

    setImage(image);
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
    if (! CTkAppUtil::stringToRelief(value, relief_))
      return false;

    CTkAppUtil::setFrameRelief(qwidget_, relief_);

    appearanceChanged();
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
  else if (name == "-text") {
    setText(value);
  }
  else if (name == "-selectbackground") {
    QColor c;
    if (CTkAppUtil::stringToQColor(value, c))
      selectBackground_ = c;
  }
  else if (name == "-selectborderwidth") {
    int w;
    if (! CTkAppUtil::stringToDistanceI(value, w))
      return tk_->throwError("Invalid width \"" + value + "\"");
    setSelectBorderWidth(w);
  }
  else if (name == "-selectforeground") {
    QColor c;
    if (CTkAppUtil::stringToQColor(value, c))
      selectForeground_ = c;
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
    auto f = tk_->getQFont(value);

    qwidget_->setFont(f);

    qwidget_->update();
  }
  else if (name == "-colormap") {
    tk_->TODO(name);
  }
  else if (name == "-setgrid") {
    bool b;
    if (! CTkAppUtil::stringToBool(value, b))
      return tk_->throwError("Invalid " + name + " \"" + value + "\"");
    gridSize_ = b;
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
setBackground(const QColor &c)
{
  auto pal = qwidget_->palette();
  pal.setColor(QPalette::Normal, qwidget()->backgroundRole(), c);
  pal.setColor(QPalette::Inactive, qwidget()->backgroundRole(), c);
  qwidget_->setPalette(pal);
}

void
CTkAppWidget::
setForeground(const QColor &c)
{
  auto pal = qwidget_->palette();
  pal.setColor(QPalette::Normal, qwidget()->foregroundRole(), c);
  pal.setColor(QPalette::Inactive, qwidget()->backgroundRole(), c);
  qwidget_->setPalette(pal);
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

  // <width>x<height>
  if (! parse.isChar('-') && ! parse.isChar('+')) {
    if (parse.readInteger(&w)) {
      if (! parse.isChar('x'))
        return throwError();

      parse.skipChar();

      if (! parse.readInteger(&h))
        return throwError();

      parse.skipSpace();
    }
  }

  // +-<x>+-<y>
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
  Qt::Alignment align;
  if (CTkAppUtil::stringToAlign(value, align))
    setAnchor(align);
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

void
CTkAppWidget::
setSelectBorderWidth(int)
{
}

//---

std::vector<std::string>
CTkAppWidget::
bindtags() const
{
  auto bindtags = bindtags_;

  if (bindtags.empty()) {
    bindtags.push_back(getName());
    bindtags.push_back(getClassName());

    if (! isTopLevel()) {
      if (toplevel())
        bindtags.push_back(toplevel()->getName());
      else
        bindtags.push_back(".");
    }

    bindtags.push_back("all");
  }

  return bindtags;
}

bool
CTkAppWidget::
bindEvent(const CTkAppEventData &data)
{
  eventDatas_.push_back(data);

  return true;
}

void
CTkAppWidget::
getBindings(const CTkAppEventData &data, std::vector<std::string> &bindings)
{
  for (const auto &edata : eventDatas_)
    if (edata == data)
      bindings.push_back(edata.pattern);
}

void
CTkAppWidget::
getBindings(std::vector<std::string> &bindings)
{
  for (const auto &edata : eventDatas_)
    bindings.push_back(edata.pattern);
}

//---

bool
CTkAppWidget::
triggerMousePressEvents(QEvent *e, int button)
{
  auto *me = static_cast<QMouseEvent *>(e);

  CTkAppEventData matchEventData;

  tk_->encodeEvent(me, CTkAppEventMode::Press, button, matchEventData);

  return triggerEvents(e, matchEventData);
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

  return triggerEvents(e, matchEventData);
}

bool
CTkAppWidget::
triggerMouseReleaseEvents(QEvent *e, int button)
{
  auto *me = static_cast<QMouseEvent *>(e);

  CTkAppEventData matchEventData;

  tk_->encodeEvent(me, CTkAppEventMode::Release, button, matchEventData);

  return triggerEvents(e, matchEventData);
}

bool
CTkAppWidget::
triggerKeyPressEvents(QEvent *e)
{
  auto *ke = dynamic_cast<QKeyEvent *>(e);

  CTkAppEventData matchEventData;

  tk_->encodeEvent(ke, /*press*/true, matchEventData);

  return triggerEvents(e, matchEventData);
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
triggerVirtualEvents(const CTkAppEventData &matchEventData)
{
  processVirtualEvents(matchEventData);

  // class and global events
  return tk_->triggerVirtualEvents(getClassName(), this, matchEventData);
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

void
CTkAppWidget::
processVirtualEvents(const CTkAppEventData &matchEventData)
{
  for (const auto &eventData : eventDatas_) {
    if (eventData == matchEventData) {
      tk_->execVirtualEvent(this, matchEventData, eventData.command);
    }
  }
}

//---

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
  setObjectName("qmenu");

  connectSlots(true);
}

void
CTkAppSubMenu::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, this,
    SIGNAL(aboutToShow()), this, SLOT(showSlot()));
  CQUtil::connectDisconnect(b, this,
    SIGNAL(aboutToHide()), this, SLOT(hideSlot()));
}

void
CTkAppSubMenu::
showSlot()
{
  //std::cerr << "Show " << menu_ << "\n";

  auto *w = tk_->lookupWidgetByName(menu_);
  auto *menu = dynamic_cast<CTkAppMenu *>(w);
  if (! menu) { std::cerr << "No menu '" << menu_ << "'\n"; return; }

  auto actions = menu->qmenu()->actions();

  for (auto *action : actions) {
    //std::cerr << "Add Action " << action->text().toStdString() << "\n";

    //menu->qmenu()->removeAction(action);
    //action->setParent(this);

    this->addAction(action);
  }
}

void
CTkAppSubMenu::
hideSlot()
{
  //std::cerr << "Hide " << menu_ << "\n";

  auto *w    = tk_->lookupWidgetByName(menu_);
  auto *menu = dynamic_cast<CTkAppMenu *>(w);
  if (! menu) { std::cerr << "No menu '" << menu_ << "'\n"; return; }

  auto actions = this->actions();

  for (auto *action : actions) {
    //std::cerr << "Remove Action " << action->text().toStdString() << "\n";

    //action->setParent(menu->qmenu());

    menu->qmenu()->addAction(action);
  }
}
