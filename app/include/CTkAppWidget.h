#ifndef CTkAppWidget_H
#define CTkAppWidget_H

#include <CTkAppEventData.h>

#include <CImageLib.h>

#include <QFrame>
#include <QWidgetAction>
#include <QCheckBox>
#include <QRadioButton>
#include <QMenu>
#include <QLabel>
#include <QBrush>
#include <QPen>

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

class QFrame;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QLineEdit;
class QGroupBox;
class QListWidget;
class QScrollBar;
class QSplitter;
class QTextEdit;
class QToolButton;
class QBoxLayout;
class QMenuBar;

class CTkAppWidget : public QObject {
  Q_OBJECT

 public:
  using Args = std::vector<std::string>;

 public:
  CTkAppWidget(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  virtual ~CTkAppWidget();

  //---

  CTkAppWidget *getParent() const { return parent_; }

  CTkAppRoot *root() const;

  QWidget *qwidget() const { return qwidget_; }

  CTkAppTopLevel *toplevel() const;

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

  CTkAppPackLayout  *getTkPackLayout();
  CTkAppGridLayout  *getTkGridLayout();
  CTkAppPlaceLayout *getTkPlaceLayout();

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

  const std::string &getOptionClass() const { return optionClass_; }
  void setOptionClass(const std::string &s) { optionClass_ = s; }

  //---

  const Qt::Alignment &align() const { return align_; }
  void setAlign(const Qt::Alignment &v) { align_ = v; }

  //---

  bool bindEvent(const CTkAppEventData &data);

  virtual bool triggerEvents(QEvent *e, const CTkAppEventData &matchEventData);

  virtual bool triggerMousePressEvents  (QEvent *e, int button);
  virtual bool triggerMouseMoveEvents   (QEvent *e, int button, bool pressed);
  virtual bool triggerMouseReleaseEvents(QEvent *e, int button);

  virtual bool triggerKeyPressEvents(QEvent *e);

  //---

  void deleteLater();

  bool getOptionValue(const std::string &optName, const std::string &optClass,
                      std::string &optValue) const;

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

  Qt::Alignment align_ { Qt::AlignCenter };

  std::string command_;

  std::string optionClass_;

  double highlightThickness_ { -1 };

  bool initNotify_ { false };
};

//---

class CTkAppWidgetEventFilter :  public QObject {
  Q_OBJECT

 public:
  CTkAppWidgetEventFilter(CTkAppWidget *w) : w_(w) { }

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
  CTkAppRoot(CTkApp *tk);

  const char *getClassName() const override { return "Root"; }

  void show() override;

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  bool decodeWidgetName(const std::string &name, CTkAppWidget **parent,
                        std::string &childName) const;

 private:
  CTkAppRootWidget* qframe_ { nullptr };
  std::string       menuName_;
};

class CTkAppRootWidget : public QFrame {
  Q_OBJECT

 public:
  CTkAppRootWidget(CTkAppRoot *root);

  void setMenu(QMenu *menu);

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private:
  CTkAppRoot* root_    { nullptr };
  QMenu*      menu_    { nullptr };
  QMenuBar*   menuBar_ { nullptr };
};

//---

class CTkAppButton : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkAppButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Button"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void setImage(CImagePtr image);

  void flash();

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void clickSlot();

 private:
  QPushButton* qbutton_ { nullptr };
};

//---

class CTkAppCanvasWidget;
class CTkAppCanvasShape;

