#ifndef CTkAppCanvasWidget_H
#define CTkAppCanvasWidget_H

#include <CTkAppPoint.h>
#include <CTkApp.h>

#include <CDisplayRange2D.h>

#include <QWidget>
#include <QPointer>

#include <optional>

class CTkAppCanvas;

class CTkAppCanvasShape;
class CTkAppCanvasArcShape;
class CTkAppCanvasBitmapShape;
#ifdef CTK_APP_TKPATH
class CTkAppCanvasCircleShape;
class CTkAppCanvasEllipseShape;
#endif
class CTkAppCanvasImageShape;
class CTkAppCanvasLineShape;
class CTkAppCanvasOvalShape;
#ifdef CTK_APP_TKPATH
class CTkAppCanvasPathShape;
#endif
class CTkAppCanvasPolygonShape;
class CTkAppCanvasRectangleShape;
class CTkAppCanvasTextShape;
class CTkAppCanvasWindowShape;

class CQStrParse;

class CTkAppCanvasWidget : public QWidget {
  Q_OBJECT

 public:
  using Canvas = CTkAppCanvasWidget;
  using Shape  = CTkAppCanvasShape;
  using ShapeP = QPointer<Shape>;
  using Shapes = std::vector<Shape *>;
  using Point  = CTkAppPoint;
  using Points = CTkAppPoints;

  using Arc       = CTkAppCanvasArcShape;
  using Bitmap    = CTkAppCanvasBitmapShape;
#ifdef CTK_APP_TKPATH
  using Circle    = CTkAppCanvasCircleShape;
  using Ellipse   = CTkAppCanvasEllipseShape;
#endif
  using Image     = CTkAppCanvasImageShape;
  using Line      = CTkAppCanvasLineShape;
  using Oval      = CTkAppCanvasOvalShape;
#ifdef CTK_APP_TKPATH
  using Path      = CTkAppCanvasPathShape;
#endif
  using Polygon   = CTkAppCanvasPolygonShape;
  using Rectangle = CTkAppCanvasRectangleShape;
  using Text      = CTkAppCanvasTextShape;
  using Window    = CTkAppCanvasWindowShape;

  using OptReal = std::optional<double>;

  //---

  enum class TagTokenType {
    NONE,
    NOT,
    AND,
    OR,
    XOR,
    TAG,
    EXPRESSION
  };

  struct TagToken {
    TagTokenType          type { TagTokenType::NONE };
    QString               str;
    std::vector<TagToken> tokens;

    TagToken() { }
    TagToken(TagTokenType _type) : type(_type) { }
  };

  struct TagTokenData {
    bool                  isId { false };
    int                   id { 0 };
    std::vector<TagToken> tokens;
  };

 public:
  CTkAppCanvasWidget(CTkAppCanvas *canvas);

  //---

  CTkAppCanvas *canvas() const { return canvas_; }

  const Shapes &getShapes() const { return shapes_; }

  bool tagsMatch(const std::set<QString> &tags, const TagTokenData &tagTokenData) const;
  bool tagsMatch(const std::set<QString> &tags, const std::vector<TagToken> &tokens,
                 uint &i, uint n) const;

  //---

  double closeEnough() const { return closeEnough_; }
  void setCloseEnough(double r) { closeEnough_ = r; }

  bool isConfine() const { return confine_; }
  void setConfine(bool b) { confine_ = b; }

  //---

  const QRectF &scrollRegion() const { return scrollRegion_; }
  void setScrollRegion(const QRectF &r);

  double scrollX() const { return scrollX_; }
  void setScrollX(double x);

  double scrollY() const { return scrollY_; }
  void setScrollY(double y);

  double xScrollIncrement() const { return xScrollIncrement_; }
  void setXScrollIncrement(double r) { xScrollIncrement_ = r; }

  double yScrollIncrement() const { return yScrollIncrement_; }
  void setYScrollIncrement(double r) { yScrollIncrement_ = r; }

  double scrollXMin() const {
    return (scrollRegion_.isValid() ? scrollRegion_.left() : 0.0); }
  double scrollXMax() const {
    return (scrollRegion_.isValid() ? scrollRegion_.right() : width() - 1.0); }

  double scrollYMin() const {
    return (scrollRegion_.isValid() ? scrollRegion_.top() : 0.0); }
  double scrollYMax() const {
    return (scrollRegion_.isValid() ? scrollRegion_.bottom() : height() - 1.0); }

  double scrollWidth () const {
    return (scrollRegion_.isValid() ? scrollRegion_.width() : width()); }
  double scrollHeight() const {
    return (scrollRegion_.isValid() ? scrollRegion_.height() : height()); }

