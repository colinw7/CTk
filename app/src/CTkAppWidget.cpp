#include <CTkAppWidget.h>
#include <CTkAppRoot.h>
#include <CTkAppTopLevel.h>
#include <CTkAppCommands.h>
#include <CTkApp.h>
#include <CTkAppPackLayout.h>
#include <CTkAppPlaceLayout.h>
#include <CTkAppGridLayout.h>
#include <CTkAppImage.h>
#include <CTkAppUtil.h>
#include <CTkAppX11.h>

#include <CQUtil.h>

#include <CQStrParse.h>

#include <QApplication>
#include <QMouseEvent>

#include <deque>

CTkAppWidget::
CTkAppWidget(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 tk_(tk), parent_(parent), name_(name)
{
  if (getenv("CTK_APP_DEBUG_WIDGET"))
    std::cerr << "Create widget " << name.toStdString() << "\n";

  if      (parent_) {
    setParent(parent->getQWidget());

    parent_->addChild(this);
  }
  else if (tk->root()) {
    setParent(tk->root()->getQWidget());
  }

  int n = tk_->addWidget(this);

  setObjectName(QString("widget%1").arg(n));

  connect(this, SIGNAL(destroyed(QObject *)), this, SLOT(destroySlot(QObject *)));
}

CTkAppWidget::
~CTkAppWidget()
{
  if (getenv("CTK_APP_DEBUG_WIDGET"))
    std::cerr << "Delete widget " << name_.toStdString() << "\n";

  if (deleteWindowCmd() != "")
    tk_->eval(deleteWindowCmd());

  //---

  deleted_ = true;

  if (parent_)
    parent_->removeChild(this);

  tk_->removeWidget(this);

  WidgetMap children;

  std::swap(children, children_);

  for (auto &pc : children)
    delete pc.second;

  delete qwidget_;

  delete getWidgetCommand();

  delete eventFilter_;

  if (imageRef_)
    imageRef_->removeRef(getName());
}

void
CTkAppWidget::
setParentWidget(QWidget *w)
{
  assert(! isTopLevel());

  getQWidget()->setParent(w);

  getQWidget()->setVisible(true);
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

QString
CTkAppWidget::
getFullName() const
{
  auto name = getName();

  if (parent_ != nullptr) {
    auto pname = parent_->getFullName();

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
    qwidget_->setObjectName(name_);
  else
    qwidget_->setObjectName(".");

  eventFilter_ = new CTkAppWidgetEventFilter(this);

  qwidget_->installEventFilter(eventFilter_);

  qwidget_->setFocusPolicy(Qt::ClickFocus);

  qwidget_->setFont(tk_->appFont());

  qwidget_->setVisible(false);
}

QWidget *
CTkAppWidget::
getQWidget() const
{
  return qwidget_;
}

//---

void
CTkAppWidget::
destroySlot(QObject *)
{
  if (! deleted_) {
    CTkAppEventData matchEventData;

    matchEventData.type = CTkAppEventType::Destroy;

    QCloseEvent event;

    triggerEvents(&event, matchEventData);
  }
}

//---

const CTkAppOptData &
CTkAppWidget::
getOpts() const
{
  return getWidgetCommand()->getOpts();
}

//---

void
CTkAppWidget::
setBackground(const QColor &c)
{
  background_ = c;

  updatePalette();
}

void
CTkAppWidget::
setForeground(const QColor &c)
{
  foreground_ = c;

  updatePalette();
}

// inside background
void
CTkAppWidget::
setActiveBackground(const QColor &c)
{
  activeBackground_ = c;

  updatePalette();
}

// inside foreground
void
CTkAppWidget::
setActiveForeground(const QColor &c)
{
  activeForeground_ = c;

  updatePalette();
}

void
CTkAppWidget::
setDisabledBackground(const QColor &c)
{
  disabledBackground_ = c;

  updatePalette();
}

void
CTkAppWidget::
setDisabledForeground(const QColor &c)
{
  disabledForeground_ = c;

  updatePalette();
}

void
CTkAppWidget::
setHighlightBackground(const QColor &c)
{
  highlightBackground_ = c;

  updatePalette();
}

void
CTkAppWidget::
setHighlightForeground(const QColor &c)
{
  highlightForeground_ = c;

  updatePalette();
}

void
CTkAppWidget::
setInsertBackground(const QColor &c)
{
  insertBackground_ = c;

  updatePalette();
}

void
CTkAppWidget::
updatePalette()
{
  auto pal = qwidget_->palette();

  auto bgRole = getQWidget()->backgroundRole();
  auto fgRole = getQWidget()->foregroundRole();

  if (background_.isValid()) {
    pal.setColor(QPalette::Normal  , bgRole, background_);
    pal.setColor(QPalette::Active  , bgRole, background_);
    pal.setColor(QPalette::Inactive, bgRole, CTkAppUtil::grayColor(background_));
  }

  if (foreground_.isValid()) {
    pal.setColor(QPalette::Normal  , fgRole, foreground_);
    pal.setColor(QPalette::Active  , fgRole, foreground_);
    pal.setColor(QPalette::Inactive, fgRole, CTkAppUtil::grayColor(foreground_));
  }

  if (activeBackground_.isValid()) {
  //pal.setColor(QPalette::Active  , role, activeBackground_);
  //pal.setColor(QPalette::Inactive, role, CTkAppUtil::grayColor(activeBackground_));
  }

  if (activeForeground_.isValid()) {
  //pal.setColor(QPalette::Active  , role, activeForeground_);
  //pal.setColor(QPalette::Inactive, role, CTkAppUtil::grayColor(activeForeground_));
  }

  if (disabledBackground_.isValid()) {
    pal.setColor(QPalette::Disabled, bgRole, disabledBackground_);
  }

  if (disabledForeground_.isValid()) {
    pal.setColor(QPalette::Disabled, fgRole, disabledForeground_);
  }

  if (highlightBackground_.isValid()) {
    pal.setColor(QPalette::Active  , QPalette::Highlight, highlightBackground_);
    pal.setColor(QPalette::Inactive, QPalette::Highlight,
                 CTkAppUtil::grayColor(highlightBackground_));
  }

  if (highlightForeground_.isValid()) {
    pal.setColor(QPalette::Active  , QPalette::HighlightedText, highlightForeground_);
    pal.setColor(QPalette::Inactive, QPalette::HighlightedText,
                 CTkAppUtil::grayColor(highlightForeground_));
  }

  qwidget_->setBackgroundRole(bgRole);
  qwidget_->setForegroundRole(fgRole);

  qwidget_->setPalette(pal);

  qwidget_->update();
}

//---

void
CTkAppWidget::
setImageRef(const CTkAppImageRef &i)
{
  imageRef_ = i;

  imageRef_->addRef(getName());
}

double
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

double
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
setWidth(double w)
{
  //assert(w >= 0 && w <= 2048);

  w_ = w;
}

void
CTkAppWidget::
setHeight(double h)
{
  //assert(h >= 0 && h <= 2048);

  h_ = h;
}

void
CTkAppWidget::
updateWmSizeHints()
{
  if (wmMinAspect_ && wmMaxAspect_)
    CTkAppX11::setWmAspect(getQWidget()->winId(), *wmMinAspect_, *wmMaxAspect_);
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

QMargins
CTkAppWidget::
contentsMargins() const
{
  return QMargins(padX(), padY(), padX(), padY());
  //return qwidget_->contentsMargins();
}

void
CTkAppWidget::
addChild(CTkAppWidget *w)
{
  auto name = w->getName();

  auto pc = children_.find(name);

  if (pc != children_.end()) {
    assert((*pc).second->isDeleted());

    children_.erase(pc);
  }

  children_[name] = w;
}

void
CTkAppWidget::
removeChild(CTkAppWidget *w)
{
  auto pc = children_.find(w->getName());

  if (pc != children_.end() && (*pc).second == w)
    children_.erase(pc);
}

void
CTkAppWidget::
getChildren(Children &children) const
{
  for (const auto &pc : children_) {
    if (! pc.second->isDeleted())
      children.push_back(pc.second);
  }
}

CTkAppWidget *
CTkAppWidget::
getChild(const QString &name) const
{
  auto pc = children_.find(name);

  if (pc == children_.end())
    return nullptr;

  if ((*pc).second->isDeleted())
    return nullptr;

  return (*pc).second;
}

void
CTkAppWidget::
getColormapChildren(Children &children) const
{
  for (const auto &pc : children_) {
    if (pc.second->isDeleted()) continue;

    if (pc.second->hasColormap())
      children.push_back(pc.second);
  }
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

  if (! l1 && create) {
    delete l;

    l1 = new CTkAppPackLayout(this);
  }

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

  if (! l1 && create) {
    delete l;

    l1 = new CTkAppGridLayout(this);
  }

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

  if (! l1 && create) {
    delete l;

    l1 = new CTkAppPlaceLayout(this);
  }

  return l1;
}

bool
CTkAppWidget::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-activebackground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");

    setActiveBackground(c);
  }
  else if (name == "-activeforeground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");

    setActiveForeground(c);
  }
  else if (name == "-anchor") {
    Qt::Alignment align;
    if (! CTkAppUtil::variantToAlign(tk_, var, align))
      return tk_->throwError("bad anchor \"" + tk_->variantToString(var) + "\": must be "
                             "n, ne, e, se, s, sw, w, nw, or center");

    setAnchor(align);
  }
  else if (name == "-background" || name == "-bg") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");

    setBackground(c);
  }
  else if (name == "-bitmap") {
    auto value = tk_->variantToString(var);

    if (! setBitmap(value)) {
      if (value[0] == '@') {
        auto value1 = value.mid(1);
        return tk_->throwError("error reading bitmap file \"" + value1 + "\"");
      }
      else
        return tk_->throwError("bitmap \"" + value + "\" not defined");
    }
  }
  else if (name == "-borderwidth" || name == "-bd") {
    CTkAppDistance w;
    if (! tk_->variantToDistance(var, w))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    setBorderWidth(w.rvalue);
  }
  else if (name == "-colormap") {
    auto value = tk_->variantToString(var);

    if (value == "new") {
      setColormap(true, nullptr);
    }
    else {
      auto *w = tk_->lookupWidgetByName(value);
      if (! w) return false;

      setColormap(true, w);
    }
  }
  else if (name == "-compound") {
    CTkAppCompoundType compoundType;
    if (! CTkAppUtil::variantToCompound(tk_, var, compoundType))
      return tk_->throwError(tk_->msg() + "bad compound \"" + var + "\": must be "
                             "bottom, center, left, none, right, or top");
    setCompoundType(compoundType);
  }
  else if (name == "-cursor") {
    CTkAppCursorData cursorData;

    if (tk_->variantIsValid(var)) {
      if (! tk_->variantToCursor(var, cursorData))
        return tk_->throwError(tk_->msg() + "bad cursor spec \"" + var + "\"");

      auto *cursor = tk_->getCursor(cursorData.name);
      if (! cursor)
        return tk_->throwError(tk_->msg() + "bad cursor spec \"" + var + "\"");
    }

    setCursor(cursorData);
  }
  else if (name == "-disabledbackground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");

    setDisabledBackground(c);
  }
  else if (name == "-disabledforeground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");

    setDisabledBackground(c);
  }
  else if (name == "-font") {
    auto value = tk_->variantToString(var);

    if (value == "")
      return tk_->throwError("font \"" + value + "\" doesn't exist");

    auto f = tk_->getQFont(value);

    qwidget_->setFont(f);

    qwidget_->update();
  }
  else if (name == "-foreground" || name == "-fg") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");

    setForeground(c);
  }
  else if (name == "-height") {
    CTkAppDistance h;
    if (! tk_->variantToDistanceI(var, h))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    setHeight(h.ivalue);

    qwidget_->resize(getWidth(), getHeight());
  }
  else if (name == "-highlightbackground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");

    setHighlightBackground(c);
  }
  else if (name == "-highlightcolor") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");
    setHighlightForeground(c);
  }
  else if (name == "-highlightthickness") {
    CTkAppDistance thickness;
    if (! tk_->variantToDistance(var, thickness))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    setHighlightThickness(thickness.rvalue);
  }
  else if (name == "-image") {
    auto value = tk_->variantToString(var);

    if (! setImage(value))
      return tk_->throwError("image \"" + value + "\" doesn't exist");
  }
  else if (name == "-insertbackground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");

    setInsertBackground(c);
  }
  else if (name == "-insertborderwidth") {
    CTkAppDistance w;
    if (! tk_->variantToDistance(var, w))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    setInsertBorderWidth(w.rvalue);
  }
  else if (name == "-insertofftime") {
    long t;
    if (! tk_->variantToInt(var, t))
      return tk_->invalidInteger(var);

    setInsertOffTime(t);
  }
  else if (name == "-insertontime") {
    long t;
    if (! tk_->variantToInt(var, t))
      return tk_->invalidInteger(var);

    setInsertOnTime(t);
  }
  else if (name == "-insertwidth") {
    CTkAppDistance w;
    if (! tk_->variantToDistance(var, w))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    setInsertWidth(w.rvalue);
  }
  else if (name == "-justify") {
    Qt::Alignment align;
    if (! CTkAppUtil::variantToJustify(var, align))
      return tk_->throwError(tk_->msg() + "bad justification \"" + var + "\": must be "
                             "left, right, or center");

    setJustify(align);
  }
  else if (name == "-padx") {
    CTkAppDistance pad;
    if (! tk_->variantToDistanceI(var, pad))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    setPadX(pad.ivalue);
  }
  else if (name == "-pady") {
    CTkAppDistance pad;
    if (! tk_->variantToDistanceI(var, pad))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    setPadY(pad.ivalue);
  }
  else if (name == "-relief") {
    if (! setReliefFromVar(var))
      return tk_->throwError(tk_->msg() + "bad relief \"" + var + "\": must be "
                             "flat, groove, raised, ridge, solid, or sunken");
  }
  else if (name == "-repeatdelay") {
    long i;
    if (! tk_->getOptionInt(name, var, i))
      return tk_->invalidInteger(var);

    setRepeatDelay(i);
  }
  else if (name == "-repeatinterval") {
    long i;
    if (! tk_->getOptionInt(name, var, i))
      return tk_->invalidInteger(var);

    setRepeatInterval(i);
  }
  else if (name == "-selectbackground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");

    selectBackground_ = c;
  }
  else if (name == "-selectborderwidth") {
    CTkAppDistance w;
    if (! tk_->variantToDistance(var, w))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    setSelectBorderWidth(w.rvalue);
  }
  else if (name == "-selectforeground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk_, var, c))
      return tk_->throwError(tk_->msg() + "unknown color name \"" + var + "\"");
    selectForeground_ = c;
  }
  else if (name == "-setgrid") {
    bool b;
    if (! tk_->variantToBool(var, b))
      return tk_->throwError(tk_->msg() + "expected boolean value but got \"" + var + "\"");
    setGridWidget(b);
  }
  else if (name == "-takefocus") {
    bool b;
    if (tk_->variantToBool(var, b))
      qwidget_->setFocusPolicy(b ? Qt::TabFocus : Qt::NoFocus);
    else
      tk_->TODO(name); // allow arbritray value
  }
  else if (name == "-text") {
    auto value = tk_->variantToString(var);

    setText(value);
  }
  else if (name == "-visual") {
    // implement Tk_GetVisual
    auto value = tk_->variantToString(var);

    if (value != "" && value[0] == ".") {
      auto *w = tk_->lookupWidgetByName(value);
      if (! w) return false;

      setVisual(w->visual());
    }
    else
      setVisual(value);
  }
  else if (name == "-width") {
    CTkAppDistance w;
    if (! tk_->variantToDistanceI(var, w))
      return tk_->throwError(tk_->msg() + "bad screen distance \"" + var + "\"");

    setWidth(w.ivalue);

    qwidget_->resize(getWidth(), getHeight());
  }
  else if (name == "-xscrollcommand") {
    auto value = tk_->variantToString(var);

    setXScrollCommand(value);
  }
  else if (name == "-yscrollcommand") {
    auto value = tk_->variantToString(var);

    setYScrollCommand(value);
  }
  else
    return tk_->throwError("Invalid value name \"" + name + "\" "
                           "for " + QString(getClassName()));

  return true;
}

