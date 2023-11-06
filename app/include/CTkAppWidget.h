#ifndef CTkAppWidget_H
#define CTkAppWidget_H

#include <CTkAppEventData.h>
#include <CTkApp.h>
#include <CTkAppUtil.h>

#include <CQLabelImage.h>

#include <QBrush>
#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPainterPath>
#include <QPen>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QToolButton>
#include <QTreeWidget>
#include <QWidgetAction>

#include <optional>
#include <cmath>

class CTkApp;
class CTkAppRoot;
class CTkAppTopLevel;
class CTkAppWidgetCommand;
class CTkAppOptData;

class CTkAppLayout;
class CTkAppPackLayout;
class CTkAppGridLayout;
class CTkAppPlaceLayout;

class CQRealSlider;

class QBoxLayout;
class QFrame;
class QGroupBox;
class QMenuBar;
class QSplitter;
class QTabWidget;
class QTextCursor;
class QTextEdit;
class QTreeWidget;

//---

class CTkAppWidgetEventFilter;

class CTkAppWidget : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString type     READ getType)
  Q_PROPERTY(QString name     READ getName)
  Q_PROPERTY(QString fullName READ getFullName)

  Q_PROPERTY(QColor background READ background WRITE setBackground)
  Q_PROPERTY(QColor foreground READ foreground WRITE setForeground)

  Q_PROPERTY(QColor activeBackground READ activeBackground WRITE setActiveBackground)
  Q_PROPERTY(QColor activeForeground READ activeForeground WRITE setActiveForeground)

  Q_PROPERTY(QColor disabledBackground READ disabledBackground WRITE setDisabledBackground)
  Q_PROPERTY(QColor disabledForeground READ disabledForeground WRITE setDisabledForeground)

  Q_PROPERTY(QColor highlightBackground READ highlightBackground WRITE setHighlightBackground)
  Q_PROPERTY(QColor highlightForeground READ highlightForeground WRITE setHighlightForeground)

  Q_PROPERTY(QColor insertBackground READ insertBackground WRITE setInsertBackground)

  Q_PROPERTY(QString relief READ reliefStr WRITE setReliefStr)

  Q_PROPERTY(int repeatDelay    READ repeatDelay    WRITE setRepeatDelay)
  Q_PROPERTY(int repeatInterval READ repeatInterval WRITE setRepeatInterval)

  Q_PROPERTY(QString       anchorStr READ anchorStr WRITE setAnchorStr)
  Q_PROPERTY(Qt::Alignment anchor    READ anchor    WRITE setAnchor)

  Q_PROPERTY(double borderWidth       READ borderWidth       WRITE setBorderWidth)
  Q_PROPERTY(double selectBorderWidth READ selectBorderWidth WRITE setSelectBorderWidth)
  Q_PROPERTY(double insertBorderWidth READ insertBorderWidth WRITE setInsertBorderWidth)
  Q_PROPERTY(double insertWidth       READ insertWidth       WRITE setInsertWidth)

  Q_PROPERTY(QString bitmap READ bitmap WRITE setBitmap)

  Q_PROPERTY(double highlightThickness READ highlightThickness WRITE setHighlightThickness)

  Q_PROPERTY(QString text     READ getText     WRITE setText)
  Q_PROPERTY(double  width    READ getWidth    WRITE setWidth)
  Q_PROPERTY(double  height   READ getHeight   WRITE setHeight)
  Q_PROPERTY(QString title    READ getTitle    WRITE setTitle)
  Q_PROPERTY(QString icon     READ getIcon     WRITE setIcon)
  Q_PROPERTY(QString geometry READ getGeometry WRITE setGeometry)

  Q_PROPERTY(int padX READ padX WRITE setPadX)
  Q_PROPERTY(int padY READ padY WRITE setPadY)

  Q_PROPERTY(Qt::Alignment justify READ justify WRITE setJustify)

  Q_PROPERTY(QString command        READ getCommand        WRITE setCommand)
  Q_PROPERTY(QString xScrollCommand READ getXScrollCommand WRITE setXScrollCommand)
  Q_PROPERTY(QString yScrollCommand READ getYScrollCommand WRITE setYScrollCommand)

  Q_PROPERTY(bool gridWidget READ isGridWidget WRITE setGridWidget)

  Q_PROPERTY(QString deleteWindowCmd READ deleteWindowCmd WRITE setDeleteWindowCmd)
  Q_PROPERTY(QString wmTransientFor  READ wmTransientFor  WRITE setWmTransientFor)

 public:
  using Relief   = CTkAppWidgetRelief;
  using Args     = std::vector<QVariant>;
  using Children = std::vector<CTkAppWidget *>;

 public:
  explicit CTkAppWidget(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  virtual ~CTkAppWidget();

  //---

  CTkApp *app() const { return tk_; }

  CTkAppWidget *getParent() const { return parent_; }

  QWidget *parentWidget() const { return parent_->getQWidget(); }
  void setParentWidget(QWidget *w);

  CTkAppRoot *root() const;

  CTkAppTopLevel *toplevel() const;

  virtual bool isTopLevel() const { return false; }

  //---

  virtual const char *getClassName() const = 0;

  //---

  QString getType() const { return QString(getClassName()); }

  const QString &getName() const { return name_; }

  QString getFullName() const;

  //---

  void setQWidget(QWidget *w);
  QWidget *getQWidget() const;

  const CTkAppWidgetCommand *getWidgetCommand() const { return widgetCommand_; }
  void setWidgetCommand(CTkAppWidgetCommand *p) { widgetCommand_ = p; }

  const CTkAppOptData &getOpts() const;

  //---

  const QColor &background() const { return background_; }
  void setBackground(const QColor &c);

  const QColor &foreground() const { return foreground_; }
  void setForeground(const QColor &c);

  const QColor &activeBackground() const { return activeBackground_; }
  void setActiveBackground(const QColor &c);

  const QColor &activeForeground() const { return activeForeground_; }
  void setActiveForeground(const QColor &c);

  const QColor &disabledBackground() const { return disabledBackground_; }
  void setDisabledBackground(const QColor &c);

  const QColor &disabledForeground() const { return disabledForeground_; }
  void setDisabledForeground(const QColor &c);

  const QColor &highlightBackground() const { return highlightBackground_; }
  void setHighlightBackground(const QColor &c);

  const QColor &highlightForeground() const { return highlightForeground_; }
  void setHighlightForeground(const QColor &c);

  const QColor &insertBackground() const { return insertBackground_; }
  void setInsertBackground(const QColor &c);

  void updatePalette();

  //---

  double highlightThickness() const { return highlightThickness_; }
  void setHighlightThickness(double r) { highlightThickness_ = r; }

  //---

  const QString &getText() const { return text_; }
  virtual void setText(const QString &s) { text_ = s; }

  //---

  const QString &bitmap() const { return bitmap_; }
  bool setBitmap(const QString &s);

  const QString &image() const { return image_; }
  bool setImage(const QString &s);

  const CTkAppImageRef &getImageRef() const { return imageRef_; }
  virtual void setImageRef(const CTkAppImageRef &i);

  const QString &cursor() const { return cursor_; }
  void setCursor(const QString &s);

  const CTkAppCompoundType &compoundType() const { return compoundType_; }
  void setCompoundType(const CTkAppCompoundType &v) { compoundType_ = v; }

  //---

  virtual double getWidth() const;
  virtual void setWidth(double w);

  virtual double getHeight() const;
  virtual void setHeight(double h);

  //---

  virtual QSize sizeHint() const;

  virtual QSize minimumSizeHint() const;

  QMargins contentsMargins() const;

  //---

  bool isDeleted() const { return deleted_; }

  //---

  void addChild   (CTkAppWidget *w);
  void removeChild(CTkAppWidget *w);

  void getChildren(Children &children) const;

  CTkAppWidget *getChild(const QString &name) const;

  //---

  bool isInitNotify() const { return initNotify_; }
  void setInitNotify(bool b) { initNotify_ = b; }

  //---

  void setFocus();

  //---

  void lower();
  void raise();

  //---

  const QString &getXScrollCommand() const { return xScrollCommand_; }
  void setXScrollCommand(const QString &s) { xScrollCommand_ = s; }

  const QString &getYScrollCommand() const { return yScrollCommand_; }
  void setYScrollCommand(const QString &s) { yScrollCommand_ = s; }

  //---

  CTkAppLayout *getTkLayout();

  CTkAppPackLayout  *getTkPackLayout(bool create=true);
  CTkAppGridLayout  *getTkGridLayout(bool create=true);
  CTkAppPlaceLayout *getTkPlaceLayout(bool create=true);

  const Qt::Alignment &getGridAnchor() const { return gridAnchor_; }
  void setGridAnchor(const Qt::Alignment &a) { gridAnchor_ = a; }

  void getGridBBox(int &column1, int &row1, int &column2, int &row2) const {
    column1 = gridColumn1_; row1 = gridRow1_;
    column2 = gridColumn2_; row2 = gridRow2_;
  }
  void setGridBBox(int column1, int row1, int column2, int row2) {
    gridColumn1_ = column1; gridRow1_ = row1;
    gridColumn2_ = column2; gridRow2_ = row2;
  }

  bool getGridPropagate() const { return gridPropagate_; }
  void setGridPropagate(bool b) { gridPropagate_ = b; }

  //---

  // handle name/value change (configure)
  virtual bool execConfig(const QString &name, const QVariant &value);

  // handle widget operation (<widget> <operation>)
  virtual bool execOp(const Args &);

  virtual void show();
  virtual void hide();

  //---

  QString getTitle() const;
  void setTitle(const QString &s);

  QString getIcon() const;
  void setIcon(const QString &s);

  QString getGeometry() const;
  bool setGeometry(const QString &s);

  bool setTransientGeometry(CTkAppWidget *pw, const QString &s);

  //---

  int padX() const { return padx_; }
  virtual void setPadX(int i);

  int padY() const { return pady_; }
  virtual void setPadY(int i);

  //---

  const Qt::Alignment &justify() const { return justify_; }
  virtual void setJustify(const Qt::Alignment &a) { justify_ = a; }

  //--

  void setWidgetAnchor(const QString &value);

  //---

  double borderWidth() const { return borderWidth_; }
  void setBorderWidth(double width);

  double selectBorderWidth() const { return selectBorderWidth_; }
  void setSelectBorderWidth(double width);

  double insertBorderWidth() const { return insertBorderWidth_; }
  void setInsertBorderWidth(double width);

  double insertWidth() const { return insertWidth_; }
  void setInsertWidth(double width);

  //---

  const QString &getOptionClass() const { return optionClass_; }
  void setOptionClass(const QString &s) { optionClass_ = s; }

  //---

  const Relief &relief() const { return relief_; }
  void setRelief(const Relief &v) { relief_ = v; }

  QString reliefStr() const { return reliefVar_.toString(); }
  bool setReliefStr(const QString &s);

  bool setReliefFromVar(const QVariant &s);

  //---

  int repeatDelay() const { return repeatDelay_; }
  void setRepeatDelay(int i) { repeatDelay_ = i; }

  int repeatInterval() const { return repeatInterval_; }
  void setRepeatInterval(int i) { repeatInterval_ = i; }

  //---

  const Qt::Alignment &anchor() const { return anchor_; }
  virtual void setAnchor(const Qt::Alignment &a) { anchor_ = a; }

  const QString &anchorStr() const { return anchorStr_; }
  bool setAnchorVar(const QVariant &var);
  bool setAnchorStr(const QString &s);

  bool isGridWidget() const { return gridWidget_; }
  void setGridWidget(bool b) { gridWidget_ = b; }

  const QString &deleteWindowCmd() const { return deleteWindowCmd_; }
  void setDeleteWindowCmd(const QString &s) { deleteWindowCmd_ = s; }

  //---

  std::vector<QString> bindtags() const;
  void setBindtags(const std::vector<QString> &strs) { bindtags_ = strs; }

  bool bindEvent(const CTkAppEventData &data);

  void getBindings(const CTkAppEventData &data, std::vector<QString> &bindings);
  void getBindings(std::vector<QString> &bindings);

  virtual bool triggerMousePressEvents  (QEvent *e, int button);
  virtual bool triggerMouseMoveEvents   (QEvent *e, int button, bool pressed);
  virtual bool triggerMouseReleaseEvents(QEvent *e, int button);

  virtual bool triggerKeyPressEvents(QEvent *e);

  virtual bool triggerEvents(QEvent *e, const CTkAppEventData &matchEventData);
  virtual bool triggerVirtualEvents(const CTkAppEventData &matchEventData);

  void processEvents(QEvent *e, const CTkAppEventData &matchEventData);
  void processVirtualEvents(const CTkAppEventData &matchEventData);

  //---

  virtual void appearanceChanged() { }

  //---

  void deleteLater();

  bool getOptionValue(const QString &optName, const QString &optClass,
                      QVariant &optValue) const;

  //---

  void setResizable(bool /*x*/, bool /*y*/) { }

  //---

  void setWmAtomValue(const QString &atomName, const QString &atomValue);
  QString getWmAtomValue(const QString &atomName) const;

  //---

  const QString &wmTransientFor() const { return wmTransientFor_; }
  void setWmTransientFor(const QString &name);

  bool setWidgetState(const QString &value);

 protected:
  const QString &getCommand() const { return command_; }
  void setCommand(const QString &command) { command_ = command; }

  void runCommand(const Args &args=Args());

 protected:
  using WidgetP    = QPointer<QWidget>;
  using AppWidgetP = QPointer<CTkAppWidget>;
  using WidgetMap  = std::map<QString, AppWidgetP>;
  using EventDatas = std::vector<CTkAppEventData>;
  using WmAtoms    = std::map<QString, QString>;

  CTkApp*       tk_     { nullptr };
  CTkAppWidget* parent_ { nullptr };
  QString       name_;

  CTkAppWidgetCommand *widgetCommand_ { nullptr };

  int        w_       { 0 };
  int        h_       { 0 };
  bool       deleted_ { false };
  WidgetP    qwidget_;
  WidgetMap  children_;
  EventDatas eventDatas_;

  QString xScrollCommand_;
  QString yScrollCommand_;

  Relief   relief_ { Relief::NONE };
  QVariant reliefVar_;

  int repeatDelay_    { -1 };
  int repeatInterval_ { -1 };

  Qt::Alignment anchor_ { Qt::AlignCenter };
  QString       anchorStr_;

  double borderWidth_       { 0.0 };
  double selectBorderWidth_ { 0.0 };
  double insertBorderWidth_ { 0.0 };
  double insertWidth_       { 0.0 };

  int padx_ { 0 };
  int pady_ { 0 };

  Qt::Alignment justify_ { Qt::AlignLeft | Qt::AlignTop };

  Qt::Alignment gridAnchor_    { Qt::AlignTop | Qt::AlignLeft };
  int           gridColumn1_   { 0 };
  int           gridRow1_      { 0 };
  int           gridColumn2_   { 0 };
  int           gridRow2_      { 0 };
  bool          gridPropagate_ { false };

  QString command_;

  QString optionClass_;

  QColor background_;
  QColor foreground_;
  QColor activeBackground_;
  QColor activeForeground_;
  QColor disabledBackground_;
  QColor disabledForeground_;
  QColor highlightBackground_;
  QColor highlightForeground_;
  QColor insertBackground_;

  double highlightThickness_ { -1 };

  QString text_;

  QString        bitmap_;
  QString        image_;
  CTkAppImageRef imageRef_;

  std::vector<QString> bindtags_;

  QColor selectBackground_;
  QColor selectForeground_;

  bool gridWidget_ { false }; // is size based on number of grid cells (chars)

  QString cursor_;

  bool initNotify_ { false };

  CTkAppCompoundType compoundType_ { CTkAppCompoundType::NONE };

  CTkAppWidgetEventFilter *eventFilter_ { nullptr };

  WmAtoms wmAtoms_;
  QString deleteWindowCmd_;
  QString wmTransientFor_;
};

