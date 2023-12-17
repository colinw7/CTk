#include <CTkAppCanvasWidget.h>
#include <CTkAppCanvasShape.h>
#include <CTkAppCanvas.h>
#include <CTkAppWidget.h>
#include <CTkAppImage.h>

#include <CQStrParse.h>

#include <QPainter>

CTkAppCanvasWidget::
CTkAppCanvasWidget(CTkAppCanvas *canvas) :
 QWidget(canvas->getParent() ? canvas->getParent()->getQWidget() : nullptr), canvas_(canvas)
{
  setObjectName("qcanvas");

  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);
}

//---

void
CTkAppCanvasWidget::
setScrollRegion(const QRectF &r)
{
  scrollRegion_ = r;

  updateRange();

  update();
}

void
CTkAppCanvasWidget::
setScrollX(double x)
{
  scrollX_ = x;

  updateRange();

  update();
}

void
CTkAppCanvasWidget::
setScrollY(double y)
{
  scrollY_ = y;

  updateRange();

  update();
}

double
CTkAppCanvasWidget::
scrollRelX1() const
{
  if (scrollRegion_.isValid())
    return scrollX();
  else
    return 0.0;
}

double
CTkAppCanvasWidget::
scrollRelX2() const
{
  if (scrollRegion_.isValid())
    return scrollX() + width()/scrollRegion_.width();
  else
    return 1.0;
}

double
CTkAppCanvasWidget::
scrollRelY1() const
{
  if (scrollRegion_.isValid())
    return scrollY();
  else
    return 0.0;
}

double
CTkAppCanvasWidget::
scrollRelY2() const
{
  if (scrollRegion_.isValid())
    return scrollY() + height()/scrollRegion_.height();
  else
    return 1.0;
}

//---

CTkAppCanvasArcShape *
CTkAppCanvasWidget::
addArc(const Points &points)
{
  auto *arcShape = new Arc(this, points);

  return static_cast<Arc *>(addShape(arcShape));
}

CTkAppCanvasBitmapShape *
CTkAppCanvasWidget::
addBitmap(const Point &pos, const CTkAppImageRef &image)
{
  auto *bitmapShape = new Bitmap(this, pos, image);

  return static_cast<Bitmap *>(addShape(bitmapShape));
}

#ifdef CTK_APP_TKPATH
CTkAppCanvasCircleShape *
CTkAppCanvasWidget::
addCircle(double xc, double yc)
{
  auto *circleShape = new Circle(this, xc, yc);

  return static_cast<Circle *>(addShape(circleShape));
}

CTkAppCanvasEllipseShape *
CTkAppCanvasWidget::
addEllipse(double xc, double yc)
{
  auto *ellipseShape = new Ellipse(this, xc, yc);

  return static_cast<Ellipse *>(addShape(ellipseShape));
}
#endif

CTkAppCanvasImageShape *
CTkAppCanvasWidget::
addImage(const Point &pos, const CTkAppImageRef &image)
{
  auto *imageShape = new Image(this, pos, image);

  return static_cast<Image *>(addShape(imageShape));
}

CTkAppCanvasLineShape *
CTkAppCanvasWidget::
addLine(const Points &points)
{
  auto *lineShape = new Line(this, points);

  return static_cast<Line *>(addShape(lineShape));
}

CTkAppCanvasOvalShape *
CTkAppCanvasWidget::
addOval(const Point &p1, const Point &p2)
{
  auto *ovalShape = new Oval(this, p1.x, p1.y, p2.x, p2.y);

  return static_cast<Oval *>(addShape(ovalShape));
}

#ifdef CTK_APP_TKPATH
CTkAppCanvasPathShape *
CTkAppCanvasWidget::
addPath(const QString &pathStr, const QPainterPath &qpath)
{
  auto *pathShape = new Path(this, pathStr, qpath);

  return static_cast<Path *>(addShape(pathShape));
}
#endif

CTkAppCanvasPolygonShape *
CTkAppCanvasWidget::
addPolygon(const Points &points)
{
  auto *polygonShape = new Polygon(this, points);

  return static_cast<Polygon *>(addShape(polygonShape));
}