class CTkAppCanvas : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkAppCanvas(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Canvas"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  bool triggerMousePressEvents(QEvent *e, int button) override;
  bool triggerMouseMoveEvents (QEvent *e, int button, bool pressed) override;

  double width() const { return width_; }
  void setWidth(double r) { width_ = r; }

  double height() const { return height_; }
  void setHeight(double r) { height_ = r; }

 private:
  CTkAppCanvasWidget* qcanvas_ { nullptr };

  using IdEventDatas = std::map<std::string, EventDatas>;

  IdEventDatas       idEventDatas_;
  CTkAppCanvasShape* insideShape_ { nullptr };
  double             width_  { 400 };
  double             height_ { 400 };
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
  CTkAppCanvasShape(ShapeType type) :
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

  const Qt::Alignment &align() const { return align_; }
  void setAlign(const Qt::Alignment &v) { align_ = v; }

  virtual bool inside(double /*x*/, double /*y*/) const { return false; }

  virtual double distance(double x, double y) = 0;

  virtual void move(double dx, double dy) = 0;

  virtual bool getPoints(Points &) const { return false; }
  virtual bool setPoints(const Points &) { return false; }

 protected:
  ShapeType     type_  { ShapeType::NONE };
  int           id_    { 0 };
  Tags          tags_;
  QPen          pen_;
  QBrush        brush_;
  Qt::Alignment align_ { Qt::AlignCenter };
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
    Rectangle(double x1, double y1, double x2, double y2) :
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
      x1_ += dx;
      y1_ += dy;
      x2_ += dx;
      y2_ += dy;
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
    Oval(double x1, double y1, double x2, double y2) :
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
      x1_ += dx;
      y1_ += dy;
      x2_ += dx;
      y2_ += dy;
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
    Line(const Points &points) :
     Shape(ShapeType::LINE), points_(points) {
    }

    const Points &points() const { return points_; }

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

      return true;
    }

    const ArrowType &arrowType() const { return arrowType_; }
    void setArrowType(const ArrowType &t) { arrowType_ = t; }

   protected:
    Points points_;
    ArrowType arrowType_ { ArrowType::NONE };
  };

  class Polygon : public Shape {
   public:
    Polygon(const Points &points) :
     Shape(ShapeType::POLYGON), points_(points) {
    }

    const Points &points() const { return points_; }

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

      return true;
    }

   protected:
    Points points_;
  };

  class Arc : public Shape {
   public:
    Arc(const Points &points) :
     Shape(ShapeType::ARC), points_(points) {
    }

    const Points &points() const { return points_; }

    int start() const { return start_; }
    void setStart(int i) { start_ = i; }

    int extent() const { return extent_; }
    void setExtent(int i) { extent_ = i; }

    const ArcType &arcType() const { return arcType_; }
    void setArcType(const ArcType &t) { arcType_ = t; }

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

      return true;
    }

   protected:
    Points  points_;
    int     start_   { 0 };
    int     extent_  { 360 };
    ArcType arcType_ { ArcType::ARC };
  };

  class Text : public Shape {
   public:
    Text(const Point &p, const std::string &text) :
     Shape(ShapeType::TEXT), p_(p), text_(text) {
    }

    const Point &pos() const { return p_; }

    const std::string &text() const { return text_; }
    void setText(const std::string &s) { text_ = s; }

    const QFont &font() const { return font_; }
    void setFont(const QFont &f) { font_ = f; }

    const Qt::Alignment &justfy() const { return justify_; }
    void setJustify(const Qt::Alignment &v) { justify_ = v; }

    void move(double dx, double dy) override {
      p_.move(dx, dy);
    }

    double distance(double x, double y) override {
      return p_.distance(x, y);
    }

   protected:
    Point         p_;
    std::string   text_;
    QFont         font_;
    Qt::Alignment justify_ { Qt::AlignLeft };
  };

  class Image : public Shape {
   public:
    Image(const Point &p, const CImagePtr &image) :
     Shape(ShapeType::IMAGE), p_(p), image_(image) {
    }

    const Point &pos() const { return p_; }

    const CImagePtr &getImage() const { return image_; }
    void setImage(const CImagePtr &i) { image_ = i; }

    void move(double dx, double dy) override {
      p_.move(dx, dy);
    }

    double distance(double x, double y) override {
      return p_.distance(x, y);
    }

   protected:
    Point     p_;
    CImagePtr image_;
  };

  class Bitmap : public Shape {
   public:
    Bitmap(const Point &p, const CImagePtr &image) :
     Shape(ShapeType::BITMAP), p_(p), image_(image) {
    }

    const Point &pos() const { return p_; }

    const CImagePtr &getImage() const { return image_; }
    void setImage(const CImagePtr &i) { image_ = i; }

    void move(double dx, double dy) override {
      p_.move(dx, dy);
    }

    double distance(double x, double y) override {
      return p_.distance(x, y);
    }

   protected:
    Point     p_;
    CImagePtr image_;
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

  Polygon *addPolygon(const Points &points) {
    auto *polygonShape = new Polygon(points);

    return static_cast<Polygon *>(addShape(polygonShape));
  }

  Arc *addArc(const Points &points) {
    auto *arcShape = new Arc(points);

    return static_cast<Arc *>(addShape(arcShape));
  }

  Text *addText(const Point &pos, const std::string &text) {
    auto *textShape = new Text(pos, text);

    return static_cast<Text *>(addShape(textShape));
  }

  Image *addImage(const Point &pos, const CImagePtr &image=CImagePtr()) {
    auto *imageShape = new Image(pos, image);

    return static_cast<Image *>(addShape(imageShape));
  }

  Bitmap *addBitmap(const Point &pos, const CImagePtr &image=CImagePtr()) {
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
    int id;

    if (! CStrUtil::toInteger(name, &id))
      return false;

    uint i;

    if (! shapeInd(id, i))
      return false;

    shapes_[i]->move(dx, dy);

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
    int id;

    if (! CStrUtil::toInteger(name, &id))
      return false;

    uint i;

    if (! shapeInd(id, i))
      return false;

    if (! shapes_[i]->setPoints(points))
      return false;

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

class CTkAppCheckButton : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkAppCheckButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "CheckButton"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void updateVariable();

  void setChecked(bool b);

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void stateChangedSlot(int);

 private:
  QCheckBox*                qcheck_  { nullptr };
  std::string               varName_;
  CTkAppCheckButtonVarProc* varProc_ { nullptr };
};

//---

class CTkAppEntryVarProc;
class CTkAppEntryValidator;

class CTkAppEntry : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkAppEntry(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Entry"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void updateVariable();

  bool validate(const std::string &) const;

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

  QLineEdit*            qedit_ { nullptr };
  std::string           varName_;
  CTkAppEntryVarProc*   varProc_ { nullptr };
  ValidateMode          validateMode_ { ValidateMode::NONE };
  std::string           validateCmd_;
  CTkAppEntryValidator* validateProc_ { nullptr };
};

//---

class CTkAppFrame : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkAppFrame(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

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
  CTkAppLabel(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Label"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void setImage(CImagePtr image);

  void updateVariable();

 private:
  CQLabelImage*       qlabel_ { nullptr };
  std::string         varName_;
  CTkAppLabelVarProc* varProc_ { nullptr };
};

//---

class CTkAppLabelFrame : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkAppLabelFrame(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

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
  CTkAppListBox(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "ListBox"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void updateVariable();

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void vscrollSlot(int value);
  void hscrollSlot(int value);

 private:
  QListWidget*          qlist_ { nullptr };
  std::string           varName_;
  CTkAppListBoxVarProc* varProc_ { nullptr };
};

//---

class CTkAppCheckAction : public QWidgetAction {
  Q_OBJECT

 public:
  CTkAppCheckAction(CTkApp* tk) :
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

  void updateVariable();

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
  CTkAppRadioAction(CTkApp* tk) :
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

  void updateVariable();

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
  CTkAppSubMenu(CTkApp* tk);

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
  CTkAppMenu(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

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
  CTkAppMenuButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "MenuButton"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void setImage(CImagePtr image);

  void updateMenu();

  void setMenu(CTkAppMenu *menu);

  void updateVariable();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void clickSlot();

 private:
  QToolButton*             qbutton_ { nullptr };
  std::string              menuName_;
  std::string              varName_;
  CTkAppMenuButtonVarProc* varProc_ { nullptr };
};

//---

class CTkAppMessageVarProc;

class CTkAppMessage : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkAppMessage(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Message"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void updateVariable();

 private:
  QTextEdit*            qedit_ { nullptr };
  std::string           varName_;
  CTkAppMessageVarProc* varProc_ { nullptr };
};

//---

class CTkAppPanedWindow : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkAppPanedWindow(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "PanedWindow"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  QSplitter* qpane_ { nullptr };
};

//---

class CTkAppRadioButtonVarProc;

class CTkAppRadioButton : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkAppRadioButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "RadioButton"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void setValue(const std::string &value);

  void setChecked(bool b);

  void updateVariable();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void toggleSlot(bool);

 private:
  QRadioButton*             qradio_ { nullptr };
  std::string               varName_;
  std::string               value_ { "1" };
  CTkAppRadioButtonVarProc* varProc_ { nullptr };
};

//---

class CTkAppScaleVarProc;

class CTkAppScale : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkAppScale(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Scale"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void updateVariable();

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
  CTkAppScrollBar(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

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
  CTkAppSpinBox(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "SpinBox"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setValue(const std::string &);

  void updateVariable();

 private:
  CQSpinList*           qspin_ { nullptr };
  std::string           varName_;
  CTkAppSpinBoxVarProc* varProc_;
};

//---

class CTkAppText : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkAppText(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Text"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void vscrollSlot(int value);
  void hscrollSlot(int value);

 private:
  QTextEdit* qtext_ { nullptr };
};

//---

class CTkAppTopLevel : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkAppTopLevel(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "TopLevel"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  QFrame* qframe_ { nullptr };
};

#endif