//---

class CTkAppWidgetEventFilter :  public QObject {
  Q_OBJECT

 public:
  explicit CTkAppWidgetEventFilter(CTkAppWidget *w) : w_(w) { }

 protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

 private:
  CTkAppWidget* w_ { nullptr };

  mutable bool pressed_     { false };
  mutable int  pressButton_ { 0 };
};

//---

class CTkAppRootWidget;

class CTkAppRoot : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppRoot(CTkApp *tk);

  void setRoot(QFrame *qroot);

  const char *getClassName() const override { return "Root"; }

  bool isTopLevel() const override { return true; }

  void show() override;

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  bool decodeWidgetName(const QVariant &name, CTkAppWidget **parent, QString &childName) const;

 private:
  QFrame* qroot_ { nullptr };
  QString menuName_;
};

class CTkAppRootWidget : public QFrame {
  Q_OBJECT

 public:
  explicit CTkAppRootWidget(CTkAppRoot *root);

  void setMenu(QMenu *menu);

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private:
  CTkAppRoot* root_    { nullptr };
  QMenu*      menu_    { nullptr };
  QMenuBar*   menuBar_ { nullptr };
};

//---

class CTkAppButtonWidget;

class CTkAppButton : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(bool overRaised READ isOverRaised WRITE setOverRaised)

 public:
  explicit CTkAppButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Button"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  bool isOverRaised() const { return overRaised_; }
  void setOverRaised(bool b) { overRaised_ = b; }

  void setText(const QString &s) override;

  void setImageRef(const CTkAppImageRef &i) override;

  void setJustify(const Qt::Alignment &a) override;

  void flash();

  void setPadX(int i) override;
  void setPadY(int i) override;

  void appearanceChanged() override;

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void clickSlot();

 private:
  CTkAppButtonWidget* qbutton_    { nullptr };
  bool                overRaised_ { false };
};

//---

class CTkAppCanvasWidget;
class CTkAppCanvasShape;

class CTkAppCanvas : public CTkAppWidget {
  Q_OBJECT

 public:
  using Shape = CTkAppCanvasShape;

