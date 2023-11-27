#ifndef CTkAppCanvasShape_H
#define CTkAppCanvasShape_H

#include <CTkApp.h>
#include <CTkAppPoint.h>

#include <QObject>
#include <QPainterPath>
#include <QPen>
#include <QFont>

#include <set>
#include <optional>

class CTkAppCanvasWidget;
class CTclApp;

enum class CTkAppCanvasShapeType {
  NONE,
  ARC,
  BITMAP,
#ifdef CTK_APP_TKPATH
  CIRCLE,
  ELLIPSE,
#endif
  IMAGE,
  LINE,
  OVAL,
#ifdef CTK_APP_TKPATH
  PATH,
#endif
  POLYGON,
  RECTANGLE,
  TEXT,
  WINDOW
};

//---

class CTkAppCanvasShape : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString typeName READ typeName)

  Q_PROPERTY(long id READ id)

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
    long          x        { 0 };
    long          y        { 0 };
  };

  enum class IndexType {
    NONE,
    END,
    INSERT,
    SEL_FIRST,
    SEL_LAST,
    COORD,
    POS
  };

  struct IndexData {
    IndexType type { IndexType::NONE };
    long      x    { 0 };
    long      y    { 0 };
    long      ind  { 0 };
  };

 public:
  explicit CTkAppCanvasShape(CTkAppCanvasWidget *canvas, ShapeType type);

  virtual ~CTkAppCanvasShape() = default;

  //---

  CTkAppCanvasWidget *canvas() const { return canvas_; }

  ShapeType type() const { return type_; }

  QString typeName() const;

  long id() const { return id_; }

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

  void addTag(const QString &tag);

  void deleteTag(const QString &tag);

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

  long fontSize() const { return font_.pixelSize(); }
  void setFontSize(long s) { font_.setPixelSize(s); }

  const Qt::Alignment &anchor() const { return anchor_; }
  void setAnchor(const Qt::Alignment &v) { anchor_ = v; }

  //---

  virtual bool canFocus() const { return false; }

  //---

  virtual bool addEndItem(const QString &) { return false; }

  //---

  virtual QRectF rect() const = 0;

  virtual bool inside(const Point & /*p*/) const = 0;

  virtual bool intersects(const QRectF &r) const;

  virtual double distance(double x, double y) = 0;

  virtual void moveBy(double dx, double dy) = 0;

  virtual void scale(double xc, double yc, double sx, double sy);

  virtual void rotate(double xc, double yc, double a);

  //---

  virtual bool getPoints(Points &) const = 0;
  virtual bool setPoints(const Points &) { return false; }

  const QTransform &transform() const { return transform_; }
  void setTransform(const QTransform &v) { transform_ = v; }

  const QRectF &drawRect() const { return drawRect_; }
  void setDrawRect(const QRectF &r) { drawRect_ = r; }

  QPainterPath calcStrokePath(const QPainterPath &path, const QPen &pen) const;

  const OffsetData &stippleOffset() const { return  offsetData_; }
  void setStippleOffset(const OffsetData &offsetData) { offsetData_ = offsetData; }

  const OffsetData &stippleOutlineOffset() const { return outlineOffsetData_; }
  void setStippleOutlineOffset(const OffsetData &offsetData) { outlineOffsetData_ = offsetData; }

  static bool stringToOffsetData(CTclApp *app, const QString &str, OffsetData &offsetData);
  static QString offsetDataToString(const OffsetData &offsetData);

  //---

  virtual bool movePoint(const QString &ind, const Point &p);

  //---

  virtual bool parseIndex(const QString &str, IndexData &indexData) const;

  virtual bool indexPos(const QString & /*ind*/, long &pos) { pos = 0; return false; }

  virtual bool deleteChars(const QString &start, const QString &end);

  virtual bool replaceChars(const QString &start, const QString &end, const QString &str);

  virtual void setInsertCursor(const QString &) { }

  //---

  virtual bool getShapeOpt(const QString &name, QVariant &value) const;
  virtual bool setShapeOpt(const QString &name, const QVariant &value, bool &rc);

  //---

  bool getItemPoints(const QString &item, Points &points) const;

 protected:
  CTkAppCanvasWidget* canvas_ { nullptr };

  ShapeType type_ { ShapeType::NONE };
  long      id_   { 0 };

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

//---

class CTkAppCanvasArcShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  enum class ArcType {
    NONE,
    PIE,
    CHORD,
    ARC
  };

  using OptReal = std::optional<double>;

 public:
  explicit CTkAppCanvasArcShape(CTkAppCanvasWidget *canvas, const Points &points);

  const Points &points() const { return points_; }

  double start() const { return start_; }
  void setStart(double r) { start_ = r; updatePath(); }

  double extent() const { return extent_; }
  void setExtent(double r) { extent_ = r; updatePath(); }

  const ArcType &arcType() const { return arcType_; }
  void setArcType(const ArcType &t) { arcType_ = t; updatePath(); }

  const OptReal &height() const { return height_; }
  void setHeight(const OptReal &r) { height_ = r; }

  QRectF rect() const override {
    return qpath_.boundingRect();
  }

  bool inside(const Point &p) const override {
    return qpath_.contains(p.qpoint());
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

  //---

  bool getShapeOpt(const QString &name, QVariant &value) const override;
  bool setShapeOpt(const QString &name, const QVariant &value, bool &rc) override;

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
  double       start_   { 0.0 };
  double       extent_  { 360.0 };
  OptReal      height_;
  ArcType      arcType_ { ArcType::ARC };
  QPainterPath qpath_;
};

//---

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
                                   const CTkAppImageRef &image);

  //---

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

  bool inside(const Point & /*p*/) const override { return false; }

  double distance(double x, double y) override {
    return p_.distance(x, y);
  }

  void moveBy(double dx, double dy) override {
    p_.moveBy(dx, dy);
  }

  bool getPoints(Points &points) const override {
    points.push_back(p_);
    return true;
  }

  //---

  bool getShapeOpt(const QString &name, QVariant &value) const override;
  bool setShapeOpt(const QString &name, const QVariant &value, bool &rc) override;

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

//---