bool
CTkAppWidget::
execDefaultOp(const QString &option, const Args &args)
{
  // set config name/value
  if      (option == "configure") {
    return widgetCommand_->execConfigure(args);
  }
  // get config name/value
  else if (option == "cget") {
    return widgetCommand_->execCGet(args);
  }
  else {
    return execOp(args);
  }
}

bool
CTkAppWidget::
execOp(const Args &args)
{
  auto arg = tk_->variantToString(args[0]);

  return tk_->throwError("bad option \"" + arg + "\" for " + QString(getClassName()));
}

bool
CTkAppWidget::
show()
{
  if (qwidget_ && ! qwidget_->isVisible())
    qwidget_->setVisible(true);

  return true;
}

void
CTkAppWidget::
hide()
{
  if (qwidget_ && qwidget_->isVisible())
    qwidget_->setVisible(false);
}

void
CTkAppWidget::
setTitle(const QString &title)
{
  qwidget_->setWindowTitle(title);
}

QString
CTkAppWidget::
getTitle() const
{
  return qwidget_->windowTitle();
}

void
CTkAppWidget::
setIcon(const QString &name)
{
  icon_     = QIcon(name);
  iconName_ = name;

  qwidget_->setWindowIcon(icon_);
}

QString
CTkAppWidget::
getIcon() const
{
  return iconName_;
}