  explicit CTkAppCanvas(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Canvas"; }

  CTkAppCanvasWidget *canvasWidget() const { return qcanvas_; }

  Shape *insideShape() const { return insideShape_; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  double getWidth() const override { return width_; }
  void setWidth(double r) override { width_ = r; }

  double getHeight() const override { return height_; }
  void setHeight(double r) override { height_ = r; }

  //---

  bool triggerMousePressEvents(QEvent *e, int button) override;
  bool triggerMouseMoveEvents (QEvent *e, int button, bool pressed) override;

  void processShapeEvents(Shape *shape, QEvent *w, const CTkAppEventData &matchEventData);

  //---

  QString newGradientName() const {
    return QString("gradient%1").arg(gradients_.size() + 1);
  }

  void addGradient(const QString &name, QGradient *g) {
    gradients_[name] = g;
  }

  bool getGradient(const QString &name, QGradient* &g) {
    auto pg = gradients_.find(name);
    if (pg == gradients_.end()) return false;

    g = (*pg).second;

    return true;
  }

 private:
  CTkAppCanvasWidget* qcanvas_ { nullptr };

  using IdEventDatas = std::map<QString, EventDatas>;
  using Gradients    = std::map<QString, QGradient *>;

  IdEventDatas idEventDatas_;
  Shape*       insideShape_ { nullptr };
  double       width_  { 400 };
  double       height_ { 400 };
  Gradients    gradients_;
};

enum class CTkAppCanvasShapeType {
  NONE,
  RECTANGLE,
  OVAL,
  LINE,
  POLYGON,
  ARC,
  TEXT,
  IMAGE,
  BITMAP
#ifdef CTK_APP_TKPATH
, PATH,
  CIRCLE,
  ELLIPSE
#endif
};

struct CTkAppPoint {
  CTkAppPoint() = default;

  CTkAppPoint(double x1, double y1) :
   x(x1), y(y1) {
  }

  double x { 0.0 };
  double y { 0.0 };

  double distance(double x1, double y1) const {
    return std::hypot(x - x1, y - y1);
  }

  void moveBy(double dx, double dy) {
    x += dx;
    y += dy;
  }
};

using CTkAppPoints = std::vector<CTkAppPoint>;

class CTkAppCanvasShape : public QObject {
  Q_OBJECT

  Q_PROPERTY(int id READ id)

  Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
  Q_PROPERTY(bool visible READ isVisible WRITE setVisible)

  Q_PROPERTY(QColor          strokeColor READ strokeColor WRITE setStrokeColor)
  Q_PROPERTY(double          strokeAlpha READ strokeAlpha WRITE setStrokeAlpha)
  Q_PROPERTY(double          strokeWidth READ strokeWidth WRITE setStrokeWidth)
  Q_PROPERTY(Qt::PenCapStyle strokeCap   READ strokeCap   WRITE setStrokeCap)

  Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor)
  Q_PROPERTY(double fillAlpha READ fillAlpha WRITE setFillAlpha)

 public:
  using ShapeType = CTkAppCanvasShapeType;
  using Tags      = std::set<QString>;
  using Point     = CTkAppPoint;
  using Points    = CTkAppPoints;

  struct OffsetData {
    Qt::Alignment align    { };
    bool          isAlign  { false };
    bool          toplevel { false };
    int           x        { 0 };
    int           y        { 0 };
  };

 public:
  explicit CTkAppCanvasShape(CTkAppCanvasWidget *canvas, ShapeType type);

  virtual ~CTkAppCanvasShape() = default;

  //---

  ShapeType type() const { return type_; }

  int id() const { return id_; }

  //---

  bool isEnabled() const { return enabled_; }
  void setEnabled(bool b) { enabled_ = b; }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  //---

  const Tags &tags() const { return tags_; }
  void setTags(const Tags &v) { tags_ = v; }

  QString tagsStr() const;
  bool setTagsStr(const QString &s);

  //---

  QColor strokeColor() const { return pen_.color(); }
  void setStrokeColor(const QColor &c) { pen_.setColor(c); }

  double strokeAlpha() const { return pen_.color().alphaF(); }
  void setStrokeAlpha(double a) { auto c = pen_.color(); c.setAlphaF(a); pen_.setColor(c); }

  double strokeWidth() const { return pen_.widthF(); }
  void setStrokeWidth(double w) { pen_.setWidthF(w); }

  Qt::PenCapStyle strokeCap() const { return pen_.capStyle(); }
  void setStrokeCap(Qt::PenCapStyle cap) { pen_.setCapStyle(cap); }

  QColor fillColor() const { return brush_.color(); }
  void setFillColor(const QColor &c) { brush_.setColor(c); }

  double fillAlpha() const { return brush_.color().alphaF(); }
  void setFillAlpha(double a) { auto c = brush_.color(); c.setAlphaF(a);
                                brush_.setStyle(Qt::SolidPattern); brush_.setColor(c); }

  //--

  const QPen &pen() const { return pen_; }
  void setPen(const QPen &p) { pen_ = p; }

  const QPen &activePen() const { return activePen_; }
  void setActivePen(const QPen &p) { activePen_ = p; }

  const QPen &disabledPen() const { return disabledPen_; }
  void setDisabledPen(const QPen &p) { disabledPen_ = p; }

  const QPen &fillOverPen() const { return fillOverPen_; }
  void setFillOverPen(const QPen &p) { fillOverPen_ = p; }

  //---

  const QBrush &brush() const { return brush_; }
  void setBrush(const QBrush &b) { brush_ = b; }

  QString stipple() const { return stipple_; }
  bool setStipple(const QString &str);

  const QBrush &activeBrush() const { return activeBrush_; }
  void setActiveBrush(const QBrush &b) { activeBrush_ = b; }

  const QBrush &disabledBrush() const { return disabledBrush_; }
  void setDisabledBrush(const QBrush &b) { disabledBrush_ = b; }

  //---

  const QBrush &outlineBrush() const { return outlineBrush_; }
  void setOutlineBrush(const QBrush &b) { outlineBrush_ = b; }

  QString outlineStipple() const { return outlineStipple_; }
  bool setOutlineStipple(const QString &str);

  const QBrush &activeOutlineBrush() const { return activeOutlineBrush_; }
  void setActiveOutlineBrush(const QBrush &b) { activeOutlineBrush_ = b; }

  const QBrush &disabledOutlineBrush() const { return disabledOutlineBrush_; }
  void setDisabledOutlineBrush(const QBrush &b) { disabledOutlineBrush_ = b; }

  //---

  const Qt::FillRule &fillRule() const { return fillRule_; }
  void setFillRule(const Qt::FillRule &v) { fillRule_ = v; }

  //---

  const QFont &font() const { return font_; }
  void setFont(const QFont &f) { font_ = f; }

  QString fontFamily() const { return font_.family(); }
  void setFontFamily(const QString &s) { font_.setFamily(s); }

  int fontSize() const { return font_.pixelSize(); }
  void setFontSize(int s) { font_.setPixelSize(s); }

  const Qt::Alignment &anchor() const { return anchor_; }
  void setAnchor(const Qt::Alignment &v) { anchor_ = v; }

  //---

  virtual QRectF rect() const = 0;

  virtual bool inside(double /*x*/, double /*y*/) const = 0;

  virtual double distance(double x, double y) = 0;

  virtual void moveBy(double dx, double dy) = 0;

  virtual void scale(double /*xc*/, double /*yc*/, double /*sx*/, double /*sy*/) { }

  virtual bool getPoints(Points &) const { return false; }
  virtual bool setPoints(const Points &) { return false; }

  const QTransform &transform() const { return transform_; }
  void setTransform(const QTransform &v) { transform_ = v; }

  const QRectF &drawRect() const { return drawRect_; }
  void setDrawRect(const QRectF &r) { drawRect_ = r; }

  QPainterPath calcStrokePath(const QPainterPath &path, const QPen &pen) const;

  const OffsetData &stippleOffset() const { return  offsetData_; }
  void setStippleOffset(const OffsetData &offsetData) { offsetData_ = offsetData; }

  const OffsetData &stippleOutlineOffset() { return outlineOffsetData_; }
  void setStippleOutlineOffset(const OffsetData &offsetData) { outlineOffsetData_ = offsetData; }

  static bool stringToOffsetData(CTclApp *app, const QString &str, OffsetData &offsetData);
  static QString offsetDataToString(const OffsetData &offsetData);

 protected:
  CTkAppCanvasWidget* canvas_ { nullptr };

  ShapeType type_ { ShapeType::NONE };
  int       id_   { 0 };

  Tags tags_;

  QPen pen_;
  QPen activePen_;
  QPen disabledPen_;
  QPen fillOverPen_;

  QBrush  brush_;
  QString stipple_;
  QBrush  activeBrush_;
  QBrush  disabledBrush_;

  QBrush  outlineBrush_;
  QString outlineStipple_;
  QBrush  activeOutlineBrush_;
  QBrush  disabledOutlineBrush_;

  QFont font_;

  Qt::Alignment anchor_ { Qt::AlignCenter };
  QTransform    transform_;

  Qt::FillRule fillRule_ { Qt::OddEvenFill };

  OffsetData offsetData_;
  OffsetData outlineOffsetData_;

  bool enabled_ { true };
  bool visible_ { true };

  mutable QRectF drawRect_;
};

class CTkAppCanvasRectangleShape : public CTkAppCanvasShape {
  Q_OBJECT

  Q_PROPERTY(double x1 READ x1 WRITE setX1)
  Q_PROPERTY(double y1 READ y1 WRITE setY1)
  Q_PROPERTY(double x2 READ x2 WRITE setX2)
  Q_PROPERTY(double y2 READ y2 WRITE setY2)

 public:
  explicit CTkAppCanvasRectangleShape(CTkAppCanvasWidget *canvas, double x1, double y1,
                                      double x2, double y2) :
   CTkAppCanvasShape(canvas, ShapeType::RECTANGLE), x1_(x1), y1_(y1), x2_(x2), y2_(y2) {
    updatePaths();
  }

  //---

  double x1() const { return x1_; } void setX1(double x1) { x1_ = x1; updatePaths(); }
  double y1() const { return y1_; } void setY1(double y1) { y1_ = y1; updatePaths(); }
  double x2() const { return x2_; } void setX2(double x2) { x2_ = x2; updatePaths(); }
  double y2() const { return y2_; } void setY2(double y2) { y2_ = y2; updatePaths(); }

  //---

  bool inside(double x, double y) const override {
    return rect().contains(QPointF(x, y));
  }

  double distance(double x, double y) override {
    return std::min({ Point(x1_, y1_).distance(x, y),
                      Point(x1_, y2_).distance(x, y),
                      Point(x2_, y1_).distance(x, y),
                      Point(x2_, y2_).distance(x, y)});
  }

  void moveBy(double dx, double dy) override {
    x1_ += dx; y1_ += dy;
    x2_ += dx; y2_ += dy;

    updatePaths();
  }

  QRectF rect() const override { return drawRect(); }

  const QPainterPath &path() const { return path_; }

  const QPainterPath &strokePath() const { return strokePath_; }

 private:
  void updatePaths() {
    auto rect = calcRect();

    path_ = QPainterPath();
    path_.addRect(rect);

    strokePath_ = calcStrokePath(path_, pen());

    setDrawRect(path_.boundingRect().united(strokePath_.boundingRect()));
  }

  QRectF calcRect() const { return QRectF(x1_, y1_, x2_ - x1_, y2_ - y1_); }

 protected:
  double x1_ { 0.0 };
  double y1_ { 0.0 };
  double x2_ { 1.0 };
  double y2_ { 1.0 };

  QPainterPath path_;
  QPainterPath strokePath_;
};

class CTkAppCanvasOvalShape : public CTkAppCanvasShape {
  Q_OBJECT