#ifdef CTK_APP_TKPATH
class CTkAppCanvasCircleShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  explicit CTkAppCanvasCircleShape(CTkAppCanvasWidget *canvas, double xc, double yc);

  double radius() const { return r_; }
  void setRadius(double r) { r_ = r; updatePath(); }

  QRectF rect() const override { return QRectF(xc_ - r_, yc_ - r_, 2*r_, 2*r_); }

  bool inside(const Point &p) const override {
    return qpath_.contains(p.qpoint());
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

  bool getPoints(Points &points) const override {
    points.push_back(Point(xc_ - r_, yc_ - r_));
    points.push_back(Point(xc_ + r_, yc_ + r_));
    return true;
  }

  bool setPoints(const Points &) override {
    return false;
  }

  const QPainterPath &qpath() const { return qpath_; }

  //---

  bool getShapeOpt(const QString &name, QVariant &value) const override;
  bool setShapeOpt(const QString &name, const QVariant &value, bool &rc) override;

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
#endif

//---

#ifdef CTK_APP_TKPATH
class CTkAppCanvasEllipseShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  explicit CTkAppCanvasEllipseShape(CTkAppCanvasWidget *canvas, double xc, double yc);

  double radiusX() const { return xr_; }
  void setRadiusX(double r) { xr_ = r; updatePath(); }

  double radiusY() const { return yr_; }
  void setRadiusY(double r) { yr_ = r; updatePath(); }

  QRectF rect() const override { return QRectF(xc_ - xr_, yc_ - yr_, 2*xr_, 2*yr_); }

  bool inside(const Point &p) const override {
    return qpath_.contains(p.qpoint());
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

  bool getPoints(Points &points) const override {
    points.push_back(Point(xc_ - xr_, yc_ - yr_));
    points.push_back(Point(xc_ + xr_, yc_ + yr_));
    return true;
  }

  bool setPoints(const Points &) override {
    return false;
  }

  const QPainterPath &qpath() const { return qpath_; }

  //---

  bool getShapeOpt(const QString &name, QVariant &value) const override;
  bool setShapeOpt(const QString &name, const QVariant &value, bool &rc) override;

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

//---

class CTkAppCanvasImageShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  explicit CTkAppCanvasImageShape(CTkAppCanvasWidget *canvas, const Point &p,
                                  const CTkAppImageRef &image);

  const Point &pos() const { return p_; }

  const CTkAppImageRef &getImage() const { return image_; }
  void setImage(const CTkAppImageRef &i) { image_ = i; }

  const CTkAppImageRef &getActiveImage() const { return activeImage_; }
  void setActiveImage(const CTkAppImageRef &i) { activeImage_ = i; }

  const CTkAppImageRef &getDisabledImage() const { return disabledImage_; }
  void setDisabledImage(const CTkAppImageRef &i) { disabledImage_ = i; }

  QRectF rect() const override { return drawRect(); }

  bool inside(const Point &p) const override {
    return drawRect().contains(p.qpoint());
  }

  double distance(double x, double y) override {
    return p_.distance(x, y);
  }

  void moveBy(double dx, double dy) override {
    p_.moveBy(dx, dy);
  }

  bool getPoints(Points &points) const override {
    points.push_back(p_);
    return true;
  }

  //---

  bool getShapeOpt(const QString &name, QVariant &value) const override;
  bool setShapeOpt(const QString &name, const QVariant &value, bool &rc) override;

 protected:
  Point          p_;
  CTkAppImageRef image_;
  CTkAppImageRef activeImage_;
  CTkAppImageRef disabledImage_;
};

//---

class CTkAppCanvasLineShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  enum class ArrowType {
    NONE,
    FIRST,
    LAST,
    BOTH
  };

  struct ArrowShape {
    double d1 { 0.0 };
    double d2 { 0.0 };
    double d3 { 0.0 };
  };

 public:
  explicit CTkAppCanvasLineShape(CTkAppCanvasWidget *canvas, const Points &points);

  //---

  const Points &points() const { return points_; }
  bool setPoints(const Points &points) override { points_ = points; updatePath(); return true; }

  const ArrowType &arrowType() const { return arrowType_; }
  void setArrowType(const ArrowType &t) { arrowType_ = t; updatePath(); }

  const ArrowShape &arrowShape() const { return arrowShape_; }
  void setArrowShape(const ArrowShape &v) { arrowShape_ = v; }

  const Qt::PenCapStyle &capStyle() const { return capStyle_; }
  void setCapStyle(const Qt::PenCapStyle &v) { capStyle_ = v; }

  const Qt::PenJoinStyle &joinStyle() const { return joinStyle_; }
  void setJoinStyle(const Qt::PenJoinStyle &v) { joinStyle_ = v; }

  const QString &smooth() const { return smooth_; }
  void setSmooth(const QString &s) { smooth_ = s; }

  long splineSteps() const { return splineSteps_; }
  void setSplineSteps(long i) { splineSteps_ = i; }

  //---

  bool inside(const Point &p) const override {
    return qStrokePath_.contains(p.qpoint());
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
    } return path.boundingRect();
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

  bool addEndItem(const QString &item) override;

  const QPainterPath &qpath() const { return qpath_; }

  const QPainterPath &getQStrokePath() const { return qStrokePath_; }
  void setQStrokePath(const QPainterPath &v) { qStrokePath_ = v; }

  //---

  bool getShapeOpt(const QString &name, QVariant &value) const override;
  bool setShapeOpt(const QString &name, const QVariant &value, bool &rc) override;

  //---

  bool replaceChars(const QString &start, const QString &end, const QString &str) override;

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
  Points           points_;
  ArrowType        arrowType_   { ArrowType::NONE };
  ArrowShape       arrowShape_;
  Qt::PenCapStyle  capStyle_    { Qt::FlatCap };
  Qt::PenJoinStyle joinStyle_   { Qt::RoundJoin };
  QString          smooth_;
  long             splineSteps_ { 0 };
  QPainterPath     qpath_;
  QPainterPath     qStrokePath_;
};

//---

class CTkAppCanvasOvalShape : public CTkAppCanvasShape {
  Q_OBJECT

  Q_PROPERTY(double x1 READ x1 WRITE setX1)
  Q_PROPERTY(double y1 READ y1 WRITE setY1)
  Q_PROPERTY(double x2 READ x2 WRITE setX2)
  Q_PROPERTY(double y2 READ y2 WRITE setY2)

 public:
  explicit CTkAppCanvasOvalShape(CTkAppCanvasWidget *canvas, double x1, double y1,
                                 double x2, double y2);

  double x1() const { return x1_; } void setX1(double x1) { x1_ = x1; updatePaths(); }
  double y1() const { return y1_; } void setY1(double y1) { y1_ = y1; updatePaths(); }
  double x2() const { return x2_; } void setX2(double x2) { x2_ = x2; updatePaths(); }
  double y2() const { return y2_; } void setY2(double y2) { y2_ = y2; updatePaths(); }