bool
CTkAppWidget::
setGeometry(const QString &str)
{
  return setTransientGeometry(nullptr, str);
}

bool
CTkAppWidget::
setTransientGeometry(CTkAppWidget *pw, const QString &str)
{
  auto rect = qwidget_->geometry();

  int x = rect.x    (), y = rect.y     ();
  int w = rect.width(), h = rect.height();

  // format WxH+X+Y
  CQStrParse parse(str);

  parse.skipSpace();

  // optional '='
  if (parse.isChar('='))
    parse.skipChar();

  // <width>x<height>
  if (! parse.isChar('-') && ! parse.isChar('+')) {
    if (parse.readInteger(&w)) {
      if (! parse.isChar('x'))
        return false;

      parse.skipChar();

      if (! parse.readInteger(&h))
        return false;

      parse.skipSpace();
    }
  }

  // +-<x>+-<y>
  if (parse.isChar('-') || parse.isChar('+')) {
    int sign = (parse.isChar('+') ? 1 : -1);

    parse.skipChar();

    if (! parse.readInteger(&x))
      return false;

    x *= sign;

    if (! parse.isChar('-') && ! parse.isChar('+'))
      return false;

    sign = (parse.isChar('+') ? 1 : -1);

    parse.skipChar();

    if (! parse.readInteger(&y))
      return false;

    y *= sign;
  }

  parse.skipSpace();

  if (! parse.eof())
    return false;

  QRect r(x, y, w, h);

  if (pw) {
    auto p = pw->getQWidget()->mapToGlobal(r.topLeft());

    r.setTopLeft(p);
  }

  qwidget_->setGeometry(r);

  return true;
}