  Q_PROPERTY(double x1 READ x1 WRITE setX1)
  Q_PROPERTY(double y1 READ y1 WRITE setY1)
  Q_PROPERTY(double x2 READ x2 WRITE setX2)
  Q_PROPERTY(double y2 READ y2 WRITE setY2)

 public:
  explicit CTkAppCanvasOvalShape(CTkAppCanvasWidget *canvas, double x1, double y1,
                                 double x2, double y2) :
   CTkAppCanvasShape(canvas, ShapeType::OVAL), x1_(x1), y1_(y1), x2_(x2), y2_(y2) {
    updatePaths();
  }

  double x1() const { return x1_; } void setX1(double x1) { x1_ = x1; updatePaths(); }
  double y1() const { return y1_; } void setY1(double y1) { y1_ = y1; updatePaths(); }
  double x2() const { return x2_; } void setX2(double x2) { x2_ = x2; updatePaths(); }
  double y2() const { return y2_; } void setY2(double y2) { y2_ = y2; updatePaths(); }

  bool inside(double x, double y) const override {
    return rect().contains(QPointF(x, y));
  }

  double distance(double x, double y) override {
    return std::min({ Point(x1_, y1_).distance(x, y),
                      Point(x1_, y2_).distance(x, y),
                      Point(x2_, y1_).distance(x, y),
                      Point(x2_, y2_).distance(x, y)});
  }

  void moveBy(double dx, double dy) override {
    x1_ += dx; y1_ += dy;
    x2_ += dx; y2_ += dy;

    updatePaths();
  }

  QRectF rect() const override { return drawRect(); }

  const QPainterPath &path() const { return path_; }

  const QPainterPath &strokePath() const { return strokePath_; }

 private:
  void updatePaths() {
    auto rect = calcRect();

    path_ = QPainterPath();
    path_.addEllipse(rect);

    strokePath_ = calcStrokePath(path_, pen());
  }

  QRectF calcRect() const { return QRectF(x1_, y1_, x2_ - x1_, y2_ - y1_); }

 protected:
  double x1_ { 0.0 };
  double y1_ { 0.0 };
  double x2_ { 1.0 };
  double y2_ { 1.0 };

  QPainterPath path_;
  QPainterPath strokePath_;
};

class CTkAppCanvasLineShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  enum class ArrowType {
    NONE,
    FIRST,
    LAST,
    BOTH
  };

 public:
  explicit CTkAppCanvasLineShape(CTkAppCanvasWidget *canvas, const Points &points) :
   CTkAppCanvasShape(canvas, ShapeType::LINE), points_(points) {
    updatePath();
  }

  const Points &points() const { return points_; }

  bool inside(double x, double y) const override {
    return qStrokePath_.contains(QPointF(x, y));
  }

  QRectF rect() const override {
    QPainterPath path;
    uint i = 0;
    for (auto &p : points_) {
      if (i == 0)
        path.moveTo(p.x, p.y);
      else
        path.lineTo(p.x, p.y);
      ++i;
    }
    return path.boundingRect();
  }

  double distance(double x, double y) override {
    double dist = 1E50;

    for (auto &p : points_)
      dist = std::min(dist, p.distance(x, y));

    return dist;
  }

  void moveBy(double dx, double dy) override {
    for (auto &p : points_)
      p.moveBy(dx, dy);

    updatePath();
  }

  bool getPoints(Points &points) const override {
    points = points_;

    return true;
  }

  bool setPoints(const Points &points) override {
    points_ = points;

    updatePath();

    return true;
  }

  const ArrowType &arrowType() const { return arrowType_; }
  void setArrowType(const ArrowType &t) { arrowType_ = t; updatePath(); }

  const QPainterPath &qpath() const { return qpath_; }

  const QPainterPath &getQStrokePath() const { return qStrokePath_; }
  void setQStrokePath(const QPainterPath &v) { qStrokePath_ = v; }

 protected:
  void updatePath() {
    qpath_.clear();
    auto n = points_.size();
    for (uint i = 0; i < n; ++i) {
      const auto &point = points_[i];

      if (i == 0)
        qpath_.moveTo(point.x, point.y);
      else
        qpath_.lineTo(point.x, point.y);
    }
  }

 protected:
  Points       points_;
  ArrowType    arrowType_ { ArrowType::NONE };
  QPainterPath qpath_;
  QPainterPath qStrokePath_;
};

#ifdef CTK_APP_TKPATH
class CTkAppCanvasPathShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  explicit CTkAppCanvasPathShape(CTkAppCanvasWidget *canvas, const QString &pathStr,
                                 const QPainterPath &qpath) :
   CTkAppCanvasShape(canvas, ShapeType::PATH), pathStr_(pathStr), qpath_(qpath) {
  }

  const QString &pathStr() const { return pathStr_; }

  QRectF rect() const override {
    return qpath_.boundingRect();
  }

  bool inside(double x, double y) const override {
    return qpath_.contains(QPointF(x, y)) || qStrokePath_.contains(QPointF(x, y));
  }

  double distance(double, double) override {
    return 1E50;
  }

  void moveBy(double dx, double dy) override {
    qpath_.translate(dx, dy);
  }

  bool getPoints(Points &) const override {
    return false;
  }

  bool setPoints(const Points &) override {
    return false;
  }

  const QPainterPath &qpath() const { return qpath_; }

  const QPainterPath &getQStrokePath() const { return qStrokePath_; }
  void setQStrokePath(const QPainterPath &v) { qStrokePath_ = v; }

 protected:
  QString      pathStr_;
  QPainterPath qpath_;
  QPainterPath qStrokePath_;
};
#endif

class CTkAppCanvasPolygonShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  explicit CTkAppCanvasPolygonShape(CTkAppCanvasWidget *canvas, const Points &points) :
   CTkAppCanvasShape(canvas, ShapeType::POLYGON), points_(points) {
    updatePath();
  }

  const Points &points() const { return points_; }

  QRectF rect() const override {
    return qpath_.boundingRect();
  }

  bool inside(double x, double y) const override {
    return qpath_.contains(QPointF(x, y));
  }

  double distance(double x, double y) override {
    double dist = 1E50;

    for (auto &p : points_)
      dist = std::min(dist, p.distance(x, y));

    return dist;
  }

  void moveBy(double dx, double dy) override {
    for (auto &p : points_)
      p.moveBy(dx, dy);

    updatePath();
  }

  bool getPoints(Points &points) const override {
    points = points_;

    return true;
  }

  bool setPoints(const Points &points) override {
    points_ = points;

    updatePath();

    return true;
  }

  const QPainterPath &qpath() const { return qpath_; }

 protected:
  void updatePath() {
    qpath_.clear();

    auto n = points_.size();

    for (uint i = 0; i < n; ++i) {
      const auto &p = points_[i];

      if (i == 0)
        qpath_.moveTo(p.x, p.y);
      else
        qpath_.lineTo(p.x, p.y);
    }

    qpath_.closeSubpath();
  }

 protected:
  Points       points_;
  QPainterPath qpath_;
};

class CTkAppCanvasArcShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  enum class ArcType {
    NONE,
    PIE,
    CHORD,
    ARC
  };

 public:
  explicit CTkAppCanvasArcShape(CTkAppCanvasWidget *canvas, const Points &points) :
   CTkAppCanvasShape(canvas, ShapeType::ARC), points_(points) {
    updatePath();
  }

  const Points &points() const { return points_; }

  int start() const { return start_; }
  void setStart(int i) { start_ = i; updatePath(); }

  int extent() const { return extent_; }
  void setExtent(int i) { extent_ = i; updatePath(); }

  const ArcType &arcType() const { return arcType_; }
  void setArcType(const ArcType &t) { arcType_ = t; updatePath(); }

  QRectF rect() const override {
    return qpath_.boundingRect();
  }

  bool inside(double x, double y) const override {
    return qpath_.contains(QPointF(x, y));
  }

  double distance(double x, double y) override {
    double dist = 1E50;

    for (auto &p : points_)
      dist = std::min(dist, p.distance(x, y));

    return dist;
  }

  void moveBy(double dx, double dy) override {
    for (auto &p : points_)
      p.moveBy(dx, dy);
  }

  bool getPoints(Points &points) const override {
    points = points_;

    return true;
  }

  bool setPoints(const Points &points) override {
    points_ = points;

    updatePath();

    return true;
  }

  const QPainterPath &qpath() const { return qpath_; }

 protected:
  void updatePath() {
    qpath_.clear();
    assert(points_.size() == 2);

    QRectF r(points_[0].x, points_[0].y,
             points_[1].x - points_[0].x, points_[1].y - points_[0].y);

    if      (arcType_ == ArcType::PIE) {
      qpath_.moveTo(r.center());
      qpath_.arcTo(r, start(), extent());
      qpath_.closeSubpath();
    }
    else if (arcType_ == ArcType::CHORD) {
      qpath_.arcMoveTo(r, start());
      qpath_.arcTo(r, start(), extent());
      qpath_.closeSubpath();
    }
    else {
      qpath_.arcMoveTo(r, start());
      qpath_.arcTo(r, start(), extent());
    }
  }

 protected:
  Points       points_;
  int          start_   { 0 };
  int          extent_  { 360 };
  ArcType      arcType_ { ArcType::ARC };
  QPainterPath qpath_;
};

#ifdef CTK_APP_TKPATH
class CTkAppCanvasCircleShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  explicit CTkAppCanvasCircleShape(CTkAppCanvasWidget *canvas, double xc, double yc) :
   CTkAppCanvasShape(canvas, ShapeType::CIRCLE), xc_(xc), yc_(yc) {
    updatePath();
  }

  double radius() const { return r_; }
  void setRadius(double r) { r_ = r; updatePath(); }

  QRectF rect() const override { return QRectF(xc_ - r_, yc_ - r_, 2*r_, 2*r_); }

  bool inside(double x, double y) const override {
    return qpath_.contains(QPointF(x, y));
  }

  double distance(double x, double y) override {
    double dist = std::max(std::hypot(x - xc_, y - yc_) - r_, 0.0);

    return dist;
  }

  void moveBy(double dx, double dy) override {
    xc_ += dx;
    yc_ += dy;

    updatePath();
  }

  bool getPoints(Points &) const override {
    return false;
  }

  bool setPoints(const Points &) override {
    return false;
  }

  const QPainterPath &qpath() const { return qpath_; }

 protected:
  void updatePath() {
    qpath_.clear();
    qpath_.addEllipse(rect());
  }

 protected:
  double       xc_ { 0.0 };
  double       yc_ { 0.0 };
  double       r_  { 1.0 };
  QPainterPath qpath_;
};

class CTkAppCanvasEllipseShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  explicit CTkAppCanvasEllipseShape(CTkAppCanvasWidget *canvas, double xc, double yc) :
   CTkAppCanvasShape(canvas, ShapeType::ELLIPSE), xc_(xc), yc_(yc) {
    updatePath();
  }

  double radiusX() const { return xr_; }
  void setRadiusX(double r) { xr_ = r; updatePath(); }

  double radiusY() const { return yr_; }
  void setRadiusY(double r) { yr_ = r; updatePath(); }

  QRectF rect() const override { return QRectF(xc_ - xr_, yc_ - yr_, 2*xr_, 2*yr_); }

  bool inside(double x, double y) const override {
    return qpath_.contains(QPointF(x, y));
  }

  double distance(double x, double y) override {
    double dist = std::max(std::hypot(x - xc_, y - yc_) - xr_, 0.0);

    return dist;
  }

  void moveBy(double dx, double dy) override {
    xc_ += dx;
    yc_ += dy;

    updatePath();
  }

  bool getPoints(Points &) const override {
    return false;
  }

  bool setPoints(const Points &) override {
    return false;
  }

  const QPainterPath &qpath() const { return qpath_; }

 protected:
  void updatePath() {
    qpath_.clear();
    qpath_.addEllipse(rect());
  }

 protected:
  double       xc_ { 0.0 };
  double       yc_ { 0.0 };
  double       xr_ { 1.0 };
  double       yr_ { 1.0 };
  QPainterPath qpath_;
};
#endif

class CTkAppCanvasTextShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  explicit CTkAppCanvasTextShape(CTkAppCanvasWidget *canvas, const Point &p, const QString &text) :
   CTkAppCanvasShape(canvas, ShapeType::TEXT), p_(p), text_(text) {
  }

  const Point &pos() const { return p_; }

  const QString &getText() const { return text_; }
  void setText(const QString &s) { text_ = s; }

  Qt::Alignment textAnchor() const { return textAnchor_; }
  void setTextAnchor(const Qt::Alignment &textAnchor) { textAnchor_ = textAnchor; }

  const QFont &font() const { return font_; }
  void setFont(const QFont &f) { font_ = f; }

  const Qt::Alignment &justify() const { return justify_; }
  void setJustify(const Qt::Alignment &v) { justify_ = v; }

  QRectF rect() const override {
    return QRectF(); // TODO
  }

  bool inside(double /*x*/, double /*y*/) const override { return false; }

  double distance(double x, double y) override {
    return p_.distance(x, y);
  }

  void moveBy(double dx, double dy) override {
    p_.moveBy(dx, dy);
  }

 protected:
  Point         p_;
  QString       text_;
  QFont         font_;
  Qt::Alignment justify_    { Qt::AlignLeft | Qt::AlignTop };
  Qt::Alignment textAnchor_ { Qt::AlignLeft };
};

class CTkAppCanvasImageShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  explicit CTkAppCanvasImageShape(CTkAppCanvasWidget *canvas, const Point &p,
                                  const CTkAppImageRef &image) :
   CTkAppCanvasShape(canvas, ShapeType::IMAGE), p_(p), image_(image) {
  }

  const Point &pos() const { return p_; }

  const CTkAppImageRef &getImage() const { return image_; }

  void setImage(const CTkAppImageRef &i) { image_ = i; }

  QRectF rect() const override { return drawRect(); }

  bool inside(double x, double y) const override {
    return drawRect().contains(QPointF(x, y));
  }

  double distance(double x, double y) override {
    return p_.distance(x, y);
  }

  void moveBy(double dx, double dy) override {
    p_.moveBy(dx, dy);
  }

 protected:
  Point          p_;
  CTkAppImageRef image_;
};

class CTkAppCanvasBitmapShape : public CTkAppCanvasShape {
  Q_OBJECT

  Q_PROPERTY(QColor background READ background WRITE setBackground)
  Q_PROPERTY(QColor foreground READ foreground WRITE setForeground)

  Q_PROPERTY(QColor activeBackground READ activeBackground WRITE setActiveBackground)
  Q_PROPERTY(QColor activeForeground READ activeForeground WRITE setActiveForeground)

  Q_PROPERTY(QColor disabledBackground READ disabledBackground WRITE setDisabledBackground)
  Q_PROPERTY(QColor disabledForeground READ disabledForeground WRITE setDisabledForeground)

 public:
  explicit CTkAppCanvasBitmapShape(CTkAppCanvasWidget *canvas, const Point &p,
                                   const CTkAppImageRef &image) :
   CTkAppCanvasShape(canvas, ShapeType::BITMAP), p_(p), image_(image) {
  }

  const Point &pos() const { return p_; }

  //---

  const CTkAppImageRef &getImage() const { return image_; }
  void setImage(const CTkAppImageRef &i) { image_ = i; }

  const CTkAppImageRef &getActiveImage() const { return activeImage_; }
  void setActiveImage(const CTkAppImageRef &i) { activeImage_ = i; }

  const CTkAppImageRef &getDisabledImage() const { return disabledImage_; }
  void setDisabledImage(const CTkAppImageRef &i) { disabledImage_ = i; }

  //---

  const QColor &background() const { return background_; }
  void setBackground(const QColor &c) { background_ = c; }

  const QColor &foreground() const { return foreground_; }
  void setForeground(const QColor &c) { foreground_ = c; }

  const QColor &activeBackground() const { return activeBackground_; }
  void setActiveBackground(const QColor &c) { activeBackground_ = c; }

  const QColor &activeForeground() const { return activeForeground_; }
  void setActiveForeground(const QColor &c) { activeForeground_ = c; }

  const QColor &disabledBackground() const { return disabledBackground_; }
  void setDisabledBackground(const QColor &c) { disabledBackground_ = c; }

  const QColor &disabledForeground() const { return disabledForeground_; }
  void setDisabledForeground(const QColor &c) { disabledForeground_ = c; }

  //---

  QRectF rect() const override { return drawRect(); }

  bool inside(double /*x*/, double /*y*/) const override { return false; }

  double distance(double x, double y) override {
    return p_.distance(x, y);
  }

  void moveBy(double dx, double dy) override {
    p_.moveBy(dx, dy);
  }

 protected:
  Point p_;

  QColor background_;
  QColor foreground_;
  QColor activeBackground_;
  QColor activeForeground_;
  QColor disabledBackground_;
  QColor disabledForeground_;

  CTkAppImageRef image_;
  CTkAppImageRef activeImage_;
  CTkAppImageRef disabledImage_;
};

class CTkAppCanvasWidget : public QWidget {
  Q_OBJECT

 public:
  using Canvas = CTkAppCanvasWidget;
  using Shape  = CTkAppCanvasShape;
  using Shapes = std::vector<Shape *>;
  using Point  = CTkAppPoint;
  using Points = CTkAppPoints;

  using Rectangle = CTkAppCanvasRectangleShape;
  using Oval      = CTkAppCanvasOvalShape;
  using Line      = CTkAppCanvasLineShape;
#ifdef CTK_APP_TKPATH
  using Path      = CTkAppCanvasPathShape;
#endif
  using Polygon   = CTkAppCanvasPolygonShape;
  using Arc       = CTkAppCanvasArcShape;
#ifdef CTK_APP_TKPATH
  using Circle    = CTkAppCanvasCircleShape;
  using Ellipse   = CTkAppCanvasEllipseShape;
#endif
  using Text      = CTkAppCanvasTextShape;
  using Image     = CTkAppCanvasImageShape;
  using Bitmap    = CTkAppCanvasBitmapShape;

 public:
  CTkAppCanvasWidget(CTkAppCanvas *canvas);

  //---

  CTkAppCanvas *canvas() const { return canvas_; }

  const Shapes &getShapes() const { return shapes_; }

  //---

  double closeEnough() const { return closeEnough_; }
  void setCloseEnough(double r) { closeEnough_ = r; }

  bool isConfine() const { return confine_; }
  void setConfine(bool b) { confine_ = b; }

  const QRectF &scrollRegion() const { return scrollRegion_; }
  void setScrollRegion(const QRectF &r) { scrollRegion_ = r; }

  double xScrollIncrement() const { return xScrollIncrement_; }
  void setXScrollIncrement(double r) { xScrollIncrement_ = r; }

  double yScrollIncrement() const { return yScrollIncrement_; }
  void setYScrollIncrement(double r) { yScrollIncrement_ = r; }

  //---

  Rectangle* addRectangle(double x1, double y1, double x2, double y2);
  Oval*      addOval     (double x1, double y1, double x2, double y2);
  Line*      addLine     (const Points &points);
#ifdef CTK_APP_TKPATH
  Path*      addPath     (const QString &pathStr, const QPainterPath &qpath);
#endif
  Polygon*   addPolygon  (const Points &points);
  Arc*       addArc      (const Points &points);
#ifdef CTK_APP_TKPATH
  Circle*    addCircle   (double xc, double yc);
  Ellipse*   addEllipse  (double xc, double yc);
#endif
  Text*      addText     (const Point &pos, const QString &text);
  Image*     addImage    (const Point &pos, const CTkAppImageRef &image=CTkAppImageRef());
  Bitmap*    addBitmap   (const Point &pos, const CTkAppImageRef &image=CTkAppImageRef());

  //---

