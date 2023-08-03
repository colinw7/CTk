#ifndef CTkAppWidget_H
#define CTkAppWidget_H

#include <CTkAppEventData.h>
#include <CTkApp.h>

#include <CStrUtil.h>

#include <QBrush>
#include <QCheckBox>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPainterPath>
#include <QPen>
#include <QPushButton>
#include <QRadioButton>
#include <QTextEdit>
#include <QWidgetAction>

#include <optional>
#include <cmath>

class CTkApp;
class CTkAppRoot;
class CTkAppTopLevel;
class CTkAppWidgetCommand;

class CTkAppPackLayout;
class CTkAppGridLayout;
class CTkAppPlaceLayout;

class CQRealSlider;
class CQSpinList;
class CQLabelImage;

class QBoxLayout;
class QFrame;
class QGroupBox;
class QMenuBar;
class QScrollBar;
class QSplitter;
class QTextCursor;
class QTextEdit;
class QToolButton;

//---

class CTkAppWidgetEventFilter;

class CTkAppWidget : public QObject {
  Q_OBJECT

 public:
  using Args = std::vector<std::string>;

 public:
  explicit CTkAppWidget(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  virtual ~CTkAppWidget();

  //---

  CTkAppWidget *getParent() const { return parent_; }

  QWidget *parentWidget() const { return parent_->qwidget(); }
  void setParentWidget(QWidget *w);

  CTkAppRoot *root() const;

  QWidget *qwidget() const { return qwidget_; }

  CTkAppTopLevel *toplevel() const;

  virtual bool isTopLevel() { return false; }

  //---

  virtual const char *getClassName() const = 0;

  const std::string &getName() const { return name_; }

  std::string getFullName() const;

  //---

  void setQWidget(QWidget *w);
  QWidget *getQWidget() const;

  const CTkAppWidgetCommand *getWidgetCommand() const { return widgetCommand_; }
  void setWidgetCommand(CTkAppWidgetCommand *p) { widgetCommand_ = p; }

  //---

  double highlightThickness() const { return highlightThickness_; }
  void setHighlightThickness(double r) { highlightThickness_ = r; }

  //---

  int getWidth () const;
  int getHeight() const;

  void setWidth (int w);
  void setHeight(int h);

  QSize sizeHint() const;

  QSize minimumSizeHint() const;

  //---

  bool isDeleted() const { return deleted_; }

  //---

  void addChild   (CTkAppWidget *w);
  void removeChild(CTkAppWidget *w);

  void getChildren(std::vector<CTkAppWidget *> &children) const;

  CTkAppWidget *getChild(const std::string &name) const;

  //---

  bool isInitNotify() const { return initNotify_; }
  void setInitNotify(bool b) { initNotify_ = b; }

  //---

  void setFocus();

  //---

  void lower();
  void raise();

  //---

  CTkAppPackLayout  *getTkPackLayout(bool create=true);
  CTkAppGridLayout  *getTkGridLayout(bool create=true);
  CTkAppPlaceLayout *getTkPlaceLayout(bool create=true);

  //---

  // handle name/value change (configure)
  virtual bool execConfig(const std::string &name, const std::string &value);

  // handle widget operation (<widget> <operation>)
  virtual bool execOp(const Args &);

  virtual void show();

  //---

  std::string getTitle() const;
  void setTitle(const std::string &title);

  std::string getIcon() const;
  void setIcon(const std::string &title);

  std::string getGeometry() const;
  bool setGeometry(const std::string &title);

  //---

  void setPadX(int i);
  void setPadY(int i);

  //--

  void setWidgetAnchor(const std::string &value);
  void setBorderWidth(int width);
  void setSelectBorderWidth(int width);

  const std::string &getOptionClass() const { return optionClass_; }
  void setOptionClass(const std::string &s) { optionClass_ = s; }

  //---

  const Qt::Alignment &anchor() const { return anchor_; }
  void setAnchor(const Qt::Alignment &v) { anchor_ = v; }

  //---

  bool bindEvent(const CTkAppEventData &data);

  void getBindings(const CTkAppEventData &data, std::vector<std::string> &bindings);

  virtual bool triggerEvents(QEvent *e, const CTkAppEventData &matchEventData);

  virtual bool triggerMousePressEvents  (QEvent *e, int button);
  virtual bool triggerMouseMoveEvents   (QEvent *e, int button, bool pressed);
  virtual bool triggerMouseReleaseEvents(QEvent *e, int button);

  virtual bool triggerKeyPressEvents(QEvent *e);

  //---

  void deleteLater();

  bool getOptionValue(const std::string &optName, const std::string &optClass,
                      std::string &optValue) const;

  //---

  void processEvents(QEvent *e, const CTkAppEventData &matchEventData);

 protected:
  const std::string &getCommand() const { return command_; }
  void setCommand(const std::string &command) { command_ = command; }

  void runCommand(const Args &args=Args());

 protected:
  using WidgetMap  = std::map<std::string, CTkAppWidget *>;
  using EventDatas = std::vector<CTkAppEventData>;
//using EventMap   = std::map<std::string, std::string>;

  CTkApp*       tk_     { nullptr };
  CTkAppWidget* parent_ { nullptr };
  std::string   name_;

  CTkAppWidgetCommand *widgetCommand_ { nullptr };

  int           w_       { 0 };
  int           h_       { 0 };
  bool          deleted_ { false };
  QWidget*      qwidget_ { nullptr };
  WidgetMap     children_;
  EventDatas    eventDatas_;
//EventMap      events_;

  std::string xScrollCommand_;
  std::string yScrollCommand_;

  Qt::Alignment anchor_ { Qt::AlignCenter };

  std::string command_;

  std::string optionClass_;

  double highlightThickness_ { -1 };

  QColor selectBackground_;
  QColor selectForeground_;

  bool gridSize_ { false }; // is size based on number of grid cells (chars)

  std::string cursor_;

  bool initNotify_ { false };

  CTkAppCompoundType compoundType_ { CTkAppCompoundType::NONE };

  CTkAppWidgetEventFilter *eventFilter_ { nullptr };
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

  bool isTopLevel() override { return true; }

  void show() override;

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  bool decodeWidgetName(const std::string &name, CTkAppWidget **parent,
                        std::string &childName) const;

 private:
  QFrame*     qroot_ { nullptr };
  std::string menuName_;
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

 public:
  explicit CTkAppButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Button"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setImage(const CTkAppImageRef &image);

  void flash();

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void clickSlot();

 private:
  CTkAppButtonWidget* qbutton_ { nullptr };
};

class CTkAppButtonWidget : public QPushButton {
  Q_OBJECT