QString
CTkAppWidget::
getGeometry() const
{
  auto r = qwidget_->geometry();

  auto str = QString("%1x%2+%3+%4").arg(r.width()).arg(r.height()).arg(r.x()).arg(r.y());

  return str;
}

void
CTkAppWidget::
setPadX(int padx)
{
  padx_ = padx;

  auto m = qwidget_->contentsMargins();
  m.setLeft(padx); m.setRight(padx);
  qwidget_->setContentsMargins(m);
}

void
CTkAppWidget::
setPadY(int pady)
{
  pady_ = pady;

  auto m = qwidget_->contentsMargins();
  m.setTop(pady); m.setBottom(pady);
  qwidget_->setContentsMargins(m);
}

bool
CTkAppWidget::
setAnchorStr(const QString &str)
{
  Qt::Alignment align;
  if (! CTkAppUtil::variantToAlign(tk_, str, align))
    return false;
  anchorStr_ = str;
  setAnchor(align);
  return true;
}

bool
CTkAppWidget::
setBitmap(const QString &s)
{
  auto image = tk_->getBitmap(s);
  if (! image) return false;

  bitmap_ = s;

  setImageRef(image);

  return true;
}

bool
CTkAppWidget::
setImage(const QString &s)
{
  auto image = tk_->getImage(s);
  if (! image) return false;

  image_ = s;

  setImageRef(image);

  return true;
}