  QRectF getShapeBBox(const QString &name) const {
    QRectF bbox;

    Shapes shapes;

    if (! getShapes(name, shapes))
      return bbox;

    bool bboxSet = false;

    for (auto *shape : shapes) {
      if (! bboxSet) {
        bbox    = shape->rect();
        bboxSet = true;
      }
      else {
        bbox = bbox.united(shape->rect());
      }
    }

    return bbox;
  }

  void deleteAllShapes() {
    for (auto *shape : shapes_)
      delete shape;

    shapes_.clear();
  }

  bool deleteShape(const QString &name) {
    Shapes shapes;

    if (! getShapes(name, shapes))
      return true;

    for (auto *shape : shapes)
      deleteShape(shape);

    return true;
  }

  bool deleteShape(Shape *shape) {
    uint i;

    if (! shapeInd(shape, i))
      return false;

    delete shapes_[i];

    auto n = shapes_.size();

    ++i;

    while (i < n) {
      shapes_[i - 1] = shapes_[i];

      ++i;
    }

    shapes_.pop_back();

    return true;
  }

  bool lowerShape(const QString &name) {
    Shapes shapes;

    if (! getShapes(name, shapes))
      return true;

    for (auto *shape : shapes)
      lowerShape(shape);

    return true;
  }

  bool lowerShape(Shape *shape) {
    uint i;

    if (! shapeInd(shape, i))
      return false;

    if (i == 0)
      return true;

    std::swap(shapes_[i], shapes_[i - 1]);

    return true;
  }

  bool raiseShape(const QString &name) {
    Shapes shapes;

    if (! getShapes(name, shapes))
      return true;

    for (auto *shape : shapes)
      raiseShape(shape);

    return true;
  }

  bool raiseShape(Shape *shape) {
    uint i;

    if (! shapeInd(shape, i))
      return false;

    auto n = shapes_.size();

    if (i == n - 1)
      return true;

    std::swap(shapes_[i], shapes_[i + 1]);

    return true;
  }

  bool moveShapeBy(const QString &name, double dx, double dy) {
    Shapes shapes;

    if (! getShapes(name, shapes))
      return true;

    for (auto *shape : shapes)
      shape->moveBy(dx, dy);

    return true;
  }

  bool moveShapeTo(const QString &name, double x, double y) {
    Shapes shapes;

    if (! getShapes(name, shapes))
      return true;

    auto r = getShapesRect(shapes);

    auto dx = x - r.left();
    auto dy = y - r.top();

    for (auto *shape : shapes)
      shape->moveBy(dx, dy);

    return true;
  }

  bool scaleShape(const QString &name, double xc, double yc, double sx, double sy) {
    Shapes shapes;

    if (! getShapes(name, shapes))
      return true;

    for (auto *shape : shapes)
      shape->scale(xc, yc, sx, sy);

    return true;
  }

  bool getShapePoints(const QString &name, Points &points) const {
    bool ok;
    auto id = name.toInt(&ok);
    if (! ok) return false;

    uint i;

    if (! shapeInd(id, i))
      return false;

    if (! shapes_[i]->getPoints(points))
      return false;

    return true;
  }

  bool setShapePoints(const QString &name, const Points &points) {
    Shapes shapes;

    if (! getShapes(name, shapes))
      return true;

    for (auto *shape : shapes) {
      if (! shape->setPoints(points))
        return false;
    }

    return true;
  }

  bool getShapes(const QString &name, Shapes &shapes) const {
    bool ok;
    auto id = name.toInt(&ok);

    if (ok) {
      for (auto *shape : shapes_) {
        if (shape->id() == id)
          shapes.push_back(shape);
      }
    }
    else {
      for (auto *shape : shapes_) {
        for (const auto &tag : shape->tags()) {
          if (tag == name) {
            shapes.push_back(shape);
            break;
          }
        }
      }
    }

    return ! shapes.empty();
  }

  QRectF getShapesRect(const Shapes &shapes) const {
    QRectF r;

    uint i = 0;
    for (auto *shape : shapes) {
      if (i == 0)
        r = shape->rect();
      else
        r = shape->rect().united(r);
      ++i;
    }
    return r;
  }

  void getShapes(Shapes &shapes) const {
    for (auto *shape : shapes_)
      shapes.push_back(shape);
  }

  Shape *insideShape(double x, double y) {
    for (auto *shape : shapes_) {
      if (! shape->isEnabled())
        continue;

      if (shape->inside(x, y))
        return shape;
    }

    return nullptr;
  }

  int findNearest(double x, double y) {
    int    ind  = -1;
    double dist = -1;

    for (auto *shape : shapes_) {
      auto dist1 = shape->distance(x, y);

      if (ind < 0 || dist1 < dist) {
        ind  = shape->id();
        dist = dist1;
      }
    }

    return ind;
  }

  bool shapeInd(const Shape *shape, uint &ind) const {
    uint i = 0;
    auto n = shapes_.size();

    while (i < n) {
      if (shapes_[i] == shape) {
        ind = i;
        return true;
      }

      ++i;
    }

    return false;
  }

  bool shapeInd(int id, uint &ind) const {
    uint i = 0;
    auto n = shapes_.size();

    while (i < n) {
      if (shapes_[i]->id() == id) {
        ind = i;
        return true;
      }

      ++i;
    }

    return false;
  }

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  QBrush calcBrush(Shape *s) const;

  QPen calcPen(Shape *s) const;

  QSize sizeHint() const override;

 private:
  Shape *addShape(Shape *s) { shapes_.push_back(s); return s; }

  void drawShape(QPainter *p, Shape *s);

 private:
  Shapes        shapes_;
  double        closeEnough_      { 1.0 };
  double        confine_          { true };
  QRectF        scrollRegion_;
  double        xScrollIncrement_ { 0.0 };
  double        yScrollIncrement_ { 0.0 };
  CTkAppCanvas* canvas_           { nullptr };
};

//---

class CTkAppCheckButtonVarProc;

class CTkAppCheckButtonWidget;

class CTkAppCheckButton : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName       READ varName         WRITE setVarName)
  Q_PROPERTY(QString onValue       READ onValueStr      WRITE setOnValueStr)
  Q_PROPERTY(QString offValue      READ offValueStr     WRITE setOffValueStr)
  Q_PROPERTY(bool    showIndicator READ isShowIndicator WRITE setShowIndicator)
  Q_PROPERTY(bool    overRaised    READ isOverRaised    WRITE setOverRaised)
  Q_PROPERTY(int     underlinePos  READ underlinePos    WRITE setUnderlinePos)

 public:
  using OptString = std::optional<QString>;

 public:
  explicit CTkAppCheckButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "CheckButton"; }

  //---

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  const OptString &onValue() const { return onValue_; }
  void setOnValue(const OptString &s) { onValue_ = s; }

  QString onValueStr() const { return onValue().value_or(""); }
  void setOnValueStr(const QString &s) { setOnValue(s); }

  const OptString &offValue() const { return offValue_; }
  void setOffValue(const OptString &s) { offValue_ = s; }

  QString offValueStr() const { return offValue().value_or(""); }
  void setOffValueStr(const QString &s) { setOffValue(s); }

  bool isShowIndicator() const { return showIndicator_; }
  void setShowIndicator(bool b) { showIndicator_ = b; }

  bool isOverRaised() const { return overRaised_; }
  void setOverRaised(bool b) { overRaised_ = b; }

  int underlinePos() const { return underlinePos_; }
  void setUnderlinePos(int i) { underlinePos_ = i; }

  //---

  void setText(const QString &text) override;

  void setChecked(bool b);

  void updateFromVar();
  void updateToVar();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void stateChangedSlot(int);

 private:
  CTkAppCheckButtonWidget*  qcheck_  { nullptr };
  QString                   varName_;
  CTkAppCheckButtonVarProc* varProc_ { nullptr };
  OptString                 onValue_;
  OptString                 offValue_;
  QColor                    selectColor_;
  bool                      showIndicator_ { true };
  bool                      overRaised_ { false };
  int                       underlinePos_ { -1 };
};

class CTkAppCheckButtonWidget : public QCheckBox {
  Q_OBJECT

 public:
  explicit CTkAppCheckButtonWidget(CTkAppCheckButton *check);

  const QString &getLabel() const { return text_; }
  void setLabel(const QString &s) { text_ = s; updateText(); }

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; }

  double wrapLength() const { return wrapLength_; }
  void setWrapLength(double r) { wrapLength_ = r; updateText(); }

 private:
  void updateText();

 private:
  CTkAppCheckButton *check_ { nullptr };

  QString text_;
  bool    readOnly_   { false };
  double  wrapLength_ { -1 };
};

//---

class CTkAppComboBoxWidget;
class CTkAppComboBoxVarProc;

class CTkAppComboBox : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppComboBox(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "ComboBox"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void updateFromVar();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void indexChangedSlot(int);

 private:
  CTkAppComboBoxWidget*  qcombo_ { nullptr };
  QString                varName_;
  CTkAppComboBoxVarProc* varProc_ { nullptr };
};

class CTkAppComboBoxWidget : public QComboBox {
  Q_OBJECT

 public:
  CTkAppComboBoxWidget(CTkAppComboBox *combo);

  int width() const { return width_; }
  void setWidth(int i) { width_ = i; }

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; }

  QSize sizeHint() const override;

 private:
  CTkAppComboBox* combo_    { nullptr };
  int             width_    { -1 };
  bool            readOnly_ { false };
};

//---

class CTkAppEntryWidget;
class CTkAppEntryVarProc;
class CTkAppEntryValidator;

class CTkAppEntry : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppEntry(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Entry"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void setText(const QString &text) override;

  bool validate(const QString &) const;

  void updateFromVar();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void valueChangedSlot();

 private:
  enum class ValidateMode {
    NONE,
    FOCUS,
    FOCUSIN,
    FOCUSOUT,
    KEY,
    ALL
  };

  CTkAppEntryWidget*    qedit_ { nullptr };
  QString               varName_;
  CTkAppEntryVarProc*   varProc_ { nullptr };
  ValidateMode          validateMode_ { ValidateMode::NONE };
  QString               validateCmd_;
  CTkAppEntryValidator* validateProc_ { nullptr };
};