 public:
  explicit CTkAppButtonWidget(CTkAppButton *button);

  const QString &text() const { return text_; }
  void setText(const QString &s);

  double width() const { return width_; }
  void setWidth(double r) { width_ = r; }

  double height() const { return height_; }
  void setHeight(double r) { height_ = r; }

  double wrapLength() const { return wrapLength_; }
  void setWrapLength(double r);

 private:
  void updateText();

 private:
  CTkAppButton *button_     { nullptr };
  double        width_      { -1 };
  double        height_     { -1 };
  double        wrapLength_ { -1 };
  QString       text_;
};

//---

class CTkAppCanvasWidget;
class CTkAppCanvasShape;

class CTkAppCanvas : public CTkAppWidget {
  Q_OBJECT

 public:
  using Shape = CTkAppCanvasShape;

  explicit CTkAppCanvas(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Canvas"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  bool triggerMousePressEvents(QEvent *e, int button) override;
  bool triggerMouseMoveEvents (QEvent *e, int button, bool pressed) override;

  double width() const { return width_; }
  void setWidth(double r) { width_ = r; }

  double height() const { return height_; }
  void setHeight(double r) { height_ = r; }

  void processShapeEvents(Shape *shape, QEvent *w, const CTkAppEventData &matchEventData);

  //---

  auto newGradientName() const {
    return "gradient" + std::to_string(gradients_.size() + 1);
  }

  void addGradient(const std::string &name, QGradient *g) {
    gradients_[name] = g;
  }

  bool getGradient(const std::string &name, QGradient* &g) {
    auto pg = gradients_.find(name);
    if (pg == gradients_.end()) return false;

    g = (*pg).second;

    return true;
  }

 private:
  CTkAppCanvasWidget* qcanvas_ { nullptr };

  using IdEventDatas = std::map<std::string, EventDatas>;
  using Gradients    = std::map<std::string, QGradient *>;

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

  void move(double dx, double dy) {
    x += dx;
    y += dy;
  }
};

using CTkAppPoints = std::vector<CTkAppPoint>;

class CTkAppCanvasShape {
 public:
  using ShapeType = CTkAppCanvasShapeType;
  using Tags      = std::vector<std::string>;
  using Points    = CTkAppPoints;

 public:
  explicit CTkAppCanvasShape(ShapeType type) :
   type_(type) {
    static int lastId = 0;

    id_ = ++lastId;
  }

  virtual ~CTkAppCanvasShape() = default;

  ShapeType type() const { return type_; }

  int id() const { return id_; }

  const Tags &tags() const { return tags_; }
  void setTags(const Tags &v) { tags_ = v; }

  const QPen &pen() const { return pen_; }
  void setPen(const QPen &p) { pen_ = p; }