bool
CTkAppWidget::
setCursor(const CTkAppCursorData &c)
{
  cursor_ = c;

  //auto c = CTkAppUtil::stringToCursor(s);
  //qwidget_->setCursor(c);

  return true;
}

//---

// set borderwidth
void
CTkAppWidget::
setBorderWidth(double w)
{
  borderWidth_ = w;

  auto *frame = qobject_cast<QFrame *>(qwidget_);

  if (frame)
    frame->setLineWidth(w);
}

void
CTkAppWidget::
setSelectBorderWidth(double w)
{
  selectBorderWidth_ = w;
}

void
CTkAppWidget::
setInsertBorderWidth(double w)
{
  insertBorderWidth_ = w;
}

void
CTkAppWidget::
setInsertWidth(double w)
{
  insertWidth_ = w;
}

bool
CTkAppWidget::
setReliefStr(const QString &s)
{
  return setReliefFromVar(s);
}

bool
CTkAppWidget::
setReliefFromVar(const QVariant &var)
{
  Relief relief { Relief::NONE };
  if (! CTkAppUtil::variantToRelief(tk_, var, relief))
    return false;

  relief_    = relief;
  reliefVar_ = var;

  CTkAppUtil::setFrameRelief(qwidget_, relief_);

  appearanceChanged();

  return true;
}