class CTkAppEntryWidget : public QLineEdit {
  Q_OBJECT

 public:
  CTkAppEntryWidget(CTkAppEntry *entry);

  int width() const { return width_; }
  void setWidth(int i) { width_ = i; }

  QSize sizeHint() const override;

 private:
  CTkAppEntry* entry_ { nullptr };
  int          width_ { -1 };
};

//---

class CTkAppFrameWidget;

class CTkAppFrame : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppFrame(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Frame"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

 private:
  CTkAppFrameWidget* qframe_ { nullptr };
};

class CTkAppFrameWidget : public QFrame {
 public:
  explicit CTkAppFrameWidget(CTkAppFrame *frame);

  QSize sizeHint() const override;

 private:
  CTkAppFrame *frame_;
};

//---

class CTkAppLabelWidget;
class CTkAppLabelVarProc;

class CTkAppLabel : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppLabel(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Label"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void setText(const QString &text) override;

  void setImageRef(const CTkAppImageRef &i) override;

  void setJustify(const Qt::Alignment &a) override;

  void updateFromVar();

 private:
  CTkAppLabelWidget*  qlabel_ { nullptr };
  QString             varName_;
  CTkAppLabelVarProc* varProc_ { nullptr };
};

class CTkAppLabelWidget : public CQLabelImage {
  Q_OBJECT

 public:
  using OptReal = std::optional<double>;

 public:
  explicit CTkAppLabelWidget(CTkAppLabel *label);

  const QString &getLabel() const { return text_; }
  void setLabel(const QString &s) { text_ = s; updateText(); }

  const OptReal &width() const { return width_; }
  void setWidth(const OptReal &w) { width_ = w; updateText(); }

  const OptReal &height() const { return height_; }
  void setHeight(const OptReal &h) { height_ = h; updateText(); }

  double wrapLength() const { return wrapLength_; }
  void setWrapLength(double r) { wrapLength_ = r; updateText(); }

  QSize sizeHint() const override;

 private:
  void updateText();

 private:
  CTkAppLabel* label_ { nullptr };

  QString text_;

  OptReal width_;
  OptReal height_;

  double wrapLength_ { -1 };
};

//---

class CTkAppLabelFrame : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppLabelFrame(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "LabelFrame"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  void setText(const QString &text) override;

 private:
  QGroupBox* qframe_ { nullptr };
};

//---

class CTkAppListBoxWidget;
class CTkAppListBoxVarProc;

class CTkAppListBox : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppListBox(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "ListBox"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void updateFromVar();

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void selectionSlot();

  void vscrollSlot(int value);
  void hscrollSlot(int value);

 private:
  CTkAppListBoxWidget*  qlist_ { nullptr };
  QString               varName_;
  CTkAppListBoxVarProc* varProc_ { nullptr };
  bool                  exportSelection_ { true };
};

class CTkAppListBoxWidget : public QListWidget {
  Q_OBJECT

 public:
  using OptReal = std::optional<double>;

 public:
  CTkAppListBoxWidget(CTkAppListBox *listbox);

  const OptReal &width() const { return width_; }
  void setWidth(const OptReal &w);

  const OptReal &height() const { return height_; }
  void setHeight(const OptReal &h);

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; }

  QSize sizeHint() const override;

 private:
  CTkAppListBox* listbox_ { nullptr };

  OptReal width_;
  OptReal height_;
  bool    readOnly_ { false };
};

//---

class CTkAppCheckAction : public QWidgetAction {
  Q_OBJECT

  Q_PROPERTY(QString text    READ text    WRITE setText)
  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppCheckAction(CTkApp* tk) :
   QWidgetAction(nullptr), tk_(tk) {
  }

  QWidget *createWidget(QWidget *parent) override {
    check_ = new QCheckBox(parent);

    if (getText() != "")
      check_->setText(getText());

    connect(check_, SIGNAL(stateChanged(int)), this, SIGNAL(stateChanged(int)));

    return check_;
  }

  //---

  const QString &getText() const { return text_; }
  void setText(const QString &s) { text_ = s; }

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void updateFromVar();

 Q_SIGNALS:
  void stateChanged(int);

 private:
  CTkApp*    tk_ { nullptr };
  QCheckBox* check_ { nullptr };
  QString    text_;
  QString    varName_;
};

class CTkAppRadioAction : public QWidgetAction {
  Q_OBJECT

  Q_PROPERTY(QString text    READ text    WRITE setText)
  Q_PROPERTY(QString value   READ value   WRITE setValue)
  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppRadioAction(CTkApp* tk) :
   QWidgetAction(nullptr), tk_(tk) {
  }

  QWidget *createWidget(QWidget *parent) override {
    radio_ = new QRadioButton(parent);

    if (getText() != "")
      radio_->setText(getText());

    connect(radio_, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));

    return radio_;
  }

  //---

  const QString &getText() const { return text_; }
  void setText(const QString &s) { text_ = s; }

  const QString &value() const { return value_; }
  void setValue(const QString &s) { value_ = s; }

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void updateFromVar();

 Q_SIGNALS:
  void toggled(bool);

 private:
  CTkApp*       tk_ { nullptr };
  QRadioButton* radio_ { nullptr };
  QString       text_;
  QString       value_;
  QString       varName_;
};

class CTkAppSubMenu : public QMenu {
  Q_OBJECT

 public:
  explicit CTkAppSubMenu(CTkApp* tk);

  const QString &menu() const { return menu_; }
  void setMenu(const QString &s) { menu_ = s; }

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void showSlot();
  void hideSlot();

 private:
  CTkApp* tk_ { nullptr };
  QString menu_;
};

class CTkAppMenu : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppMenu(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Menu"; }

  QMenu *qmenu() { return qmenu_; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void actionPressedSlot();

  void checkChangedSlot(int);

  void radioToggledSlot(bool);

 private:
  QMenu* qmenu_   { nullptr };
  bool   tearOff_ { true };
};

//---

class CTkAppMenuButtonWidget;
class CTkAppMenuButtonVarProc;

class CTkAppMenuButton : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName       READ varName         WRITE setVarName)
  Q_PROPERTY(bool    showIndicator READ isShowIndicator WRITE setShowIndicator)

 public:
  explicit CTkAppMenuButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "MenuButton"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  bool isShowIndicator() const { return showIndicator_; }
  void setShowIndicator(bool b) { showIndicator_ = b; }

  //---

  void setText(const QString &text) override;

  void setImageRef(const CTkAppImageRef &i) override;

  void updateMenu();

  void setMenu(CTkAppMenu *menu);

  void updateFromVar();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void clickSlot();

 private:
  CTkAppMenuButtonWidget*  qbutton_ { nullptr };
  QString                  menuName_;
  QString                  varName_;
  CTkAppMenuButtonVarProc* varProc_ { nullptr };
  bool                     showIndicator_ { true };
};

class CTkAppMenuButtonWidget : public QToolButton {
  Q_OBJECT

 public:
  explicit CTkAppMenuButtonWidget(CTkAppMenuButton *button);

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; }

 private:
  CTkAppMenuButton *button_   { nullptr };
  bool              readOnly_ { false };
};

//---

class CTkAppMessageVarProc;

class CTkAppMessage : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppMessage(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Message"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void setText(const QString &text) override;

  void updateFromVar();

 private:
  QTextEdit*            qedit_ { nullptr };
  QString               varName_;
  CTkAppMessageVarProc* varProc_ { nullptr };
};

//---

class CTkAppNoteBookWidget;

class CTkAppNoteBook : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppNoteBook(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "NoteBook"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

 private:
  QTabWidget* qtab_ { nullptr };
};

class CTkAppNoteBookWidget : public QTabWidget {
  Q_OBJECT

 public:
  CTkAppNoteBookWidget(CTkAppNoteBook *notebook);

 private:
  CTkAppNoteBook *notebook_ { nullptr };
};

//---

class CTkAppPanedWindow : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppPanedWindow(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "PanedWindow"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

 private:
  QSplitter* qpane_ { nullptr };
};

//---

class CTkAppRadioButtonWidget;
class CTkAppRadioButtonVarProc;

class CTkAppRadioButton : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName       READ varName         WRITE setVarName)
  Q_PROPERTY(QString value         READ value           WRITE setValue)
  Q_PROPERTY(QColor  selectColor   READ selectColor     WRITE setSelectColor)
  Q_PROPERTY(bool    showIndicator READ isShowIndicator WRITE setShowIndicator)
  Q_PROPERTY(double  userWidth     READ userWidth       WRITE setUserWidth)
  Q_PROPERTY(double  userHeight    READ userHeight      WRITE setUserHeight)

 public:
  struct RadioGroupData {
    QButtonGroup*                 group { nullptr };
    std::set<CTkAppRadioButton *> buttons;
  };

  using RadioGroups = std::map<QString, RadioGroupData>;

 public:
  static void addRadioGroup   (CTkAppRadioButton *radio, const QString &varName);
  static void removeRadioGroup(CTkAppRadioButton *radio, const QString &varName);

 public:
  explicit CTkAppRadioButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");
 ~CTkAppRadioButton();

  const char *getClassName() const override { return "RadioButton"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  CTkAppRadioButtonWidget *qradio() const { return qradio_; }

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s);

  const QString &value() const { return value_; }
  void setValue(const QString &value);

  const QColor &selectColor() const { return selectColor_; }
  void setSelectColor(const QColor &c) { selectColor_ = c; }

  bool isShowIndicator() const { return showIndicator_; }
  void setShowIndicator(bool b) { showIndicator_ = b; }

  int userWidth() const { return userWidth_; }
  void setUserWidth(int i) { userWidth_ = i; }

  int userHeight() const { return userHeight_; }
  void setUserHeight(int i) { userHeight_ = i; }

  //---

  void setText(const QString &text) override;

  void setImageRef(const CTkAppImageRef &i) override;

  void setChecked(bool b);

  void updateFromVar();
  void updateToVar();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void toggleSlot(bool);

 private:
  static RadioGroups s_radioGroups;

  CTkAppRadioButtonWidget*  qradio_  { nullptr };
  QString                   varName_;
  QString                   value_   { "1" };
  CTkAppRadioButtonVarProc* varProc_ { nullptr };
  QColor                    selectColor_;
  bool                      showIndicator_ { true };
  double                    userWidth_ { -1 };
  double                    userHeight_ { -1 };
};