  const QBrush &brush() const { return brush_; }
  void setBrush(const QBrush &b) { brush_ = b; }

  const QFont &font() const { return font_; }
  void setFont(const QFont &f) { font_ = f; }

  std::string fontFamily() const { return font_.family().toStdString(); }
  void setFontFamily(const std::string &s) { font_.setFamily(QString::fromStdString(s)); }

  int fontSize() const { return font_.pixelSize(); }
  void setFontSize(int s) { font_.setPixelSize(s); }

  const Qt::Alignment &anchor() const { return anchor_; }
  void setAnchor(const Qt::Alignment &v) { anchor_ = v; }

  virtual bool inside(double /*x*/, double /*y*/) const = 0;

  virtual double distance(double x, double y) = 0;

  virtual void move(double dx, double dy) = 0;

  virtual void scale(double /*xc*/, double /*yc*/, double /*sx*/, double /*sy*/) { }

  virtual bool getPoints(Points &) const { return false; }
  virtual bool setPoints(const Points &) { return false; }

  const QTransform &transform() const { return transform_; }
  void setTransform(const QTransform &v) { transform_ = v; }

 protected:
  ShapeType     type_  { ShapeType::NONE };
  int           id_    { 0 };
  Tags          tags_;
  QPen          pen_;
  QBrush        brush_;
  QFont         font_;
  Qt::Alignment anchor_ { Qt::AlignCenter };
  QTransform    transform_;
};

class CTkAppCanvasWidget : public QWidget {
  Q_OBJECT

 public:
  using Shape  = CTkAppCanvasShape;
  using Shapes = std::vector<Shape *>;
  using Point  = CTkAppPoint;
  using Points = CTkAppPoints;

  enum class ArcType {
    NONE,
    PIE,
    CHORD,
    ARC
  };

  enum class ArrowType {
    NONE,
    FIRST,
    LAST,
    BOTH
  };

  class Rectangle : public Shape {
   public:
    explicit Rectangle(double x1, double y1, double x2, double y2) :
     Shape(ShapeType::RECTANGLE), x1_(x1), y1_(y1), x2_(x2), y2_(y2) {
    }

    double x1() const { return x1_; }
    double y1() const { return y1_; }
    double x2() const { return x2_; }
    double y2() const { return y2_; }

    bool inside(double x, double y) const override {
      return rect().contains(QPointF(x, y));
    }

    double distance(double x, double y) override {
      return std::min({ Point(x1_, y1_).distance(x, y),
                        Point(x1_, y2_).distance(x, y),
                        Point(x2_, y1_).distance(x, y),
                        Point(x2_, y2_).distance(x, y)});
    }

    void move(double dx, double dy) override {
      x1_ += dx; y1_ += dy;
      x2_ += dx; y2_ += dy;
    }

   protected:
    QRectF rect() const { return QRectF(x1_, y1_, x2_ - x1_, y2_ - y1_); }

   protected:
    double x1_ { 0.0 };
    double y1_ { 0.0 };
    double x2_ { 1.0 };
    double y2_ { 1.0 };
  };

  class Oval : public Shape {
   public:
    explicit Oval(double x1, double y1, double x2, double y2) :
     Shape(ShapeType::OVAL), x1_(x1), y1_(y1), x2_(x2), y2_(y2) {
    }

    double x1() const { return x1_; }
    double y1() const { return y1_; }
    double x2() const { return x2_; }
    double y2() const { return y2_; }

    bool inside(double x, double y) const override {
      return rect().contains(QPointF(x, y));
    }

    double distance(double x, double y) override {
      return std::min({ Point(x1_, y1_).distance(x, y),
                        Point(x1_, y2_).distance(x, y),
                        Point(x2_, y1_).distance(x, y),
                        Point(x2_, y2_).distance(x, y)});
    }

    void move(double dx, double dy) override {
      x1_ += dx; y1_ += dy;
      x2_ += dx; y2_ += dy;
    }

   protected:
    QRectF rect() const { return QRectF(x1_, y1_, x2_ - x1_, y2_ - y1_); }

   protected:
    double x1_ { 0.0 };
    double y1_ { 0.0 };
    double x2_ { 1.0 };
    double y2_ { 1.0 };
  };

  class Line : public Shape {
   public:
    explicit Line(const Points &points) :
     Shape(ShapeType::LINE), points_(points) {
      updatePath();
    }

    const Points &points() const { return points_; }

    bool inside(double x, double y) const override {
      return qStrokePath_.contains(QPointF(x, y));
    }

    double distance(double x, double y) override {
      double dist = 1E50;

      for (auto &p : points_)
        dist = std::min(dist, p.distance(x, y));

      return dist;
    }