//---

std::vector<QString>
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
getBindings(const CTkAppEventData &data, std::vector<QString> &bindings)
{
  for (const auto &edata : eventDatas_)
    if (edata == data)
      bindings.push_back(edata.pattern);
}

void
CTkAppWidget::
getBindings(std::vector<QString> &bindings)
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
  // apply x11/qt event to widget event handlers
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

void
CTkAppWidget::
generateEvent(const CTkAppEventData &eventData)
{
  if (! qwidget_)
    return;

  switch (eventData.type) {
    case CTkAppEventType::Enter: {
      QEnterEvent e(QPointF(0, 0), QPointF(0, 0), QPointF(0, 0));
      QApplication::sendEvent(qwidget_, &e);
      break;
    }
    case CTkAppEventType::Button: {
      auto button  = static_cast<Qt::MouseButton >(eventData.button);
      auto buttons = static_cast<Qt::MouseButtons>(eventData.button);

      auto pos = QPointF(eventData.x, eventData.y);

      Qt::KeyboardModifiers modifiers = Qt::NoModifier;

      QEvent::Type type = QEvent::MouseButtonPress;

      switch (eventData.mode) {
        case CTkAppEventMode::Press  : type = QEvent::MouseButtonPress  ; break;
        case CTkAppEventMode::Release: type = QEvent::MouseButtonRelease; break;
        case CTkAppEventMode::Motion : type = QEvent::MouseMove         ; break;
        default: {
          std::cerr << "Unhandler mouse mode for CTkAppWidget::generateEvent\n";
          break;
        }
      }

      QMouseEvent e(type, pos, button, buttons, modifiers);
      QApplication::sendEvent(qwidget_, &e);

      break;
    }
    case CTkAppEventType::Key: {
      Qt::KeyboardModifiers modifiers = Qt::NoModifier;

      QKeyEvent e(QEvent::KeyPress, eventData.key, modifiers, eventData.keyStr);
      QApplication::sendEvent(qwidget_, &e);

      break;
    }
    default: {
      std::cerr << "Unhandled type for CTkAppWidget::generateEvent\n";
      break;
    }
  }
}

//---