class CTkAppRadioButtonWidget : public QRadioButton {
  Q_OBJECT

 public:
  explicit CTkAppRadioButtonWidget(CTkAppRadioButton *radio);

  const QString &getText() const { return text_; }
  void setText(const QString &s) { text_ = s; updateText(); }

  //! get/set image
  const QImage &image() const { return image_; }
  void setImage(const QImage &i) { image_ = i; update(); }

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; }

  double wrapLength() const { return wrapLength_; }
  void setWrapLength(double r) { wrapLength_ = r; updateText(); }

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private:
  void updateText();

 private:
  CTkAppRadioButton *radio_ { nullptr };

  QString text_;
  QImage  image_;
  bool    readOnly_ { false };

  double wrapLength_ { -1 };
};

//---

class CTkAppScaleVarProc;

class CTkAppScale : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppScale(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Scale"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void updateFromVar();
  void updateToVar();

 private:
  void connectSlots(bool);

  void updateSize();

 private Q_SLOTS:
  void valueSlot(int);

 private:
  CQRealSlider*       qscale_  { nullptr };
  QString             varName_;
  CTkAppScaleVarProc* varProc_ { nullptr };
  int                 length_  { 0 };
  int                 width_   { 0 };
};

//---

class CTkAppScrollBar;

class CTkAppScrollBarWidget : public QScrollBar {
  Q_OBJECT

 public:
  explicit CTkAppScrollBarWidget(CTkAppScrollBar *scrollbar);

  const QColor &troughColor() const { return troughColor_; }
  void setTroughColor(const QColor &c) { troughColor_ = c; }

 private:
  CTkAppScrollBar* scrollbar_ { nullptr };
  QColor           troughColor_;
};

class CTkAppScrollBar : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppScrollBar(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "ScrollBar"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void scrollSlot(int);

 private:
  CTkAppScrollBarWidget* qscrollbar_ { nullptr };
};

//---

class CTkAppSpinBoxWidget;
class CTkAppSpinBoxVarProc;
class CTkAppSpinBoxValidator;

class CTkAppSpinBox : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName        READ varName           WRITE setVarName)
  Q_PROPERTY(QString invalidCommand READ getInvalidCommand WRITE setInvalidCommand)

 public:
  explicit CTkAppSpinBox(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "SpinBox"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  const QString &getInvalidCommand() const { return invalidCommand_; }
  void setInvalidCommand(const QString &command) { invalidCommand_ = command; }

  //---

  void setText(const QString &text) override;

  bool validate(const QString &) const;

  void updateFromVar();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void valueChangedSlot();

 private:
  enum class ValidateMode {
    NONE,
    FOCUS,
    FOCUSIN,
    FOCUSOUT,
    KEY,
    ALL
  };

  CTkAppSpinBoxWidget*    qspin_ { nullptr };
  QString                 varName_;
  CTkAppSpinBoxVarProc*   varProc_;
  ValidateMode            validateMode_ { ValidateMode::NONE };
  QString                 validateCmd_;
  CTkAppSpinBoxValidator* validateProc_ { nullptr };
  QString                 invalidCommand_;
};

//---

class CTkAppTextWidget;

class CTkAppText : public CTkAppWidget {
  Q_OBJECT

 public:
  using TextInd = CTkAppTextInd;

  struct TextIndRange {
    TextIndRange() { }

    TextIndRange(const TextInd &start1, const TextInd &end1) :
     start(start1), end(end1) {
    }

    TextInd start;
    TextInd end;

    QString toString() const {
      return start.toString() + " " + end.toString();
    }
  };

  using TextIndRanges = std::vector<TextIndRange>;

  struct TagData {
    QColor        foreground;
    QColor        background;
    QFont         font;
    double        borderWidth { 0.0 };
    bool          underline { false };
    TextIndRanges indRanges;
  };

 public:
  explicit CTkAppText(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Text"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  void setCurrentInd(const TextInd &ind,
                     const QTextCursor::MoveMode &modeMode=QTextCursor::MoveAnchor) const;
  void setCurrentInd(QTextCursor &cursor, const TextInd &ind,
                     const QTextCursor::MoveMode &modeMode=QTextCursor::MoveAnchor) const;

  void getCurrentInd(TextInd &ind) const;
  void getCurrentInd(QTextCursor &cursor, TextInd &ind) const;

  bool stringToTextInd(const QVariant &str, TextInd &ind) const;

  void setMark(const QString &mark, const TextInd &ind);
  bool getMark(const QString &mark, TextInd &ind) const;
  void getMarkNames(std::vector<QString> &names) const;

  bool getMarkGravity(const QString &name, QString &gravity) const;
  bool setMarkGravity(const QString &name, const QString &gravity);

  TextIndRange remapIndRange(const TextIndRange &ind) const;
  TextInd remapInd(const TextInd &ind) const;

  void setText(const QString &text) override;

  void lowerTag(const QString &tag, const QString &aboveTag);
  void raiseTag(const QString &tag, const QString &aboveTag);

  void applyTag(const TagData &tagData);

  void deleteTag(const QString &tag);

  void addTagRanges(const QString &tag, const TextIndRanges &indRanges);

  void getTagRanges(const QString &tag, TextIndRanges &indRanges) const;

  void tagRemove(const QString &tag);

  void getTagNames(std::vector<QString> &names) const;

  TagData &tagData(const QString &name);

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void vscrollSlot(int value);
  void hscrollSlot(int value);

 private:
  struct MarkData {
    TextInd textInd;
    QString gravity { "right" };

    MarkData() { }

    explicit MarkData(const TextInd &ind) :
     textInd(ind) {
    }
  };

  using Marks = std::map<QString, MarkData>;
  using Tags  = std::map<QString, TagData>;

  CTkAppTextWidget* qtext_ { nullptr };
  Marks             marks_;
  Tags              tags_;
};

//---

class CTkAppTopLevelWidget;

class CTkAppTopLevel : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString iconWindow READ iconWindow WRITE setIconWindow)

 public:
  explicit CTkAppTopLevel(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

 ~CTkAppTopLevel();

  const char *getClassName() const override { return "TopLevel"; }

  bool isTopLevel() const override { return true; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  void setTopLevelWidget(CTkAppTopLevelWidget *qtoplevel);

  bool isNeedsShow() const { return needsShow_; }
  void setNeedsShow(bool b) { needsShow_ = b; }

  const QString &iconWindow() const { return iconWindow_; }
  void setIconWindow(const QString &s) { iconWindow_ = s; }

  void show() override;
  void hide() override;

 private:
  CTkAppTopLevelWidget* qtoplevel_ { nullptr };
  bool                  needsShow_ { false };
  QString               iconWindow_;
};

class CTkAppTopLevelWidget : public QFrame {
  Q_OBJECT

 public:
  explicit CTkAppTopLevelWidget(CTkAppTopLevel *toplevel);

  QSize sizeHint() const override;

 private:
  CTkAppTopLevel* toplevel_ { nullptr };
};

//---

class CTkAppTreeViewWidget;

class CTkAppTreeView : public CTkAppWidget {
  Q_OBJECT

 public:
  class Index {
   public:
    static Index root() { Index ind; ind.root_ = true; return ind; }

    Index() { }

   ~Index() { delete parent_; }

    Index(const Index &i) {
      root_   = i.root_;
      row_    = i.row_;
      col_    = i.col_;
      parent_ = (i.parent_ ? new Index(*i.parent_) : nullptr);
    }

    Index &operator=(const Index &i) {
      delete parent_;

      root_   = i.root_;
      row_    = i.row_;
      col_    = i.col_;
      parent_ = (i.parent_ ? new Index(*i.parent_) : nullptr);

      return *this;
    }

    bool operator==(const Index &rhs) const {
      if (root_ && rhs.root_)
        return true;

      if (row_ != rhs.row_ || col_ != rhs.col_)
        return false;

      if (! parent_ || ! rhs.parent_)
        return (parent_ == rhs.parent_);

      return (*parent_ == *rhs.parent_);
    }

    bool isRoot() const { return root_; }
    void setRoot(bool b) { root_ = b; }

    int row() const { return row_; }
    void setRow(int i) { row_ = i; }

    int col() const { return col_; }
    void setCol(int i) { col_ = i; }

    bool hasParent() const { return parent_; }

    Index parent() const { return *parent_; }

    void setParent(const Index &ind) {
      parent_ = new Index(ind);
    }

   private:
    bool   root_   { false };
    int    row_    { -1 };
    int    col_    { -1 };
    Index *parent_ { nullptr };
  };

 public:
  explicit CTkAppTreeView(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "TreeView"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

 private:
  void connectSlots(bool b);

  void setIdIndex(const QString &id, const Index &ind);
  bool getIdIndex(const QString &id, Index &ind) const;
  bool getIndexId(const Index &ind, QString &id) const;

 private Q_SLOTS:
  void selectionSlot();

 private:
  using Ids = std::map<QString, Index>;

  CTkAppTreeViewWidget* qtree_ { nullptr };
  Ids                   ids_;
};

class CTkAppTreeViewWidget : public QTreeWidget {
  Q_OBJECT

 public:
  using OptReal = std::optional<double>;

 public:
  CTkAppTreeViewWidget(CTkAppTreeView *listbox);

  const OptReal &height() const { return height_; }
  void setHeight(const OptReal &h);

  QSize sizeHint() const override;

 private:
  CTkAppTreeView* treeview_ { nullptr };

  OptReal height_;
};

#endif