    void move(double dx, double dy) override {
      for (auto &p : points_)
        p.move(dx, dy);

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
  class Path : public Shape {
   public:
    explicit Path(const std::string &pathStr, const QPainterPath &qpath) :
     Shape(ShapeType::PATH), pathStr_(pathStr), qpath_(qpath) {
    }

    const std::string &pathStr() const { return pathStr_; }

    bool inside(double x, double y) const override {
      return qpath_.contains(QPointF(x, y)) || qStrokePath_.contains(QPointF(x, y));
    }

    double distance(double, double) override {
      return 1E50;
    }

    void move(double dx, double dy) override {
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
    std::string  pathStr_;
    QPainterPath qpath_;
    QPainterPath qStrokePath_;
  };
#endif

  class Polygon : public Shape {
   public:
    explicit Polygon(const Points &points) :
     Shape(ShapeType::POLYGON), points_(points) {
      updatePath();
    }

    const Points &points() const { return points_; }

    bool inside(double x, double y) const override {
      return qpath_.contains(QPointF(x, y));
    }

    double distance(double x, double y) override {
      double dist = 1E50;

      for (auto &p : points_)
        dist = std::min(dist, p.distance(x, y));

      return dist;
    }

    void move(double dx, double dy) override {
      for (auto &p : points_)
        p.move(dx, dy);

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

  class Arc : public Shape {
   public:
    explicit Arc(const Points &points) :
     Shape(ShapeType::ARC), points_(points) {
      updatePath();
    }

    const Points &points() const { return points_; }

    int start() const { return start_; }
    void setStart(int i) { start_ = i; updatePath(); }

    int extent() const { return extent_; }
    void setExtent(int i) { extent_ = i; updatePath(); }

    const ArcType &arcType() const { return arcType_; }
    void setArcType(const ArcType &t) { arcType_ = t; updatePath(); }

    bool inside(double x, double y) const override {
      return qpath_.contains(QPointF(x, y));
    }

    double distance(double x, double y) override {
      double dist = 1E50;

      for (auto &p : points_)
        dist = std::min(dist, p.distance(x, y));

      return dist;
    }

    void move(double dx, double dy) override {
      for (auto &p : points_)
        p.move(dx, dy);
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
      qpath_.arcMoveTo(r, start());
      qpath_.arcTo(r, start(), extent());
    }

   protected:
    Points       points_;
    int          start_   { 0 };
    int          extent_  { 360 };
    ArcType      arcType_ { ArcType::ARC };
    QPainterPath qpath_;
  };

#ifdef CTK_APP_TKPATH
  class Circle : public Shape {
   public:
    explicit Circle(double xc, double yc) :
     Shape(ShapeType::CIRCLE), xc_(xc), yc_(yc) {
      updatePath();
    }

    double radius() const { return r_; }
    void setRadius(double r) { r_ = r; updatePath(); }

    QRectF rect() const { return QRectF(xc_ - r_, yc_ - r_, 2*r_, 2*r_); }

    bool inside(double x, double y) const override {
      return qpath_.contains(QPointF(x, y));
    }

    double distance(double x, double y) override {
      double dist = std::max(std::hypot(x - xc_, y - yc_) - r_, 0.0);

      return dist;
    }

    void move(double dx, double dy) override {
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

  class Ellipse : public Shape {
   public:
    explicit Ellipse(double xc, double yc) :
     Shape(ShapeType::ELLIPSE), xc_(xc), yc_(yc) {
      updatePath();
    }

    double radiusX() const { return xr_; }
    void setRadiusX(double r) { xr_ = r; updatePath(); }

    double radiusY() const { return yr_; }
    void setRadiusY(double r) { yr_ = r; updatePath(); }

    QRectF rect() const { return QRectF(xc_ - xr_, yc_ - yr_, 2*xr_, 2*yr_); }

    bool inside(double x, double y) const override {
      return qpath_.contains(QPointF(x, y));
    }

    double distance(double x, double y) override {
      double dist = std::max(std::hypot(x - xc_, y - yc_) - xr_, 0.0);

      return dist;
    }

    void move(double dx, double dy) override {
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

  class Text : public Shape {
   public:
    explicit Text(const Point &p, const std::string &text) :
     Shape(ShapeType::TEXT), p_(p), text_(text) {
    }

    const Point &pos() const { return p_; }

    const std::string &text() const { return text_; }
    void setText(const std::string &s) { text_ = s; }

    Qt::Alignment textAnchor() const { return textAnchor_; }
    void setTextAnchor(const Qt::Alignment &textAnchor) { textAnchor_ = textAnchor; }

    const QFont &font() const { return font_; }
    void setFont(const QFont &f) { font_ = f; }

    const Qt::Alignment &justify() const { return justify_; }
    void setJustify(const Qt::Alignment &v) { justify_ = v; }

    bool inside(double /*x*/, double /*y*/) const override { return false; }

    double distance(double x, double y) override {
      return p_.distance(x, y);
    }

    void move(double dx, double dy) override {
      p_.move(dx, dy);
    }

   protected:
    Point         p_;
    std::string   text_;
    QFont         font_;
    Qt::Alignment justify_    { Qt::AlignLeft|Qt::AlignTop };
    Qt::Alignment textAnchor_ { Qt::AlignLeft };
  };

  class Image : public Shape {
   public:
    explicit Image(const Point &p, const CTkAppImageRef &image) :
     Shape(ShapeType::IMAGE), p_(p), image_(image) {
    }

    const Point &pos() const { return p_; }

    const CTkAppImageRef &getImage() const { return image_; }
    void setImage(const CTkAppImageRef &i) { image_ = i; }

    bool inside(double /*x*/, double /*y*/) const override { return false; }

    double distance(double x, double y) override {
      return p_.distance(x, y);
    }

    void move(double dx, double dy) override {
      p_.move(dx, dy);
    }

   protected:
    Point          p_;
    CTkAppImageRef image_;
  };

  class Bitmap : public Shape {
   public:
    explicit Bitmap(const Point &p, const CTkAppImageRef &image) :
     Shape(ShapeType::BITMAP), p_(p), image_(image) {
    }

    const Point &pos() const { return p_; }

    const CTkAppImageRef &getImage() const { return image_; }
    void setImage(const CTkAppImageRef &i) { image_ = i; }

    bool inside(double /*x*/, double /*y*/) const override { return false; }

    double distance(double x, double y) override {
      return p_.distance(x, y);
    }

    void move(double dx, double dy) override {
      p_.move(dx, dy);
    }

   protected:
    Point          p_;
    CTkAppImageRef image_;
  };

 public:
  CTkAppCanvasWidget(CTkAppCanvas *canvas);

  Rectangle *addRectangle(double x1, double y1, double x2, double y2) {
    auto *rectangleShape = new Rectangle(x1, y1, x2, y2);

    return static_cast<Rectangle *>(addShape(rectangleShape));
  }

  Oval *addOval(double x1, double y1, double x2, double y2) {
    auto *ovalShape = new Oval(x1, y1, x2, y2);

    return static_cast<Oval *>(addShape(ovalShape));
  }

  Line *addLine(const Points &points) {
    auto *lineShape = new Line(points);

    return static_cast<Line *>(addShape(lineShape));
  }

#ifdef CTK_APP_TKPATH
  Path *addPath(const std::string &pathStr, const QPainterPath &qpath) {
    auto *pathShape = new Path(pathStr, qpath);

    return static_cast<Path *>(addShape(pathShape));
  }
#endif

  Polygon *addPolygon(const Points &points) {
    auto *polygonShape = new Polygon(points);

    return static_cast<Polygon *>(addShape(polygonShape));
  }

  Arc *addArc(const Points &points) {
    auto *arcShape = new Arc(points);

    return static_cast<Arc *>(addShape(arcShape));
  }

#ifdef CTK_APP_TKPATH
  Circle *addCircle(double xc, double yc) {
    auto *circleShape = new Circle(xc, yc);

    return static_cast<Circle *>(addShape(circleShape));
  }

  Ellipse *addEllipse(double xc, double yc) {
    auto *ellipseShape = new Ellipse(xc, yc);

    return static_cast<Ellipse *>(addShape(ellipseShape));
  }
#endif

  Text *addText(const Point &pos, const std::string &text) {
    auto *textShape = new Text(pos, text);

    return static_cast<Text *>(addShape(textShape));
  }

  Image *addImage(const Point &pos, const CTkAppImageRef &image=CTkAppImageRef()) {
    auto *imageShape = new Image(pos, image);

    return static_cast<Image *>(addShape(imageShape));
  }

  Bitmap *addBitmap(const Point &pos, const CTkAppImageRef &image=CTkAppImageRef()) {
    auto *bitmapShape = new Bitmap(pos, image);

    return static_cast<Bitmap *>(addShape(bitmapShape));
  }

  void deleteAllShapes() {
    for (auto *shape : shapes_)
      delete shape;

    shapes_.clear();
  }

  bool deleteShape(const std::string &name) {
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

  bool lowerShape(const std::string &name) {
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

  bool raiseShape(const std::string &name) {
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

  bool moveShape(const std::string &name, double dx, double dy) {
    Shapes shapes;

    if (! getShapes(name, shapes))
      return true;

    for (auto *shape : shapes)
      shape->move(dx, dy);

    return true;
  }

  bool scaleShape(const std::string &name, double xc, double yc, double sx, double sy) {
    Shapes shapes;

    if (! getShapes(name, shapes))
      return true;

    for (auto *shape : shapes)
      shape->scale(xc, yc, sx, sy);

    return true;
  }

  bool getShapePoints(const std::string &name, Points &points) const {
    int id;

    if (! CStrUtil::toInteger(name, &id))
      return false;

    uint i;

    if (! shapeInd(id, i))
      return false;

    if (! shapes_[i]->getPoints(points))
      return false;

    return true;
  }

  bool setShapePoints(const std::string &name, const Points &points) {
    Shapes shapes;

    if (! getShapes(name, shapes))
      return true;

    for (auto *shape : shapes) {
      if (! shape->setPoints(points))
        return false;
    }

    return true;
  }

  bool getShapes(const std::string &name, Shapes &shapes) const {
    int id;

    if (CStrUtil::toInteger(name, &id)) {
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

  void getShapes(Shapes &shapes) const {
    for (auto *shape : shapes_)
      shapes.push_back(shape);
  }

  Shape *insideShape(double x, double y) {
    for (auto *shape : shapes_) {
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

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private:
  Shape *addShape(Shape *s) { shapes_.push_back(s); return s; }

  void drawShape(QPainter *p, Shape *s);

 private:
  Shapes        shapes_;
  CTkAppCanvas* canvas_ { nullptr };
};

//---

class CTkAppCheckButtonVarProc;

class CTkAppCheckButtonWidget;

class CTkAppCheckButton : public CTkAppWidget {
  Q_OBJECT

 public:
  using OptString = std::optional<std::string>;

 public:
  explicit CTkAppCheckButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "CheckButton"; }

  const OptString &onValue() const { return onValue_; }
  void setOnValue(const OptString &s) { onValue_ = s; }

  const OptString &offValue() const { return offValue_; }
  void setOffValue(const OptString &s) { offValue_ = s; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void setChecked(bool b);

  void updateFromVar();
  void updateToVar();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void stateChangedSlot(int);

 private:
  CTkAppCheckButtonWidget*  qcheck_  { nullptr };
  std::string               varName_;
  CTkAppCheckButtonVarProc* varProc_ { nullptr };
  OptString                 onValue_;
  OptString                 offValue_;
  QColor                    selectColor_;
  bool                      showIndicator_ { true };
};

class CTkAppCheckButtonWidget : public QCheckBox {
  Q_OBJECT

 public:
  explicit CTkAppCheckButtonWidget(CTkAppCheckButton *check);

 private:
  CTkAppCheckButton *check_ { nullptr };
};

//---

class CTkAppEntryWidget;
class CTkAppEntryVarProc;
class CTkAppEntryValidator;

class CTkAppEntry : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppEntry(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Entry"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  bool validate(const std::string &) const;

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
  std::string           varName_;
  CTkAppEntryVarProc*   varProc_ { nullptr };
  ValidateMode          validateMode_ { ValidateMode::NONE };
  std::string           validateCmd_;
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

class CTkAppFrame : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppFrame(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Frame"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  QFrame* qframe_ { nullptr };
};

//---

class CTkAppLabelVarProc;

class CTkAppLabel : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppLabel(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Label"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void setImage(const CTkAppImageRef &image);

  void updateFromVar();

 private:
  CQLabelImage*       qlabel_ { nullptr };
  std::string         varName_;
  CTkAppLabelVarProc* varProc_ { nullptr };
};

//---

class CTkAppLabelFrame : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppLabelFrame(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "LabelFrame"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

 private:
  QGroupBox* qframe_ { nullptr };
};

//---

class CTkAppListBoxVarProc;

class CTkAppListBox : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppListBox(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "ListBox"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void updateFromVar();

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void vscrollSlot(int value);
  void hscrollSlot(int value);

 private:
  QListWidget*          qlist_ { nullptr };
  std::string           varName_;
  CTkAppListBoxVarProc* varProc_ { nullptr };
  bool                  exportSelection_ { true };
};

class CTkAppListBoxWidget : public QListWidget {
  Q_OBJECT

 public:
  CTkAppListBoxWidget(CTkAppListBox *listbox);

 private:
  CTkAppListBox* listbox_ { nullptr };
};

//---

class CTkAppCheckAction : public QWidgetAction {
  Q_OBJECT

 public:
  explicit CTkAppCheckAction(CTkApp* tk) :
   QWidgetAction(nullptr), tk_(tk) {
  }

  QWidget *createWidget(QWidget *parent) override {
    check_ = new QCheckBox(parent);

    if (text() != "")
      check_->setText(QString::fromStdString(text()));

    connect(check_, SIGNAL(stateChanged(int)), this, SIGNAL(stateChanged(int)));

    return check_;
  }

  const std::string &text() const { return text_; }
  void setText(const std::string &s) { text_ = s; }

  const std::string &varName() const { return varName_; }
  void setVarName(const std::string &s) { varName_ = s; }

  void updateFromVar();

 Q_SIGNALS:
  void stateChanged(int);

 private:
  CTkApp*     tk_ { nullptr };
  QCheckBox*  check_ { nullptr };
  std::string text_;
  std::string varName_;
};

class CTkAppRadioAction : public QWidgetAction {
  Q_OBJECT

 public:
  explicit CTkAppRadioAction(CTkApp* tk) :
   QWidgetAction(nullptr), tk_(tk) {
  }

  QWidget *createWidget(QWidget *parent) override {
    radio_ = new QRadioButton(parent);

    if (text() != "")
      radio_->setText(QString::fromStdString(text()));

    connect(radio_, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));

    return radio_;
  }

  const std::string &text() const { return text_; }
  void setText(const std::string &s) { text_ = s; }

  const std::string &value() const { return value_; }
  void setValue(const std::string &s) { value_ = s; }

  const std::string &varName() const { return varName_; }
  void setVarName(const std::string &s) { varName_ = s; }

  void updateFromVar();

 Q_SIGNALS:
  void toggled(bool);

 private:
  CTkApp*       tk_ { nullptr };
  QRadioButton* radio_ { nullptr };
  std::string   text_;
  std::string   value_;
  std::string   varName_;
};

class CTkAppSubMenu : public QMenu {
  Q_OBJECT

 public:
  explicit CTkAppSubMenu(CTkApp* tk);

  const std::string &menu() const { return menu_; }
  void setMenu(const std::string &s) { menu_ = s; }

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void showSlot();
  void hideSlot();

 private:
  CTkApp*     tk_ { nullptr };
  std::string menu_;
};

class CTkAppMenu : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppMenu(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Menu"; }

  QMenu *qmenu() { return qmenu_; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void actionPressedSlot();

  void checkChangedSlot(int);

  void radioToggledSlot(bool);

 private:
  QMenu*      qmenu_ { nullptr };
  std::string command_;
};

//---

class CTkAppMenuButtonVarProc;

class CTkAppMenuButton : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppMenuButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "MenuButton"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void setImage(const CTkAppImageRef &image);

  void updateMenu();

  void setMenu(CTkAppMenu *menu);

  void updateFromVar();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void clickSlot();

 private:
  QToolButton*             qbutton_ { nullptr };
  std::string              menuName_;
  std::string              varName_;
  CTkAppMenuButtonVarProc* varProc_ { nullptr };
  bool                     showIndicator_ { true };
};

//---

class CTkAppMessageVarProc;

class CTkAppMessage : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppMessage(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Message"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void updateFromVar();

 private:
  QTextEdit*            qedit_ { nullptr };
  std::string           varName_;
  CTkAppMessageVarProc* varProc_ { nullptr };
};

//---

class CTkAppPanedWindow : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppPanedWindow(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "PanedWindow"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  QSplitter* qpane_ { nullptr };
};

//---

class CTkAppRadioButtonVarProc;
class CTkAppRadioButtonWidget;

class CTkAppRadioButton : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppRadioButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");
 ~CTkAppRadioButton();

  const char *getClassName() const override { return "RadioButton"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void setValue(const std::string &value);

  void setImage(const CTkAppImageRef &image);

  void setChecked(bool b);

  void updateFromVar();
  void updateToVar();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void toggleSlot(bool);

 private:
  CTkAppRadioButtonWidget*  qradio_  { nullptr };
  std::string               varName_;
  std::string               value_   { "1" };
  CTkAppRadioButtonVarProc* varProc_ { nullptr };
  QColor                    selectColor_;
  bool                      showIndicator_ { true };
};

class CTkAppRadioButtonWidget : public QRadioButton {
  Q_OBJECT

 public:
  explicit CTkAppRadioButtonWidget(CTkAppRadioButton *radio);

  //! get/set image
  const QImage &image() const { return image_; }
  void setImage(const QImage &i) { image_ = i; update(); }

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private:
  CTkAppRadioButton *radio_ { nullptr };
  QImage             image_;
};

//---

class CTkAppScaleVarProc;

class CTkAppScale : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppScale(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Scale"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void updateFromVar();
  void updateToVar();

 private:
  void connectSlots(bool);

  void updateSize();

 private Q_SLOTS:
  void valueSlot(int);

 private:
  CQRealSlider*       qscale_  { nullptr };
  std::string         varName_;
  CTkAppScaleVarProc* varProc_ { nullptr };
  int                 length_  { 0 };
  int                 width_   { 0 };
};

//---

class CTkAppScrollBar : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppScrollBar(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "ScrollBar"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void scrollSlot(int);

 private:
  QScrollBar* qscrollbar_ { nullptr };
};

//---

class CTkAppSpinBoxVarProc;

class CTkAppSpinBox : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppSpinBox(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "SpinBox"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setValue(const std::string &);

  void updateFromVar();

 private:
  CQSpinList*           qspin_ { nullptr };
  std::string           varName_;
  CTkAppSpinBoxVarProc* varProc_;
};

//---

class CTkAppTextWidget;

class CTkAppText : public CTkAppWidget {
  Q_OBJECT

 public:
  struct TextInd {
    enum { END = INT_MAX };

    int lineNum { -1 };
    int charNum { -1 };

    TextInd() { }

    TextInd(int l, int c) :
      lineNum(l), charNum(c) {
    }

    static TextInd end() {
      return TextInd(END, END);
    }

    static int cmp(const TextInd &ind1, const TextInd &ind2) {
      if (ind1.lineNum > ind2.lineNum) return  1;
      if (ind1.lineNum < ind2.lineNum) return -1;
      if (ind1.charNum > ind2.charNum) return  1;
      if (ind1.charNum < ind2.charNum) return -1;
      return 0;
    }

    std::string toString() const {
      return std::to_string(lineNum) + "." + std::to_string(charNum);
    }

    friend std::ostream &operator<<(std::ostream &os, const TextInd &ind) {
      os << ind.toString();
      return os;
    }
  };

  struct TextIndRange {
    TextIndRange() { }

    TextIndRange(const TextInd &start1, const TextInd &end1) :
     start(start1), end(end1) {
    }

    TextInd start;
    TextInd end;

    std::string toString() const {
      return start.toString() + " " + end.toString();
    }
  };

  using TextIndRanges = std::vector<TextIndRange>;

  struct TagData {
    QColor        foreground;
    QColor        background;
    QFont         font;
    TextIndRanges indRanges;
  };

 public:
  explicit CTkAppText(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Text"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setCurrentInd(const TextInd &ind,
                     const QTextCursor::MoveMode &modeMode=QTextCursor::MoveAnchor) const;
  void setCurrentInd(QTextCursor &cursor, const TextInd &ind,
                     const QTextCursor::MoveMode &modeMode=QTextCursor::MoveAnchor) const;

  void getCurrentInd(TextInd &ind) const;
  void getCurrentInd(QTextCursor &cursor, TextInd &ind) const;

  bool stringToTextInd(const std::string &str, TextInd &ind) const;

  void setMark(const std::string &mark, const TextInd &ind);
  bool getMark(const std::string &mark, TextInd &ind) const;
  void getMarkNames(std::vector<std::string> &names) const;

  TextIndRange remapIndRange(const TextIndRange &ind) const;
  TextInd remapInd(const TextInd &ind) const;

  void setText(const std::string &text);

  void lowerTag(const std::string &tag, const std::string &aboveTag);
  void raiseTag(const std::string &tag, const std::string &aboveTag);

  void applyTag(const TagData &tagData);

  void deleteTag(const std::string &tag);

  void addTagRanges(const std::string &tag, const TextIndRanges &indRanges);

  void getTagRanges(const std::string &tag, TextIndRanges &indRanges) const;

  void tagRemove(const std::string &tag);

  void getTagNames(std::vector<std::string> &names) const;

  TagData &tagData(const std::string &name);

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void vscrollSlot(int value);
  void hscrollSlot(int value);

 private:
  using Marks = std::map<std::string, TextInd>;
  using Tags  = std::map<std::string, TagData>;

  CTkAppTextWidget* qtext_ { nullptr };
  Marks             marks_;
  Tags              tags_;
};

class CTkAppTextWidget : public QTextEdit {
  Q_OBJECT

 public:
  CTkAppTextWidget(CTkAppText *text);

  int width() const { return width_; }
  void setWidth(int i) { width_ = i; }

  int height() const { return height_; }
  void setHeight(int i) { height_ = i; }

  QSize sizeHint() const override;

 private:
  CTkAppText *text_   { nullptr };
  int         width_  { -1 };
  int         height_ { -1 };
};

//---

class CTkAppTopLevel : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppTopLevel(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  void setFrame(QFrame *qframe);

  const char *getClassName() const override { return "TopLevel"; }

  bool isTopLevel() override { return true; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  QFrame* qframe_ { nullptr };
};

#endif