  double scrollRelX1() const;
  double scrollRelY1() const;
  double scrollRelX2() const;
  double scrollRelY2() const;

  //---

  Arc*       addArc      (const Points &points);
  Bitmap*    addBitmap   (const Point &pos, const CTkAppImageRef &image=CTkAppImageRef());
#ifdef CTK_APP_TKPATH
  Circle*    addCircle   (double xc, double yc);
  Ellipse*   addEllipse  (double xc, double yc);
#endif
  Image*     addImage    (const Point &pos, const CTkAppImageRef &image=CTkAppImageRef());
  Line*      addLine     (const Points &points);
  Oval*      addOval     (const Point &p1, const Point &p2);
#ifdef CTK_APP_TKPATH
  Path*      addPath     (const QString &pathStr, const QPainterPath &qpath);
#endif
  Polygon*   addPolygon  (const Points &points);
  Rectangle* addRectangle(double x1, double y1, double x2, double y2);
  Text*      addText     (const Point &pos, const QString &text);
  Window*    addWindow   (const Point &p);

  //---

  uint nextShapeId() { return ++shapeId_; }

  QRectF getShapeBBox(const QString &name) const;

  bool deleteShape(const QString &name);
  bool deleteShape(Shape *shape);

  bool lowerShape(const QString &name);
  bool lowerShape(Shape *shape);

  bool raiseShape(const QString &name);
  bool raiseShape(Shape *shape);

  bool moveShapeBy(const QString &name, double dx, double dy);
  bool moveShapeTo(const QString &name, const OptReal &x, const OptReal &y);

  bool scaleShape(const QString &name, double xc, double yc, double sx, double sy);

  bool rotateShape(const QString &name, double x, double y, double a);

  bool getShapePoints(const QString &name, Points &points) const;
  bool setShapePoints(const QString &name, const Points &points);

  bool getShapes(const QString &name, Shapes &shapes) const;
  Shape *getShape(const QString &name) const;

  QRectF getShapesRect(const Shapes &shapes) const;

  void getAllShapes(Shapes &shapes) const;

  Shape *insideShape(const Point &p);

  int findNearest(double x, double y);

  bool shapeInd(const Shape *shape, uint &ind) const;
  bool shapeInd(int id, uint &ind) const;

  bool parseSearchTag(const QString &pattern, TagTokenData &tagTokenData) const;

  bool parseSearchTagEnd(CQStrParse &parse, char endChar, std::vector<TagToken> &tokens) const;

  void printTagTokenData(const TagTokenData &tagTokenData) const;
  void printTagTokens(const std::vector<TagToken> &tokens) const;

  Shape *currentShape() const;
  void setCurrentShape(Shape *shape);

  Shape *focusShape() const;
  void setFocusShape(Shape *shape);

  Shape *getAboveShape(Shape *shape) const;
  void getAboveShapes(Shape *shape, Shapes &shapes) const;
  Shape *getBelowShape(Shape *shape) const;
  void getBelowShapes(Shape *shape, Shapes &shapes) const;

  //---

  void findEnclosedShapes   (const QRectF &r, Shapes &shapes) const;
  void findOverlappingShapes(const QRectF &r, Shapes &shapes) const;

  void findNearestShapes(const Point &p, Shapes &shapes) const;

  bool saveImage(const QString &imageName, int subsample, int zoom, CTkAppImageRef &image);

  //---

  void resizeEvent(QResizeEvent *) override;

  void updateRange();

  void paintEvent(QPaintEvent *) override;

  QBrush calcBrush(Shape *s) const;

  QPen calcPen(Shape *s) const;

  QSize sizeHint() const override;

  //---

  QPainterPath windowToPixel(const QPainterPath &path) const;
  QRectF       windowToPixel(const QRectF &r) const;
  QPointF      windowToPixel(const QPointF &p) const;
  Point        windowToPixel(const Point &w) const;

  QPointF pixelToWindow(const QPointF &p) const;
  Point   pixelToWindow(const Point &p) const;

 private:
  Shape *addShape(Shape *s) { shapes_.push_back(s); return s; }

  void drawShape(QPainter *p, Shape *s);

 private:
  Shapes          shapes_;
  uint            shapeId_          { 0 };
  ShapeP          currentShape_;
  ShapeP          focusShape_;
  double          closeEnough_      { 1.0 };
  double          confine_          { true };
  QRectF          scrollRegion_;
  double          scrollX_          { 0.0 };
  double          scrollY_          { 0.0 };
  CDisplayRange2D displayRange_;
  double          xScrollIncrement_ { 0.0 };
  double          yScrollIncrement_ { 0.0 };
  CTkAppCanvas*   canvas_           { nullptr };
};

#endif