void
CTkAppWidget::
deleteLater()
{
  if (deleted_)
    return;

  //---

  for (const auto &pc : children_)
    pc.second->deleteLater();

  //---

  destroySlot(this);

  //---

  deleted_ = true;

  tk_->addDeleteWidget(this);

  auto *command = getWidgetCommand();

  if (command)
    command->deleteCommand();
}

void
CTkAppWidget::
runCommand(const Args &args)
{
  auto cmd = getCommand();
  if (cmd == "") return;

  for (const auto &arg : args)
    cmd += " " + arg.toString();

  tk_->eval(cmd);
}

bool
CTkAppWidget::
getOptionValue(const QString &optName, const QString &optClass, QVariant &optValue) const
{
  const auto &opts = getWidgetCommand()->getOpts();

  if (opts.getDefValue(optName, optClass, optValue)) {
    //std::cerr << "getOptionValue: " << optName << " " << optClass << " = " << optValue << "\n";
    return true;
  }

  auto widgetClass = QString(getClassName());

  if (getOptionClass() != "")
    widgetClass = getOptionClass();

  if (tk_->matchOption(widgetClass, optName, optClass, optValue)) {
    //std::cerr << "getOptionValue: " << optName << " " << optClass << " = " << optValue << "\n";
    return true;
  }

  std::cerr << "Error in getOptionValue: " <<
    widgetClass.toStdString() << ":" << optName.toStdString() <<
    " " << optClass.toStdString() << " = " << tk_->variantToString(optValue).toStdString() << "\n";

  return true;
}

void
CTkAppWidget::
setWmAtomValue(const QString &atomName, const QString &atomValue)
{
  if (atomName == "WM_DELETE_WINDOW")
    setDeleteWindowCmd(atomValue);

  wmAtoms_[atomName] = atomValue;
}

QString
CTkAppWidget::
getWmAtomValue(const QString &atomName) const
{
  auto p = wmAtoms_.find(atomName);

  return (p != wmAtoms_.end() ? (*p).second : "");
}

void
CTkAppWidget::
setWmTransientFor(const QString &name)
{
  wmTransientFor_ = name;
}

void
CTkAppWidget::
setOverrideRedirect(bool b)
{
  overrideRedirect_ = b;

  if (b)
    getQWidget()->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint |
                                 Qt::X11BypassWindowManagerHint);
  else
    getQWidget()->setWindowFlags(Qt::Window);
}

bool
CTkAppWidget::
setWidgetStateFromString(const QString &value)
{
  CTkAppWidgetState state;
  if (! CTkAppUtil::variantToState(tk_, value, state))
    return false;

  setWidgetState(state);

  auto *qw = this->getQWidget();

  auto *qbutton = qobject_cast<QAbstractButton *>(qw);

  if (qbutton) {
    if      (state == CTkAppWidgetState::ACTIVE)
      qbutton->setDown(true);
    else if (state == CTkAppWidgetState::DISABLED)
      qbutton->setEnabled(false);
    else if (state == CTkAppWidgetState::NORMAL) {
      qbutton->setEnabled(true);
      qbutton->setDown(false);
    }
  }
  else {
    if      (state == CTkAppWidgetState::ACTIVE)
      std::cerr << "Unsupported state '" << value.toStdString() << "'\n";
    else if (state == CTkAppWidgetState::DISABLED)
      qw->setEnabled(false);
    else if (state == CTkAppWidgetState::NORMAL)
      qw->setEnabled(true);
  }

  return true;
}

QString
CTkAppWidget::
widgetStateToString(const CTkAppWidgetState &state) const
{
  switch (state) {
    case CTkAppWidgetState::ACTIVE  : return "active";
    case CTkAppWidgetState::DISABLED: return "disabled";
    case CTkAppWidgetState::NORMAL  : return "normal";
    case CTkAppWidgetState::HIDDEN  : return "hidden";
    default: return "";
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

    case QEvent::Show: {
      break;
    }

    case QEvent::Hide: {
      break;
    }

    default: break;
  }

  // standard event processing
  return QObject::eventFilter(obj, event);
}