CTkAppCanvasRectangleShape *
CTkAppCanvasWidget::
addRectangle(double x1, double y1, double x2, double y2)
{
  auto *rectangleShape = new Rectangle(this, x1, y1, x2, y2);

  return static_cast<Rectangle *>(addShape(rectangleShape));
}

CTkAppCanvasTextShape *
CTkAppCanvasWidget::
addText(const Point &pos, const QString &text)
{
  auto *textShape = new Text(this, pos, text);

  return static_cast<Text *>(addShape(textShape));
}

CTkAppCanvasWindowShape *
CTkAppCanvasWidget::
addWindow(const Point &p)
{
  auto *windowShape = new Window(this, p);

  return static_cast<Window *>(addShape(windowShape));
}

//---

QRectF
CTkAppCanvasWidget::
getShapeBBox(const QString &name) const
{
  QRectF bbox;

  Shapes shapes;

  if (! getShapes(name, shapes))
    return bbox;

  for (auto *shape : shapes) {
    auto bbox1 = shape->rect();
    if (bbox1.isNull()) continue;

    if (bbox.isNull())
      bbox = bbox1;
    else {
      bbox = bbox.united(shape->rect());
    }
  }

  return bbox;
}

bool
CTkAppCanvasWidget::
deleteShape(const QString &name)
{
  if (name == "all") {
    for (auto *shape : shapes_)
      delete shape;

    shapes_.clear();
  }
  else {
    Shapes shapes;

    if (! getShapes(name, shapes))
      return false;

    for (auto *shape : shapes)
      deleteShape(shape);
  }

  return true;
}