  bool inside(const Point &p) const override {
    return rect().contains(p.qpoint());
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

  bool getPoints(Points &points) const override {
    points.push_back(Point(x1_, y1_));
    points.push_back(Point(x2_, y2_));
    return true;
  }

  QRectF rect() const override { return drawRect(); }

  const QPainterPath &path() const { return path_; }

  const QPainterPath &strokePath() const { return strokePath_; }

  //---

  bool getShapeOpt(const QString &name, QVariant &value) const override;
  bool setShapeOpt(const QString &name, const QVariant &value, bool &rc) override;

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

//---

#ifdef CTK_APP_TKPATH
class CTkAppCanvasPathShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  explicit CTkAppCanvasPathShape(CTkAppCanvasWidget *canvas, const QString &pathStr,
                                 const QPainterPath &qpath);

  const QString &pathStr() const { return pathStr_; }

  QRectF rect() const override {
    return qpath_.boundingRect();
  }

  bool inside(const Point &p) const override {
    auto qpoint = p.qpoint();

    return qpath_.contains(qpoint) || qStrokePath_.contains(qpoint);
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

  //---

  bool getShapeOpt(const QString &name, QVariant &value) const override;
  bool setShapeOpt(const QString &name, const QVariant &value, bool &rc) override;

 protected:
  QString      pathStr_;
  QPainterPath qpath_;
  QPainterPath qStrokePath_;
};
#endif

//---

class CTkAppCanvasPolygonShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  explicit CTkAppCanvasPolygonShape(CTkAppCanvasWidget *canvas, const Points &points);

  const Points &points() const { return points_; }

  QRectF rect() const override {
    return qpath_.boundingRect();
  }

  const Qt::PenJoinStyle &joinStyle() const { return joinStyle_; }
  void setJoinStyle(const Qt::PenJoinStyle &v) { joinStyle_ = v; }

  bool isSmooth() const { return smooth_; }
  void setSmooth(bool b) { smooth_ = b; }

  long splineSteps() const { return splineSteps_; }
  void setSplineSteps(long i) { splineSteps_ = i; }

  //---

  bool intersects(const QRectF &r) const override {
    return qpath_.intersects(r);
  }

  bool inside(const Point &p) const override {
    return qpath_.contains(p.qpoint());
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

  bool addEndItem(const QString &item) override;

  const QPainterPath &qpath() const { return qpath_; }

  //---

  bool getShapeOpt(const QString &name, QVariant &value) const override;
  bool setShapeOpt(const QString &name, const QVariant &value, bool &rc) override;

  //---

  bool deleteChars(const QString &start, const QString &end) override;

  bool replaceChars(const QString &start, const QString &end, const QString &str) override;

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
  Points           points_;
  Qt::PenJoinStyle joinStyle_   { Qt::RoundJoin };
  bool             smooth_      { false };
  long             splineSteps_ { 0 };
  QPainterPath     qpath_;
};

//---

class CTkAppCanvasRectangleShape : public CTkAppCanvasShape {
  Q_OBJECT

  Q_PROPERTY(double x1 READ x1 WRITE setX1)
  Q_PROPERTY(double y1 READ y1 WRITE setY1)
  Q_PROPERTY(double x2 READ x2 WRITE setX2)
  Q_PROPERTY(double y2 READ y2 WRITE setY2)

 public:
  explicit CTkAppCanvasRectangleShape(CTkAppCanvasWidget *canvas, double x1, double y1,
                                      double x2, double y2);

  //---

  double x1() const { return p1_.x; } void setX1(double x1) { p1_.x = x1; updatePaths(); }
  double y1() const { return p1_.y; } void setY1(double y1) { p1_.y = y1; updatePaths(); }
  double x2() const { return p2_.x; } void setX2(double x2) { p2_.x = x2; updatePaths(); }
  double y2() const { return p2_.y; } void setY2(double y2) { p2_.y = y2; updatePaths(); }

  const QString &smooth() const { return smooth_; }
  void setSmooth(const QString &s) { smooth_ = s; }

  long splineSteps() const { return splineSteps_; }
  void setSplineSteps(long i) { splineSteps_ = i; }

  //---

  bool inside(const Point &p) const override {
    return rect().contains(p.qpoint());
  }

  double distance(double x, double y) override {
    return std::min({ Point(p1_.x, p1_.y).distance(x, y),
                      Point(p1_.x, p2_.y).distance(x, y),
                      Point(p2_.x, p1_.y).distance(x, y),
                      Point(p2_.x, p2_.y).distance(x, y)});
  }

  void moveBy(double dx, double dy) override {
    p1_.x += dx; p1_.y += dy;
    p2_.x += dx; p2_.y += dy;

    updatePaths();
  }

  bool getPoints(Points &points) const override {
    points.push_back(p1_);
    points.push_back(p2_);
    return true;
  }

  bool setPoints(const Points &points) override {
    if (points.size() != 2) return false;
    p1_ = points[0];
    p2_ = points[1];
    return true;
  }

  QRectF rect() const override { return drawRect(); }

  const QPainterPath &path() const { return path_; }

  const QPainterPath &strokePath() const { return strokePath_; }

  //---

  bool getShapeOpt(const QString &name, QVariant &value) const override;
  bool setShapeOpt(const QString &name, const QVariant &value, bool &rc) override;

 private:
  void updatePaths() {
    auto rect = calcRect();

    path_ = QPainterPath();
    path_.addRect(rect);

    strokePath_ = calcStrokePath(path_, pen());

    setDrawRect(path_.boundingRect().united(strokePath_.boundingRect()));
  }

  QRectF calcRect() const { return QRectF(p1_.x, p1_.y, p2_.x - p1_.x, p2_.y - p1_.y); }

 protected:
  Point p1_;
  Point p2_;

  QString smooth_;
  long    splineSteps_ { 0 };

  QPainterPath path_;
  QPainterPath strokePath_;
};

//---

class CTkAppCanvasTextShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  explicit CTkAppCanvasTextShape(CTkAppCanvasWidget *canvas, const Point &p, const QString &text);

  const Point &pos() const { return p_; }

  const QString &getText() const { return text_; }
  void setText(const QString &s) { text_ = s; }

  double angle() const { return angle_; }
  void setAngle(double r) { angle_ = r; }

  const QFont &font() const { return font_; }
  void setFont(const QFont &f) { font_ = f; }

  const Qt::Alignment &justify() const { return justify_; }
  void setJustify(const Qt::Alignment &v) { justify_ = v; }

  Qt::Alignment textAnchor() const { return textAnchor_; }
  void setTextAnchor(const Qt::Alignment &textAnchor) { textAnchor_ = textAnchor; }

  int underLine() const { return underLine_; }
  void setUnderLine(int i) { underLine_ = i; }

  double width() const { return width_; }
  void setWidth(double r) { width_ = r; }

  //---

  QRectF rect() const override {
    return QRectF(); // TODO
  }

  bool inside(const Point & /*p*/) const override { return false; }

  double distance(double x, double y) override {
    return p_.distance(x, y);
  }

  void moveBy(double dx, double dy) override {
    p_.moveBy(dx, dy);
  }

  bool getPoints(Points &points) const override {
    points.push_back(p_);
    return true;
  }

  bool setPoints(const Points &points) override {
    if (points.size() != 1) return false;
    p_ = points[0];
    return true;
  }

  //---

  bool deleteChars(const QString &start, const QString &end) override;

  bool replaceChars(const QString &start, const QString &end, const QString &str) override;

  //---

  bool getShapeOpt(const QString &name, QVariant &value) const override;
  bool setShapeOpt(const QString &name, const QVariant &value, bool &rc) override;

 protected:
  Point         p_;
  QString       text_;
  double        angle_      { 0.0 };
  QFont         font_;
  Qt::Alignment justify_    { Qt::AlignLeft | Qt::AlignTop };
  Qt::Alignment textAnchor_ { Qt::AlignLeft };
  int           underLine_  { -1 };
  double        width_      { 0.0 };
};

//---

class CTkAppCanvasWindowShape : public CTkAppCanvasShape {
  Q_OBJECT

 public:
  explicit CTkAppCanvasWindowShape(CTkAppCanvasWidget *canvas, double x, double y);

  double width() const { return width_; }
  void setWidth(double r) { width_ = r; }

  double height() const { return height_; }
  void setHeight(double r) { height_ = r; }

  const QString &window() const { return window_; }
  void setWindow(const QString &s) { window_ = s; }

  //---

  QRectF rect() const override {
    return QRectF(); // TODO
  }

  bool inside(const Point & /*p*/) const override { return false; }

  double distance(double x, double y) override {
    return Point(x_, y_).distance(x, y);
  }

  void moveBy(double dx, double dy) override {
    x_ += dx; y_ += dy;
  }

  bool getPoints(Points &points) const override {
    points.push_back(Point(x_, y_));
    return true;
  }

  //---

  bool getShapeOpt(const QString &name, QVariant &value) const override;
  bool setShapeOpt(const QString &name, const QVariant &value, bool &rc) override;

 protected:
  double x_ { 0.0 };
  double y_ { 0.0 };

  double  width_  { -1 };
  double  height_ { -1 };
  QString window_;
};

#endif