bool
CTkAppCanvasWidget::
deleteShape(Shape *shape)
{
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

bool
CTkAppCanvasWidget::
lowerShape(const QString &name)
{
  Shapes shapes;

  if (! getShapes(name, shapes))
    return false;

  for (auto *shape : shapes)
    lowerShape(shape);

  return true;
}

bool
CTkAppCanvasWidget::
lowerShape(Shape *shape)
{
  uint i;

  if (! shapeInd(shape, i))
    return false;

  if (i == 0)
    return true;

  std::swap(shapes_[i], shapes_[i - 1]);

  return true;
}

bool
CTkAppCanvasWidget::
raiseShape(const QString &name)
{
  Shapes shapes;

  if (! getShapes(name, shapes))
    return false;

  for (auto *shape : shapes)
    raiseShape(shape);

  return true;
}

bool
CTkAppCanvasWidget::
raiseShape(Shape *shape)
{
  uint i;

  if (! shapeInd(shape, i))
    return false;

  auto n = shapes_.size();

  if (i == n - 1)
    return true;

  std::swap(shapes_[i], shapes_[i + 1]);

  return true;
}

bool
CTkAppCanvasWidget::
moveShapeBy(const QString &name, double dx, double dy)
{
  Shapes shapes;

  if (! getShapes(name, shapes))
    return false;

  for (auto *shape : shapes)
    shape->moveBy(dx, dy);

  return true;
}

bool
CTkAppCanvasWidget::
moveShapeTo(const QString &name, const OptReal &x, const OptReal &y)
{
  Shapes shapes;

  if (! getShapes(name, shapes))
    return false;

  auto r = getShapesRect(shapes);

  auto dx = (x ? *x - r.left() : 0.0);
  auto dy = (y ? *y - r.top () : 0.0);

  for (auto *shape : shapes)
    shape->moveBy(dx, dy);

  return true;
}

bool
CTkAppCanvasWidget::
scaleShape(const QString &name, double xc, double yc, double sx, double sy)
{
  Shapes shapes;

  if (! getShapes(name, shapes))
    return false;

  for (auto *shape : shapes)
    shape->scale(xc, yc, sx, sy);

  return true;
}

bool
CTkAppCanvasWidget::
rotateShape(const QString &name, double x, double y, double a)
{
  Shapes shapes;

  if (! getShapes(name, shapes))
    return false;

  for (auto *shape : shapes)
    shape->rotate(x, y, a);

  return true;
}

bool
CTkAppCanvasWidget::
getShapePoints(const QString &name, Points &points) const
{
  Shapes shapes;

  if (! getShapes(name, shapes))
    return false;

  for (auto *shape : shapes) {
    if (shape->getPoints(points))
      return true;
  }

  return false;
}

bool
CTkAppCanvasWidget::
setShapePoints(const QString &name, const Points &points)
{
  Shapes shapes;

  if (! getShapes(name, shapes))
    return false;

  for (auto *shape : shapes) {
    if (! shape->setPoints(points))
      return false;
  }

  return true;
}

bool
CTkAppCanvasWidget::
getShapes(const QString &name, Shapes &shapes) const
{
  if (name == "all") {
    shapes = shapes_;
    return true;
  }

  if (name == "current") {
    if (currentShape())
      shapes.push_back(currentShape());
    return true;
  }

  //---

  TagTokenData tagTokenData;
  if (! parseSearchTag(name, tagTokenData))
    return false;

  //---

  if (tagTokenData.isId) {
    for (auto *shape : shapes_) {
      if (shape->id() == tagTokenData.id)
        shapes.push_back(shape);
    }
  }
  else {
    for (auto *shape : shapes_) {
      if (tagsMatch(shape->tags(), tagTokenData))
        shapes.push_back(shape);
    }
  }

  return true;
}

bool
CTkAppCanvasWidget::
tagsMatch(const std::set<QString> &tags, const TagTokenData &tagTokenData) const
{
  uint i = 0;

  return tagsMatch(tags, tagTokenData.tokens, i, tagTokenData.tokens.size());
}

bool
CTkAppCanvasWidget::
tagsMatch(const std::set<QString> &tags, const std::vector<TagToken> &tokens,
          uint &i, uint n) const
{
  bool match = false;

  while (i < n) {
    const auto &token = tokens[i];

    if      (token.type == TagTokenType::NOT) {
      ++i;

      std::vector<TagToken> tokens1;

      tokens1.push_back(tokens[i++]);

      uint i1 = 0;
      match = ! tagsMatch(tags, tokens1, i1, tokens1.size());
    }
    else if (token.type == TagTokenType::AND) {
      if (! match) return false;

      ++i;

      return tagsMatch(tags, tokens, i, n);
    }
    else if (token.type == TagTokenType::OR) {
      if (match) return true;

      ++i;

      return tagsMatch(tags, tokens, i, n);
    }
    else if (token.type == TagTokenType::XOR) {
      ++i;

      return (match ^ tagsMatch(tags, tokens, i, n));
    }
    else if (token.type == TagTokenType::TAG) {
      ++i;

      bool match1 = false;

      for (const auto &tag : tags) {
        if (token.str == tag) {
          match1 = true;
          break;
        }
      }

      match = match1;
    }
    else if (token.type == TagTokenType::EXPRESSION) {
      ++i;

      uint i1 = 0;
      match = tagsMatch(tags, token.tokens, i1, token.tokens.size());
    }
    else
      assert(false);
  }

  return match;
}

CTkAppCanvasShape *
CTkAppCanvasWidget::
getShape(const QString &name) const
{
  Shapes shapes;
  if (! getShapes(name, shapes))
    return nullptr;

  return (! shapes.empty() ? shapes[0] : nullptr);
}

QRectF
CTkAppCanvasWidget::
getShapesRect(const Shapes &shapes) const
{
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

void
CTkAppCanvasWidget::
getAllShapes(Shapes &shapes) const
{
  for (auto *shape : shapes_)
    shapes.push_back(shape);
}

CTkAppCanvasWidget::Shape *
CTkAppCanvasWidget::
insideShape(const Point &p)
{
  for (auto *shape : shapes_) {
    if (! shape->isEnabled())
      continue;

    if (shape->inside(p))
      return shape;
  }

  return nullptr;
}

int
CTkAppCanvasWidget::
findNearest(double x, double y)
{
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

bool
CTkAppCanvasWidget::
shapeInd(const Shape *shape, uint &ind) const
{
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

bool
CTkAppCanvasWidget::
shapeInd(int id, uint &ind) const
{
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

//---

bool
CTkAppCanvasWidget::
parseSearchTag(const QString &pattern, TagTokenData &tagTokenData) const
{
  CQStrParse parse(pattern);

  auto rc = parseSearchTagEnd(parse, '\0', tagTokenData.tokens);

  if (rc && tagTokenData.tokens.size() == 1 && tagTokenData.tokens[0].type == TagTokenType::TAG)
    tagTokenData.id = tagTokenData.tokens[0].str.toInt(&tagTokenData.isId);

#if 0
  std::cerr << "parseSearchTag \"" << pattern.toStdString() << "\" ";
  printTagTokenData(tagTokenData);
  std::cerr << "\n";
#endif

  return rc;
}

void
CTkAppCanvasWidget::
printTagTokenData(const TagTokenData &tagTokenData) const
{
  if (tagTokenData.isId)
    std::cerr << tagTokenData.id;
  else
    printTagTokens(tagTokenData.tokens);
}

void
CTkAppCanvasWidget::
printTagTokens(const std::vector<TagToken> &tokens) const
{
  for (const auto &token : tokens) {
    switch (token.type) {
      case TagTokenType::NOT: std::cerr << "!" ; break;
      case TagTokenType::AND: std::cerr << "&&"; break;
      case TagTokenType::OR : std::cerr << "||"; break;
      case TagTokenType::XOR: std::cerr << "^" ; break;
      case TagTokenType::TAG: std::cerr << token.str.toStdString(); break;
      case TagTokenType::EXPRESSION:
        std::cerr << "("; printTagTokens(token.tokens); std::cerr << ")"; break;
      default:  break;
    }
  }
}

bool
CTkAppCanvasWidget::
parseSearchTagEnd(CQStrParse &parse, char endChar, std::vector<TagToken> &tokens) const
{
  auto *tk = canvas_->app();

  parse.skipSpace();

  QString tag;

  //---

  auto flushTag = [&]() {
    if (tag != "") {
      TagToken token1(TagTokenType::TAG);

      token1.str = tag;

      tokens.push_back(token1);

      tag = "";
    }
  };

  auto hasExpression = [&]() {
    if (tag != "") return true;

    if (tokens.empty()) return false;

    const auto &token = tokens.back();

    return (token.type == TagTokenType::EXPRESSION || token.type == TagTokenType::TAG);
  };

  auto lastIsOperator = [&]() {
    if (tokens.empty()) return false;

    const auto &token = tokens.back();

    return (token.type == TagTokenType::NONE || token.type == TagTokenType::AND ||
            token.type == TagTokenType::OR   || token.type == TagTokenType::XOR ||
            token.type == TagTokenType::NOT);
  };

  auto isSpecial = [&]() {
    if (parse.isOneOf("!()\"^")) return true;

    if (parse.isChar('&') && parse.isCharAfter('&', 1)) return true;
    if (parse.isChar('|') && parse.isCharAfter('|', 1)) return true;

    return false;
  };

  //---

  while (! parse.eof()) {
    if (endChar != '\0' && parse.isChar(endChar)) {
      flushTag();
      break;
    }

    if      (parse.isChar('!')) {
      if (hasExpression())
        return tk->throwError("invalid boolean operator in tag search expression");

      parse.skipChar();
      parse.skipSpace();

      if (parse.isChar('!'))
        return tk->throwError("too many '!' in tag search expression");

      tokens.push_back(TagToken(TagTokenType::NOT));

      if (! parseSearchTagEnd(parse, endChar, tokens))
        return false;
    }
    else if (parse.isChar('(')) {
      if (! hasExpression()) {
        parse.skipChar();

        TagToken token1(TagTokenType::EXPRESSION);

        std::vector<TagToken> tokens1;

        if (! parseSearchTagEnd(parse, ')', token1.tokens))
          return false;

        if (! parse.isChar(')'))
          return tk->throwError("missing close bracket");

        tokens.push_back(token1);

        parse.skipChar();
        parse.skipSpace();
      }
      else
        tag += parse.readChar();
    }
    else if (parse.isChar('"')) {
      parse.skipChar();

      TagToken token1(TagTokenType::TAG);

      token1.str += tag;

      bool terminated = false;

      while (! parse.eof()) {
        if      (parse.isChar('\\')) {
          parse.skipChar();

          if (parse.eof()) {
            token1.str += '\\';
            break;
          }
        }
        else if (parse.isChar('"')) {
          terminated = true;
          parse.skipChar();
          break;
        }

        token1.str += parse.readChar();
      }

      if (! terminated)
        return tk->throwError("missing endquote in tag search expression");

      if (token1.str == "")
        return tk->throwError("null quoted tag string in tag search expression");

      tokens.push_back(token1);

      tag = "";

      parse.skipSpace();
    }
    else if (parse.isChar('&')) {
      parse.skipChar();

      if (parse.isChar('&')) {
        if (! hasExpression())
          return tk->throwError("unexpected operator in tag search expression");

        parse.skipChar();
        parse.skipSpace();

        flushTag();

        TagToken token1(TagTokenType::AND);

        tokens.push_back(token1);
      }
      else
        tag += "&";
    }
    else if (parse.isChar('|')) {
      parse.skipChar();

      if (parse.isChar('|')) {
        if (! hasExpression())
          return tk->throwError("unexpected operator in tag search expression");

        parse.skipChar();
        parse.skipSpace();

        flushTag();

        TagToken token1(TagTokenType::OR);

        tokens.push_back(token1);
      }
      else
        tag += "|";
    }
    else if (parse.isChar('^')) {
      if (! hasExpression())
        return tk->throwError("unexpected operator in tag search expression");

      parse.skipChar();
      parse.skipSpace();

      flushTag();

      TagToken token1(TagTokenType::XOR);

      tokens.push_back(token1);
    }
    else {
      tag += parse.readChar();

      QString spaces;

      while (parse.isSpace())
        spaces += parse.readChar();

      if (! isSpecial())
        tag += spaces;
    }
  }

  flushTag();

  if (lastIsOperator())
    return tk->throwError("missing tag in tag search expression");

  return true;
}

//---

CTkAppCanvasShape *
CTkAppCanvasWidget::
currentShape() const
{
  return currentShape_.data();
}

void
CTkAppCanvasWidget::
setCurrentShape(Shape *shape)
{
  currentShape_ = shape;
}

CTkAppCanvasShape *
CTkAppCanvasWidget::
focusShape() const
{
  return focusShape_.data();
}

void
CTkAppCanvasWidget::
setFocusShape(Shape *shape)
{
  focusShape_ = shape;
}

//---

CTkAppCanvasShape *
CTkAppCanvasWidget::
getAboveShape(Shape *) const
{
  return nullptr; // TODO
}

void
CTkAppCanvasWidget::
getAboveShapes(Shape *, Shapes &) const
{
  // TODO
}

CTkAppCanvasShape *
CTkAppCanvasWidget::
getBelowShape(Shape *) const
{
  return nullptr; // TODO
}

void
CTkAppCanvasWidget::
getBelowShapes(Shape *, Shapes &) const
{
  // TODO
}

//---

void
CTkAppCanvasWidget::
findEnclosedShapes(const QRectF &r, Shapes &shapes) const
{
  for (auto *shape : shapes_) {
    auto sr = shape->rect();

    if (r.contains(sr))
      shapes.push_back(shape);
  }
}

void
CTkAppCanvasWidget::
findOverlappingShapes(const QRectF &r, Shapes &shapes) const
{
  for (auto *shape : shapes_) {
    if (shape->intersects(r))
      shapes.push_back(shape);
  }
}

void
CTkAppCanvasWidget::
findNearestShapes(const Point &p, Shapes &shapes) const
{
  using DistanceShapes = std::map<double, Shapes>;

  DistanceShapes distanceShapes;

  for (auto *shape : shapes_) {
    auto dist = shape->distance(p.x, p.y);

    distanceShapes[dist].push_back(shape);
  }

  for (const auto &pd : distanceShapes) {
    for (auto *shape : pd.second)
      shapes.push_back(shape);
  }
}

//---

bool
CTkAppCanvasWidget::
saveImage(const QString &imageName, int /*subsample*/, int /*zoom*/, CTkAppImageRef &image)
{
  QString type, format;

  auto *tk = canvas_->app();

  image = tk->getImage(imageName);

  if (! image) {
    image = tk->createImage(type, format, imageName, width(), height());

    (void) tk->addImageCommand(image);
  }
  else {
    image->setType(type);

    image->setFormat(format);

    image->setWidth (width());
    image->setHeight(height());
  }

  auto pixmap = this->grab();

  image->setQImage(pixmap.toImage());

  return true;
}

//---

void
CTkAppCanvasWidget::
resizeEvent(QResizeEvent *)
{
  auto *tk = canvas_->app();

  updateRange();

  if (canvas_->getXScrollCommand() != "") {
    auto cmd = canvas_->getXScrollCommand() + " 0.0 1.0";
    tk->eval(cmd);
  }

  if (canvas_->getYScrollCommand() != "") {
    auto cmd = canvas_->getYScrollCommand() + " 0.0 1.0";
    tk->eval(cmd);
  }
}

void
CTkAppCanvasWidget::
updateRange()
{
  if (scrollRegion_.isValid()) {
    auto sx = scrollX()*scrollRegion_.width ();
    auto sy = scrollY()*scrollRegion_.height();

#if 0
    displayRange_.setPixelRange(scrollRegion_.left (), scrollRegion_.bottom(),
                                scrollRegion_.right(), scrollRegion_.top   ());

//  double xo = scrollRegion_.left() + sx;
//  double yo = scrollRegion_.top () + sy;

//  displayRange_.setWindowRange(xo, yo, xo + width() - 1, yo + height() - 1);
//  displayRange_.setWindowRange(xo, yo, xo + scrollRegion_.width() - 1,
//                               yo + scrollRegion_.height() - 1);
    displayRange_.setWindowRange(0, 0, width() - 1, height() - 1);
#endif
    displayRange_.setWindowRange(scrollRegion_.left () + sx, scrollRegion_.top   () + sy,
                                 scrollRegion_.right() + sx, scrollRegion_.bottom() + sy);

    displayRange_.setPixelRange(0, scrollRegion_.height() - 1, scrollRegion_.width() - 1, 0);
  }
  else {
    displayRange_.setPixelRange(0, height() - 1, width() - 1, 0);

    displayRange_.setWindowRange(0, 0, width() - 1, height() - 1);
  }

  for (auto *s : shapes_)
    s->updateRange();
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

  p->save();
  p->setWorldTransform(s->transform());

  switch (s->type()) {
    case CTkAppCanvasShapeType::RECTANGLE: {
      auto brush = calcBrush(s);
      auto pen   = calcPen(s);

      auto *r = static_cast<Rectangle *>(s);

      auto fillPath   = windowToPixel(r->path());
      auto strokePath = windowToPixel(r->strokePath());

      p->fillPath  (fillPath  , brush);
      p->strokePath(strokePath, pen  );

      break;
    }
    case CTkAppCanvasShapeType::OVAL: {
      auto brush = calcBrush(s);
      auto pen   = calcPen(s);

      auto *o = static_cast<Oval *>(s);

      auto fillPath   = windowToPixel(o->path());
      auto strokePath = windowToPixel(o->strokePath());

      p->fillPath  (fillPath  , brush);
      p->strokePath(strokePath, pen  );

      break;
    }
    case CTkAppCanvasShapeType::LINE: {
      auto *l = static_cast<Line *>(s);

      auto qpaintPath = l->calcStrokePath(l->qpath(), l->pen());
      l->setQStrokePath(qpaintPath);

      auto path = windowToPixel(l->qpath());

      p->drawPath(path);

      break;
    }
#ifdef CTK_APP_TKPATH
    case CTkAppCanvasShapeType::PATH: {
      auto *path = static_cast<Path *>(s);

      auto qpaintPath = path->calcStrokePath(path->qpath(), path->pen());
      path->setQStrokePath(qpaintPath);

      auto qpath = windowToPixel(path->qpath());

      p->drawPath(qpath);

      break;
    }
#endif
    case CTkAppCanvasShapeType::POLYGON: {
      auto *poly = static_cast<Polygon *>(s);

      auto path = windowToPixel(poly->qpath());

      p->drawPath(path);

      break;
    }
    case CTkAppCanvasShapeType::ARC: {
      auto *a = static_cast<Arc *>(s);

      auto path = windowToPixel(a->qpath());

      p->drawPath(path);

      break;
    }
#ifdef CTK_APP_TKPATH
    case CTkAppCanvasShapeType::CIRCLE: {
      auto *c = static_cast<Circle *>(s);

      auto path = windowToPixel(c->qpath());

      p->drawPath(path);

      break;
    }
    case CTkAppCanvasShapeType::ELLIPSE: {
      auto *e = static_cast<Ellipse *>(s);

      auto path = windowToPixel(e->qpath());

      p->drawPath(path);

      break;
    }
#endif
    case CTkAppCanvasShapeType::TEXT: {
      auto *t = static_cast<Text *>(s);

      p->setFont(t->font());

      auto text = t->getText();

      auto pt  = t->calcPos();
      auto pt1 = windowToPixel(pt);

      p->drawText(pt1, text);

      break;
    }
    case CTkAppCanvasShapeType::IMAGE: {
      auto *i = static_cast<Image *>(s);

      auto qimage = i->getImage()->getQImage();

      auto rect = i->calcDrawRect();

      auto rect1 = windowToPixel(rect);

      p->drawImage(rect1, qimage);

      break;
    }
    case CTkAppCanvasShapeType::BITMAP: {
      auto *i = static_cast<Bitmap *>(s);

      auto qimage = i->getImage()->getQImage();

      auto rect = i->calcDrawRect();

      auto rect1 = windowToPixel(rect);

      p->drawImage(rect1, qimage);

      break;
    }
    default:
      break;
  }

  p->restore();
}

QBrush
CTkAppCanvasWidget::
calcBrush(Shape *s) const
{
  if (! s->isEnabled())
    return s->disabledBrush();

  if (s == currentShape())
    return s->activeBrush();

  return s->brush();
}

QPen
CTkAppCanvasWidget::
calcPen(Shape *s) const
{
  if (! s->isEnabled())
    return s->disabledPen();

  if (s == currentShape())
    return s->activePen();

  return s->pen();
}

QSize
CTkAppCanvasWidget::
sizeHint() const
{
  int w = std::max(int(canvas_->getWidth ()), 1);
  int h = std::max(int(canvas_->getHeight()), 1);

  return QSize(w, h);
}

//---

namespace {

class PathVisitor {
 public:
  PathVisitor() = default;

  virtual ~PathVisitor() = default;

  virtual void init() { }
  virtual void term() { }

  virtual void moveTo (const QPointF &p) = 0;
  virtual void lineTo (const QPointF &p) = 0;
  virtual void quadTo (const QPointF &p1, const QPointF &p2) = 0;
  virtual void curveTo(const QPointF &p1, const QPointF &p2, const QPointF &p3) = 0;

 public:
  const QPainterPath *path { nullptr };

  int     i { -1 };
  int     n { 0 };
  QPointF lastP;
  QPointF nextP;
};

void
visitPath(const QPainterPath &path, PathVisitor &visitor)
{
  visitor.path = &path;

  visitor.n = path.elementCount();

  visitor.init();

  for (visitor.i = 0; visitor.i < visitor.n; ++visitor.i) {
    const auto &e = path.elementAt(visitor.i);

    if      (e.isMoveTo()) {
      QPointF p(e.x, e.y);

      if (visitor.i < visitor.n - 1) {
        auto e1 = path.elementAt(visitor.i + 1);

        visitor.nextP = QPointF(e1.x, e1.y);
      }
      else
        visitor.nextP = p;

      visitor.moveTo(p);

      visitor.lastP = p;
    }
    else if (e.isLineTo()) {
      QPointF p(e.x, e.y);

      if (visitor.i < visitor.n - 1) {
        auto e1 = path.elementAt(visitor.i + 1);

        visitor.nextP = QPointF(e1.x, e1.y);
      }
      else
        visitor.nextP = p;

      visitor.lineTo(p);

      visitor.lastP = p;
    }
    else if (e.isCurveTo()) {
      QPointF p(e.x, e.y);

      QPointF p1, p2;

      QPainterPath::ElementType e1t { QPainterPath::MoveToElement };
      QPainterPath::ElementType e2t { QPainterPath::MoveToElement };

      if (visitor.i < visitor.n - 1) {
        auto e1 = path.elementAt(visitor.i + 1);

        e1t = e1.type;

        p1 = QPointF(e1.x, e1.y);
      }

      if (visitor.i < visitor.n - 2) {
        auto e2 = path.elementAt(visitor.i + 2);

        e2t = e2.type;

        p2 = QPointF(e2.x, e2.y);
      }

      if (e1t == QPainterPath::CurveToDataElement) {
        ++visitor.i;

        if (e2t == QPainterPath::CurveToDataElement) {
          ++visitor.i;

          if (visitor.i < visitor.n - 1) {
            auto e3 = path.elementAt(visitor.i + 1);

            visitor.nextP = QPointF(e3.x, e3.y);
          }
          else
            visitor.nextP = p;

          visitor.curveTo(p, p1, p2);

          visitor.lastP = p;
        }
        else {
          if (visitor.i < visitor.n - 1) {
            auto e3 = path.elementAt(visitor.i + 1);

            visitor.nextP = QPointF(e3.x, e3.y);
          }
          else
            visitor.nextP = p;

          visitor.quadTo(p, p1);

          visitor.lastP = p;
        }
      }
    }
    else
      assert(false);
  }

  visitor.term();
}

}

QPainterPath
CTkAppCanvasWidget::
windowToPixel(const QPainterPath &path) const
{
  class PathVisitor1 : public PathVisitor {
   public:
    PathVisitor1(const CTkAppCanvasWidget *canvas) :
     canvas_(canvas) {
    }

    void moveTo(const QPointF &p) override {
      auto pp = canvas_->windowToPixel(p);

      path_.moveTo(pp);
    }

    void lineTo(const QPointF &p) override {
      auto pp = canvas_->windowToPixel(p);

      path_.lineTo(pp);
    }

    void quadTo(const QPointF &p1, const QPointF &p2) override {
      auto pp1 = canvas_->windowToPixel(p1);
      auto pp2 = canvas_->windowToPixel(p2);

      path_.quadTo(pp1, pp2);
    }

    void curveTo(const QPointF &p1, const QPointF &p2, const QPointF &p3) override {
      auto pp1 = canvas_->windowToPixel(p1);
      auto pp2 = canvas_->windowToPixel(p2);
      auto pp3 = canvas_->windowToPixel(p3);

      path_.cubicTo(pp1, pp2, pp3);
    }

    const QPainterPath &path() const { return path_; }

   private:
    const CTkAppCanvasWidget* canvas_ { nullptr };
    QPainterPath              path_;
  };

  PathVisitor1 visitor(this);

  visitPath(path, visitor);

  return visitor.path();
}

QRectF
CTkAppCanvasWidget::
windowToPixel(const QRectF &r) const
{
  auto p1 = windowToPixel(r.topLeft    ());
  auto p2 = windowToPixel(r.bottomRight());

  return QRectF(p1, p2);
}

QPointF
CTkAppCanvasWidget::
windowToPixel(const QPointF &p) const
{
  double px, py;
  displayRange_.windowToPixel(p.x(), p.y(), &px, &py);

  return QPointF(px, py);
}

CTkAppPoint
CTkAppCanvasWidget::
windowToPixel(const Point &w) const
{
  auto p1 = windowToPixel(QPointF(w.x, w.y));

  return Point(p1.x(), p1.y());
}

QPointF
CTkAppCanvasWidget::
pixelToWindow(const QPointF &p) const
{
  double wx, wy;
  displayRange_.pixelToWindow(p.x(), p.y(), &wx, &wy);

  return QPointF(wx, wy);
}

CTkAppPoint
CTkAppCanvasWidget::
pixelToWindow(const Point &p) const
{
  auto w1 = pixelToWindow(QPointF(p.x, p.y));

  return Point(w1.x(), w1.y());
}
