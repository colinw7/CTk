#include <CTkAppCanvasShape.h>
#include <CTkAppCanvasWidget.h>
#include <CTkAppCanvas.h>
#include <CTkAppWidget.h>
#include <CTkAppImage.h>

#include <CQStrParse.h>

#include <QPainter>

CTkAppCanvasShape::
CTkAppCanvasShape(CTkAppCanvasWidget *canvas, ShapeType type) :
 QObject(canvas), canvas_(canvas), type_(type)
{
  id_ = canvas_->nextShapeId();
}

QString
CTkAppCanvasShape::
typeName() const
{
  switch (type()) {
    case ShapeType::ARC      : return "arc";
    case ShapeType::BITMAP   : return "bitmap";
#ifdef CTK_APP_TKPATH
    case ShapeType::CIRCLE   : return "circle";
    case ShapeType::ELLIPSE  : return "ellipse";
#endif
    case ShapeType::IMAGE    : return "image";
    case ShapeType::LINE     : return "line";
    case ShapeType::OVAL     : return "oval";
#ifdef CTK_APP_TKPATH
    case ShapeType::PATH     : return "path";
#endif
    case ShapeType::POLYGON  : return "polygon";
    case ShapeType::RECTANGLE: return "rectangle";
    case ShapeType::TEXT     : return "text";
    case ShapeType::WINDOW   : return "window";
    default                  : return "";
  }
}

QString
CTkAppCanvasShape::
tagsStr() const
{
  std::vector<QString> strs;
  for (const auto &tag : tags_)
    strs.push_back(tag);

  auto *tk = canvas()->canvas()->app();

  return tk->mergeList(strs);
}

bool
CTkAppCanvasShape::
setTagsStr(const QString &str)
{
  auto *tk = canvas()->canvas()->app();

  std::vector<QString> strs;
  if (! tk->splitList(str, strs))
    return false;

  std::set<QString> tags;
  for (const auto &s : strs)
    tags.insert(s);

  setTags(tags);

  return true;
}

void
CTkAppCanvasShape::
addTag(const QString &tag)
{
  auto pt = tags_.find(tag);

  if (pt == tags_.end())
    tags_.insert(tag);
}

void
CTkAppCanvasShape::
deleteTag(const QString &tag)
{
  auto pt = tags_.find(tag);

  if (pt != tags_.end())
    tags_.erase(pt);
}

//---

bool
CTkAppCanvasShape::
intersects(const QRectF &r) const
{
  return rect().intersects(r);
}

void
CTkAppCanvasShape::
scale(double xc, double yc, double sx, double sy)
{
  Points points;
  getPoints(points);

  for (uint i = 0; i < points.size(); ++i) {
    const auto &p = points[i];

    double x1 = (p.x - xc)*sx + xc;
    double y1 = (p.y - yc)*sy + yc;

    points[i] = Point(x1, y1);
  }

  setPoints(points);
}

void
CTkAppCanvasShape::
rotate(double xc, double yc, double a)
{
  auto ra = M_PI*a/180;

  auto s = std::sin(-ra);
  auto c = std::cos(-ra);

  Points points;
  getPoints(points);

  for (uint i = 0; i < points.size(); ++i) {
    const auto &p = points[i];

    double x1 = (p.x - xc);
    double y1 = (p.y - yc);

    double x2 = x1*c - y1*s;
    double y2 = x1*s + y1*c;

    points[i] = Point(x2 + xc, y2 + yc);
  }

  setPoints(points);
}

//---

bool
CTkAppCanvasShape::
setStipple(const QString &str)
{
  auto *tk = canvas()->canvas()->app();

  auto image = tk->getBitmap(str);
  if (! image) return false;

  auto b = brush();
  b.setTexture(image->getQPixmap());
  setBrush(b);

  stipple_ = str;

  return true;
}

bool
CTkAppCanvasShape::
setOutlineStipple(const QString &str)
{
  auto *tk = canvas()->canvas()->app();

  auto image = tk->getBitmap(str);
  if (! image) return false;

  auto b = outlineBrush();
  b.setTexture(image->getQPixmap());
  setOutlineBrush(b);

  outlineStipple_ = str;

  return true;
}

QPainterPath
CTkAppCanvasShape::
calcStrokePath(const QPainterPath &path, const QPen &pen) const
{
  QPainterPathStroker stroker;

  stroker.setCapStyle   (pen.capStyle());
  stroker.setDashOffset (pen.dashOffset());
  stroker.setDashPattern(pen.dashPattern());
  stroker.setJoinStyle  (pen.joinStyle());
  stroker.setMiterLimit (pen.miterLimit());
  stroker.setWidth      (pen.widthF());

  return stroker.createStroke(path);
}

bool
CTkAppCanvasShape::
stringToOffsetData(CTclApp *app, const QString &str, OffsetData &offsetData)
{
  offsetData = OffsetData();

  if (CTkAppUtil::variantToAlign(app, str, offsetData.align)) {
    offsetData.isAlign = true;
    return true;
  }

  CQStrParse parse(str);

  if (parse.isChar('#')) {
    offsetData.toplevel = true;
    parse.skipChar();
  }

  if (! parse.readInteger(&offsetData.x))
    return false;

  if (! parse.isChar(','))
    return false;

  if (! parse.readInteger(&offsetData.y))
    return false;

  return true;
}

QString
CTkAppCanvasShape::
offsetDataToString(const OffsetData &offsetData)
{
  if (offsetData.isAlign)
    return CTkAppUtil::alignToString(offsetData.align);

  QString str;

  if (offsetData.toplevel)
    str += "#";

  str + QString::number(offsetData.x) + "," + QString::number(offsetData.y);

  return str;
}

bool
CTkAppCanvasShape::
getShapeDefault(const QString &name, QVariant &value) const
{
  if      (name == "-activewidth"  ) value = "0.0";
  else if (name == "-anchor"       ) value = "center";
  else if (name == "-angle"        ) value = "0.0";
  else if (name == "-dashoffset"   ) value = "0";
  else if (name == "-disabledwidth") value = "0.0";
  else if (name == "-fill"         ) value = "#000000";
  else if (name == "-font"         ) value = "TkDefaultFont";
  else if (name == "-justify"      ) value = "left";
  else if (name == "-offset"       ) value = "0,0";
  else if (name == "-outline"      ) value = "#000000";
  else if (name == "-outlineoffset") value = "0,0";
  else if (name == "-underline"    ) value = "-1";
  else if (name == "-width"        ) value = "1.0";
  else                               value = "";

  return true;
}

bool
CTkAppCanvasShape::
getShapeOpt(const QString &name, QVariant &value) const
{
  auto *tk = canvas()->canvas()->app();

  auto dashToString = [&](const QVector<qreal> &dashes) {
    std::vector<QString> strs;
    for (const auto &r : dashes)
      strs.push_back(QString::number(r));
    return tk->mergeList(strs);
  };

  if      (name == "-anchor") {
    value = CTkAppUtil::alignToString(this->anchor());
  }
  else if (name == "-dash") {
    auto p = this->pen();
    value = dashToString(p.dashPattern());
  }
  else if (name == "-activedash") {
    auto p = this->activePen();
    value = dashToString(p.dashPattern());
  }
  else if (name == "-disableddash") {
    auto p = this->disabledPen();
    value = dashToString(p.dashPattern());
  }
  else if (name == "-dashoffset") {
    auto p = this->disabledPen();
    value = tk->realToVariant(p.dashOffset());
  }
  else if (name == "-fill") {
    auto b = this->brush();
    if (b.style() != Qt::NoBrush)
      value = tk->colorToVariant(this->fillColor());
    else
      value = "";
  }
  else if (name == "-fillrule") { // tkpath
    auto fillRule = this->fillRule();
    value = CTkAppUtil::fillRuleToString(fillRule);
  }
  else if (name == "-fillopacity") {
    value = tk->realToVariant(this->fillAlpha());
  }
  else if (name == "-activefill") {
    auto b = this->activeBrush();
    value = tk->colorToVariant(b.color());
  }
  else if (name == "-disabledfill") {
    auto b = this->disabledBrush();
    value = tk->colorToVariant(b.color());
  }
  else if (name == "-filloverstroke") { // tkpath
    auto p = this->fillOverPen();
    value = tk->colorToVariant(p.color());
  }
  else if (name == "-outline" || name == "-stroke") {
    value = tk->colorToVariant(this->strokeColor());
  }
  else if (name == "-activeoutline") {
    auto p = this->activePen();
    value = tk->colorToVariant(p.color());
  }
  else if (name == "-disabledoutline") {
    auto p = this->disabledPen();
    value = tk->colorToVariant(p.color());
  }
  else if (name == "-offset") {
    value = this->offsetDataToString(this->stippleOffset());
  }
  else if (name == "-outlinestipple") {
    value = this->outlineStipple();
  }
  else if (name == "-activeoutlinestipple") {
    tk->TODO(name);
  }
  else if (name == "-disabledoutlinestipple") {
    tk->TODO(name);
  }
  else if (name == "-outlineoffset") {
    value = this->offsetDataToString(this->stippleOutlineOffset());
  }
  else if (name == "-stipple") {
    value = this->stipple();
  }
  else if (name == "-activestipple") {
    tk->TODO(name);
  }
  else if (name == "-disabledstipple") {
    tk->TODO(name);
  }
  else if (name == "-state") {
    if      (! this->isVisible()) value = "hidden";
    else if (! this->isEnabled()) value = "disabled";
    else                          value = "normal";
  }
  else if (name == "-fontfamily") {
    value = this->fontFamily();
  }
  else if (name == "-fontsize") {
    value = tk->realToVariant(this->fontSize());
  }
  else if (name == "-tags" || name == "-tag") {
    value = this->tagsStr();
  }
  else if (name == "-width" || name == "-strokewidth") {
    auto p = this->pen();
    value = tk->realToVariant(p.widthF());
  }
  else if (name == "-strokelinecap") {
    auto capStyle = this->strokeCap();
    value = CTkAppUtil::capStyleToString(capStyle);
  }
  else if (name == "-strokeopacity") {
    value = tk->realToVariant(this->strokeAlpha());
  }
  else if (name == "-activewidth") {
    auto p = this->activePen();
    value = tk->realToVariant(p.widthF());
  }
  else if (name == "-disabledwidth") {
    value = tk->realToVariant(this->strokeWidth());
  }
#ifdef CTK_APP_TKPATH
  else if (name == "-matrix") {
    tk->TODO(name);
  }
#endif
  else if (name == "-parent") {
    tk->TODO(name);
  }
  else if (name == "-style") { // tkpath
    tk->TODO(name);
  }
  else {
    tk->TODO(name);
    return false;
  }

  return true;
}

// returns true if handled, rc = false if set name/value failed
bool
CTkAppCanvasShape::
setShapeOpt(const QString &name, const QVariant &var, bool &rc)
{
  auto *tk = canvas()->canvas()->app();

  //---

  auto stringToDashes = [&](const QString &str, QVector<qreal> &dashes) {
    std::vector<QString> strs;
    if (! tk->splitList(str, strs))
      return false;

    for (const auto &s : strs) {
      double r;
      if (CTkAppUtil::stringToReal(s, r)) {
        dashes.push_back(r);
      }
      else {
        CQStrParse parse(s);
        while (! parse.eof()) {
          if      (parse.isChar('.')) { dashes.push_back(2); dashes.push_back(4); }
          else if (parse.isChar('-')) { dashes.push_back(6); dashes.push_back(4); }
          else if (parse.isChar(',')) { dashes.push_back(4); dashes.push_back(4); }
          else if (parse.isChar(' ')) {
            if (! dashes.empty()) dashes.pop_back();
            dashes.push_back(8);
          }
          else
            return false;

          parse.skipChar();
        }
      }
    }

    return true;
  };

  auto variantToStroke = [&](const QVariant &var, QPen &stroke) {
    auto value = tk->variantToString(var);

    if (value == "")
      stroke = QPen(Qt::NoPen);
    else {
      QColor c;
      if (! CTkAppUtil::variantToQColor(tk, var, c)) {
        rc = tk->throwError(tk->msg() + "unknown color name \"" + var + "\"");
        return true;
      }
      stroke.setColor(c);
    }
    return true;
  };

  auto variantToFill = [&](const QVariant &var, QBrush &fill) {
    auto value = tk->variantToString(var);

    QGradient *g;

    if      (value == "")
      fill = QBrush(Qt::NoBrush);
    else if (canvas()->canvas()->getGradient(value, g))
      fill = QBrush(*g);
    else {
      QColor c;
      if (! CTkAppUtil::variantToQColor(tk, var, c)) {
        rc = tk->throwError(tk->msg() + "unknown color name \"" + var + "\"");
        return true;
      }

      fill.setStyle(Qt::SolidPattern);
      fill.setColor(c);
    }
    return true;
  };

  //---

  rc = true;

  auto value = tk->variantToString(var);

  if      (name == "-anchor") {
    Qt::Alignment align;
    if (! tk->variantToAlign(var, align)) {
      rc = tk->throwError(tk->msg() + "bad anchor position \"" + var + "\": must be "
                          "n, ne, e, se, s, sw, w, nw, or center");
      return true;
    }

    this->setAnchor(align);
  }
  else if (name == "-dash") {
    QVector<qreal> dashes;
    if (! stringToDashes(value, dashes)) {
      rc = tk->throwError(tk->msg() + "Invalid dash \"" + var + "\"");
      return true;
    }

    auto p = this->pen();
    p.setDashPattern(dashes);
    this->setPen(p);
  }
  else if (name == "-activedash") {
    QVector<qreal> dashes;
    if (! stringToDashes(value, dashes)) {
      rc = tk->throwError(tk->msg() + "Invalid dash \"" + var + "\"");
      return true;
    }

    auto p = this->activePen();
    p.setDashPattern(dashes);
    this->setActivePen(p);
  }
  else if (name == "-disableddash") {
    QVector<qreal> dashes;
    if (! stringToDashes(value, dashes)) {
      rc = tk->throwError(tk->msg() + "Invalid dash \"" + var + "\"");
      return true;
    }

    auto p = this->disabledPen();
    p.setDashPattern(dashes);
    this->setDisabledPen(p);
  }
  else if (name == "-dashoffset") {
    CTkAppDistance l;
    if (! tk->variantToDistance(var, l)) {
      rc = tk->throwError(tk->msg() + "Invalid distance \"" + var + "\"");
      return true;
    }

    auto p = this->pen();
    p.setDashOffset(l.rvalue);
    this->setPen(p);
  }
  else if (name == "-fill") {
    auto b = this->brush();
    if (tk->variantIsValid(var)) {
      if (! variantToFill(var, b)) {
        rc = tk->throwError(tk->msg() + "unknown color name \"" + var + "\"");
        return true;
      }
    }
    else
      b = Qt::NoBrush;

    this->setBrush(b);
  }
  else if (name == "-fillrule") { // tkpath
    Qt::FillRule fillRule;
    if (! CTkAppUtil::variantToFillRule(tk, var, fillRule)) {
      rc = tk->throwError(tk->msg() + "Invalid fill rule \"" + var + "\"");
      return true;
    }

    this->setFillRule(fillRule);
  }
  else if (name == "-fillopacity") {
    double a;
    if (! CTkAppUtil::variantToReal(tk, var, a)) {
      rc = tk->invalidReal(var);
      return true;
    }

    this->setFillAlpha(a);
  }
  else if (name == "-activefill") {
    auto b = this->activeBrush();
    if (! variantToFill(var, b)) {
      rc = tk->throwError(tk->msg() + "unknown color name \"" + var + "\"");
      return true;
    }
    this->setActiveBrush(b);
  }
  else if (name == "-disabledfill") {
    auto b = this->disabledBrush();
    if (! variantToFill(var, b)) {
      rc = tk->throwError(tk->msg() + "unknown color name \"" + var + "\"");
      return true;
    }
    this->setDisabledBrush(b);
  }
  else if (name == "-filloverstroke") { // tkpath
    auto p = this->fillOverPen();
    if (! variantToStroke(var, p)) {
      rc = tk->throwError(tk->msg() + "unknown color name \"" + var + "\"");
      return true;
    }
    this->setFillOverPen(p);
  }
  else if (name == "-outline" || name == "-stroke") {
    auto p = this->pen();
    if (! variantToStroke(var, p)) {
      rc = tk->throwError(tk->msg() + "unknown color name \"" + var + "\"");
      return true;
    }
    this->setPen(p);
  }
  else if (name == "-activeoutline") {
    auto p = this->activePen();
    if (! variantToStroke(var, p)) {
      rc = tk->throwError(tk->msg() + "unknown color name \"" + var + "\"");
      return true;
    }
    this->setActivePen(p);
  }
  else if (name == "-disabledoutline") {
    auto p = this->disabledPen();
    if (! variantToStroke(var, p)) {
      rc = tk->throwError(tk->msg() + "unknown color name \"" + var + "\"");
      return true;
    }
    this->setDisabledPen(p);
  }
  else if (name == "-offset") {
    CTkAppCanvasShape::OffsetData offsetData;
    if (! this->stringToOffsetData(tk, value, offsetData)) {
      rc = tk->throwError(tk->msg() + "Invalid offset \"" + var + "\"");
      return true;
    }

    this->setStippleOffset(offsetData);
  }
  else if (name == "-outlinestipple") {
    this->setOutlineStipple(value);
  }
  else if (name == "-activeoutlinestipple") {
    auto image = tk->getBitmap(value);
    if (! image) {
      rc = tk->throwError(tk->msg() + "Invalid image \"" + var + "\"");
      return true;
    }

    auto b = this->activeOutlineBrush();
    b.setTexture(image->getQPixmap());
    this->setActiveOutlineBrush(b);
  }
  else if (name == "-disabledoutlinestipple") {
    auto image = tk->getBitmap(value);
    if (! image) {
      rc = tk->throwError(tk->msg() + "Invalid image \"" + var + "\"");
      return true;
    }

    auto b = this->disabledOutlineBrush();
    b.setTexture(image->getQPixmap());
    this->setDisabledOutlineBrush(b);
  }
  else if (name == "-outlineoffset") {
    CTkAppCanvasShape::OffsetData offsetData;
    if (! this->stringToOffsetData(tk, value, offsetData)) {
      rc = tk->throwError(tk->msg() + "Invalid offset \"" + var + "\"");
      return true;
    }

    this->setStippleOutlineOffset(offsetData);
  }
  else if (name == "-stipple") {
    if (tk->variantIsValid(value)) {
      auto image = tk->getBitmap(value);
      if (! image) {
        rc = tk->throwError(tk->msg() + "bitmap \"" + var + "\" not defined");
        return true;
      }
      this->setStipple(value);
    }
    else
      this->setStipple("");
  }
  else if (name == "-activestipple") {
    auto image = tk->getBitmap(value);
    if (! image) {
      rc = tk->throwError(tk->msg() + "Invalid image \"" + var + "\"");
      return true;
    }

    auto b = this->activeBrush();
    b.setTexture(image->getQPixmap());
    this->setActiveBrush(b);
  }
  else if (name == "-disabledstipple") {
    auto image = tk->getBitmap(value);
    if (! image) {
      rc = tk->throwError(tk->msg() + "Invalid image \"" + var + "\"");
      return true;
    }

    auto b = this->disabledBrush();
    b.setTexture(image->getQPixmap());
    this->setDisabledBrush(b);
  }
  else if (name == "-state") {
    CTkAppWidgetState state;
    if (! CTkAppUtil::variantToState(tk, var, state)) {
      rc = tk->throwError(tk->msg() + "bad state \"" + var + "\": must be "
                           "active, disabled, or normal");
      return true;
    }

    if      (state == CTkAppWidgetState::NORMAL) {
      this->setEnabled(true); this->setVisible(true);
    }
    else if (state == CTkAppWidgetState::DISABLED) this->setEnabled(false);
    else if (state == CTkAppWidgetState::HIDDEN  ) this->setVisible(false);
  }
  else if (name == "-fontfamily") {
    this->setFontFamily(value);
  }
  else if (name == "-fontsize") {
    long s;
    if (! CTkAppUtil::stringToInt(value, s)) {
      rc = tk->throwError(tk->msg() + "Invalid integer \"" + var + "\"");
      return true;
    }

    this->setFontSize(s);
  }
  else if (name == "-tags" || name == "-tag") {
    if (! this->setTagsStr(value)) {
      rc = tk->throwError(tk->msg() + "Invalid tags \"" + var + "\"");
      return true;
    }
  }
  else if (name == "-width" || name == "-strokewidth") {
    CTkAppDistance width;
    if (! tk->variantToDistance(var, width)) {
      rc = tk->invalidDistance(var);
      return true;
    }

    this->setStrokeWidth(std::max(width.rvalue, 0.0));
  }
  else if (name == "-strokelinecap") {
    Qt::PenCapStyle capStyle;
    if (! CTkAppUtil::variantToCapStyle(tk, var, capStyle)) {
      rc = tk->throwError(tk->msg() + "Invalid cap style \"" + var + "\"");
      return true;
    }

    this->setStrokeCap(capStyle);
  }
  else if (name == "-strokeopacity") {
    double a;
    if (! CTkAppUtil::variantToReal(tk, var, a)) {
      rc = tk->invalidReal(var);
      return true;
    }

    this->setStrokeAlpha(a);
  }
  else if (name == "-activewidth") {
    CTkAppDistance width;
    if (! tk->variantToDistance(var, width)) {
      rc = tk->invalidDistance(var);
      return true;
    }

    auto p = this->activePen();
    p.setWidthF(width.rvalue);
    this->setActivePen(p);
  }
  else if (name == "-disabledwidth") {
    CTkAppDistance width;
    if (! tk->variantToDistance(var, width)) {
      rc = tk->invalidDistance(var);
      return true;
    }

    auto p = this->disabledPen();
    p.setWidthF(width.rvalue);
    this->setDisabledPen(p);
  }
#ifdef CTK_APP_TKPATH
  else if (name == "-matrix") {
    CTkApp::MatrixData mdata;

    if (tk->getNamedMatrix(value, mdata)) {
      this->setTransform(mdata.transform);
    }
    else {
      QTransform t;
      if (! CTkAppUtil::stringToMatrix(tk, value, t)) {
        rc = tk->throwError(tk->msg() + "Invalid matrix \"" + var + "\"");
        return true;
      }
    }
  }
#endif
  else {
    return false;
  }

  return true;
}

bool
CTkAppCanvasShape::
getItemPoints(const QString &item, Points &points) const
{
  auto *tk = canvas()->canvas()->app();

  std::vector<QString> items;
  if (! tk->splitList(item, items))
    return false;

  if (items.size() & 1)
    return false;

  uint i = 0;

  while (i < items.size()) {
    CTkAppDistance x, y;
    if (! tk->variantToDistance(items[i], x) || ! tk->variantToDistance(items[i + 1], y))
      return false;

    points.push_back(CTkAppPoint(x.rvalue, y.rvalue));

    i += 2;
  }

  return true;
}

//---

bool
CTkAppCanvasShape::
movePoint(const QString &ind, const Point &p)
{
//std::cerr << "CTkAppCanvasShape::movePoint " << ind.toStdString() << " " <<
//             p.x << " " << p.y << "\n";

  auto *tk = canvas()->canvas()->app();

  IndexData indexData;
  if (! parseIndex(ind, indexData))
    return tk->throwError(tk->msg() + "bad index \"" + ind + "\"");

  Points points;
  getPoints(points);

  long iind = -1;
  if (! mapIndex(indexData, iind))
    return false;

  if (iind >= 0 && iind < int(points.size()))
    points[iind] = p;

  setPoints(points);

  return true;
}

//---

bool
CTkAppCanvasShape::
indexPos(const QString &ind, long &pos) const
{
  auto *tk = canvas()->canvas()->app();

  IndexData indexData;
  if (! parseIndex(ind, indexData))
    return tk->throwError(tk->msg() + "bad index \"" + ind + "\"");

  if (! mapIndex(indexData, pos))
    return false;

  return true;
}

//---

bool
CTkAppCanvasShape::
insertChars(const QString &ind, const QString &str)
{
  std::cerr << "CTkAppCanvasShape::insertChars " << ind.toStdString() << " " <<
               str.toStdString() << "\n";

  auto *tk = canvas()->canvas()->app();

  IndexData indexData;
  if (! parseIndex(ind, indexData))
    return tk->throwError(tk->msg() + "bad index \"" + ind + "\"");

  tk->TODO("insert " + ind + " " + str);

  return false;
}

bool
CTkAppCanvasShape::
deleteChars(const QString &start, const QString &end)
{
  std::cerr << "CTkAppCanvasShape::deleteChars " << start.toStdString() << " " <<
               end.toStdString() << "\n";

  return false;
}

bool
CTkAppCanvasShape::
replaceChars(const QString &start, const QString &end, const QString &str)
{
  std::cerr << "CTkAppCanvasShape::replaceChars " << start.toStdString() << " " <<
               end.toStdString() << " " << str.toStdString() << "\n";

  return false;
}

//---

bool
CTkAppCanvasShape::
setInsertCursor(const QString &ind)
{
  auto *tk = canvas()->canvas()->app();

  IndexData indexData;
  if (! parseIndex(ind, indexData))
    return tk->throwError(tk->msg() + "bad index \"" + ind + "\"");

  insertCursor_ = indexData;

  return false;
}

bool
CTkAppCanvasShape::
setSelectFrom(const QString &ind)
{
  auto *tk = canvas()->canvas()->app();

  IndexData indexData;
  if (! parseIndex(ind, indexData))
    return tk->throwError(tk->msg() + "bad index \"" + ind + "\"");

  selectFrom_ = indexData;

  return true;
}

bool
CTkAppCanvasShape::
setSelectTo(const QString &ind)
{
  auto *tk = canvas()->canvas()->app();

  IndexData indexData;
  if (! parseIndex(ind, indexData))
    return tk->throwError(tk->msg() + "bad index \"" + ind + "\"");

  selectTo_ = indexData;

  (void) updateSelection();

  return true;
}

bool
CTkAppCanvasShape::
adjustSelect(const QString &ind)
{
  auto *tk = canvas()->canvas()->app();

  IndexData indexData;
  if (! parseIndex(ind, indexData))
    return tk->throwError(tk->msg() + "bad index \"" + ind + "\"");

  long istart, iend, iind;
  if (! mapIndex(selectFrom_, istart) || ! mapIndex(selectTo_, iend) || ! mapIndex(indexData, iind))
    return false;

  if      (iind < istart)
    selectFrom_ = indexData;
  else if (iind > iend)
    selectTo_   = indexData;

  (void) updateSelection();

  return true;
}

bool
CTkAppCanvasShape::
clearSelect()
{
  selectFrom_ = IndexData();
  selectTo_   = IndexData();

  return true;
}

//------

bool
CTkAppCanvasShape::
parseIndex(const QString &str, IndexData &indexData) const
{
  auto *tk = canvas()->canvas()->app();

  if      (str == "end")
    indexData.type = IndexType::END;
  else if (str == "insert")
    indexData.type = IndexType::INSERT;
  else if (str == "sel.first")
    indexData.type = IndexType::SEL_FIRST;
  else if (str == "sel.last")
    indexData.type = IndexType::SEL_LAST;
  else if (str.left(1) == "@") {
    indexData.type = IndexType::COORD;

    auto strs = str.mid(1).split(",");
    if (strs.length() != 2) return false;

    if (! tk->variantToInt(strs[0], indexData.x))
      return false;
    if (! tk->variantToInt(strs[1], indexData.y))
      return false;
  }
  else {
    indexData.type = IndexType::POS;

    if (! tk->variantToInt(str, indexData.ind))
      return false;
  }

  return true;
}

bool
CTkAppCanvasShape::
mapIndex(const IndexData &indexData, long &pos) const
{
  auto *tk = canvas()->canvas()->app();

  pos = -1;

  if      (indexData.type == IndexType::POS) {
    pos = indexData.ind;

    return true;
  }
  else if (indexData.type == IndexType::END) {
    // end pos ?
  }
  else if (indexData.type == IndexType::INSERT) {
    return mapIndex(insertCursor_, pos);
  }
  else if (indexData.type == IndexType::SEL_FIRST) {
    return mapIndex(selectFrom_, pos);
  }
  else if (indexData.type == IndexType::SEL_LAST) {
    return mapIndex(selectTo_, pos);
  }
  else if (indexData.type == IndexType::COORD) {
    Points points;
    getPoints(points);

    auto p = Point(indexData.x, indexData.y);

    for (uint i = 0; i < points.size(); ++i) {
      if (points[i] == p) {
        pos = i;
        return true;
      }
    }
  }

  return tk->throwError(tk->msg() + "bad index");
}

//---

CTkAppCanvasArcShape::
CTkAppCanvasArcShape(CTkAppCanvasWidget *canvas, const Points &points) :
 CTkAppCanvasShape(canvas, ShapeType::ARC), points_(points)
{
  updatePath();
}

bool
CTkAppCanvasArcShape::
getShapeOpt(const QString &name, QVariant &value) const
{
  auto *tk = canvas()->canvas()->app();

  if      (name == "-extent") {
    value = tk->realToVariant(extent());
  }
  else if (name == "-start") {
    value = tk->realToVariant(start());
  }
  else if (name == "-height") {
    if (height())
      value = *height();
  }
  else if (name == "-style") {
    if      (arcType() == CTkAppCanvasArcShape::ArcType::PIE  ) value = "pieslice";
    else if (arcType() == CTkAppCanvasArcShape::ArcType::CHORD) value = "chord";
    else if (arcType() == CTkAppCanvasArcShape::ArcType::ARC  ) value = "arc";
  }
  else
    return CTkAppCanvasShape::getShapeOpt(name, value);

  return true;
}

// returns true if handled, rc = false if set name/value failed
bool
CTkAppCanvasArcShape::
setShapeOpt(const QString &name, const QVariant &value, bool &rc)
{
  rc = true;

  auto *tk = canvas()->canvas()->app();

  if      (name == "-extent") {
    double extent;
    rc = tk->variantToReal(value, extent);
    if (rc) setExtent(extent);
  }
  else if (name == "-start") {
    double start;
    rc = tk->variantToReal(value, start);
    if (rc) setStart(start);
  }
  else if (name == "-height") {
    CTkAppDistance height;
    rc = tk->variantToDistance(value, height);
    if (rc) setHeight(height.rvalue);
  }
  else if (name == "-style") {
    static auto styleNames = std::vector<QString>({"pieslice", "chord", "arc"});

    QString value1;
    if (! tk->lookupName("-style", styleNames, value, value1)) {
      rc = false; return true;
    }

    // pieslice, chord, arc
    CTkAppCanvasArcShape::ArcType arcType { CTkAppCanvasArcShape::ArcType::NONE };
    if      (value1 == "pieslice") arcType = CTkAppCanvasArcShape::ArcType::PIE;
    else if (value1 == "chord"   ) arcType = CTkAppCanvasArcShape::ArcType::CHORD;
    else if (value1 == "arc"     ) arcType = CTkAppCanvasArcShape::ArcType::ARC;
    else rc = false;
    if (rc) setArcType(arcType);
  }
  else
    return CTkAppCanvasShape::setShapeOpt(name, value, rc);

  return true;
}

//---

CTkAppCanvasBitmapShape::
CTkAppCanvasBitmapShape(CTkAppCanvasWidget *canvas, const Point &p, const CTkAppImageRef &image) :
 CTkAppCanvasShape(canvas, ShapeType::BITMAP), p_(p), image_(image)
{
  calcDrawRect();
}

void
CTkAppCanvasBitmapShape::
setPos(const Point &p)
{
  p_ = p;

  calcDrawRect();
}

void
CTkAppCanvasBitmapShape::
setImage(const CTkAppImageRef &i)
{
  image_ = i;

  calcDrawRect();
}

QRectF
CTkAppCanvasBitmapShape::
calcDrawRect() const
{
  if (! getImage())
    return QRectF();

  auto qimage = getImage()->getQImage();

  auto iw = qimage.width();
  auto ih = qimage.height();

  auto rect = QRectF(pos().x, pos().y, iw, ih);

  auto *th = const_cast<CTkAppCanvasBitmapShape *>(this);

  th->setDrawRect(rect);

  return rect;
}

bool
CTkAppCanvasBitmapShape::
getShapeOpt(const QString &name, QVariant &value) const
{
  auto *tk = canvas()->canvas()->app();

  if      (name == "-background") {
    value = tk->colorToVariant(this->background());
  }
  else if (name == "-activebackground") {
    value = tk->colorToVariant(this->activeBackground());
  }
  else if (name == "-disabledbackground") {
    value = tk->colorToVariant(this->disabledBackground());
  }
  else if (name == "-bitmap") {
    value = tk->imageToVariant(this->getImage());
  }
  else if (name == "-activebitmap") {
    value = tk->imageToVariant(this->getActiveImage());
  }
  else if (name == "-disabledbitmap") {
    value = tk->imageToVariant(this->getDisabledImage());
  }
  else if (name == "-foreground") {
    value = tk->colorToVariant(this->foreground());
  }
  else if (name == "-activeforeground") {
    value = tk->colorToVariant(this->activeForeground());
  }
  else if (name == "-disabledforeground") {
    value = tk->colorToVariant(this->disabledForeground());
  }
  else
    return CTkAppCanvasShape::getShapeOpt(name, value);

  return true;
}

// returns true if handled, rc = false if set name/value failed
bool
CTkAppCanvasBitmapShape::
setShapeOpt(const QString &name, const QVariant &value, bool &rc)
{
  rc = true;

  auto *tk = canvas()->canvas()->app();

  if      (name == "-background") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk, value, c)) {
      rc = tk->throwError(tk->msg() + "unknown color name \"" + value + "\"");
      return true;
    }

    this->setBackground(c);
  }
  else if (name == "-activebackground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk, value, c)) {
      rc = tk->throwError(tk->msg() + "unknown color name \"" + value + "\"");
      return true;
    }

    this->setActiveBackground(c);
  }
  else if (name == "-disabledbackground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk, value, c)) {
      rc = tk->throwError(tk->msg() + "unknown color name \"" + value + "\"");
      return true;
    }

    this->setDisabledBackground(c);
  }
  else if (name == "-bitmap") {
    auto str = tk->variantToString(value);
    auto image = tk->getBitmap(str);

    this->setImage(image);
  }
  else if (name == "-activebitmap") {
    auto str = tk->variantToString(value);
    auto image = tk->getBitmap(str);

    this->setActiveImage(image);
  }
  else if (name == "-disabledbitmap") {
    auto str = tk->variantToString(value);
    auto image = tk->getBitmap(str);

    this->setDisabledImage(image);
  }
  else if (name == "-foreground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk, value, c)) {
      rc = tk->throwError(tk->msg() + "unknown color name \"" + value + "\"");
      return true;
    }

    this->setForeground(c);
  }
  else if (name == "-activeforeground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk, value, c)) {
      rc = tk->throwError(tk->msg() + "unknown color name \"" + value + "\"");
      return true;
    }

    this->setActiveForeground(c);
  }
  else if (name == "-disabledforeground") {
    QColor c;
    if (! CTkAppUtil::variantToQColor(tk, value, c)) {
      rc = tk->throwError(tk->msg() + "unknown color name \"" + value + "\"");
      return true;
    }

    this->setDisabledForeground(c);
  }
  else
    return CTkAppCanvasShape::setShapeOpt(name, value, rc);

  return true;
}

//---

#ifdef CTK_APP_TKPATH
CTkAppCanvasCircleShape::
CTkAppCanvasCircleShape(CTkAppCanvasWidget *canvas, double xc, double yc) :
 CTkAppCanvasShape(canvas, ShapeType::CIRCLE), xc_(xc), yc_(yc)
{
  updatePath();
}

bool
CTkAppCanvasCircleShape::
getShapeOpt(const QString &name, QVariant &value) const
{
  if      (name == "-r") {
    value = this->radius();
  }
  else
    return CTkAppCanvasShape::getShapeOpt(name, value);

  return true;
}

// returns true if handled, rc = false if set name/value failed
bool
CTkAppCanvasCircleShape::
setShapeOpt(const QString &name, const QVariant &value, bool &rc)
{
  rc = true;

  auto *tk = canvas()->canvas()->app();

  if      (name == "-r") {
    double r;
    if (! tk->variantToReal(value, r)) {
      rc = tk->invalidReal(value);
      return true;
    }

    this->setRadius(r);
  }
  else
    return CTkAppCanvasShape::setShapeOpt(name, value, rc);

  return true;
}
#endif

//---

#ifdef CTK_APP_TKPATH
CTkAppCanvasEllipseShape::
CTkAppCanvasEllipseShape(CTkAppCanvasWidget *canvas, double xc, double yc) :
 CTkAppCanvasShape(canvas, ShapeType::ELLIPSE), xc_(xc), yc_(yc)
{
  updatePath();
}

bool
CTkAppCanvasEllipseShape::
getShapeOpt(const QString &name, QVariant &value) const
{
  if      (name == "-rx") {
    value = this->radiusX();
  }
  else if (name == "-ry") {
    value = this->radiusY();
  }
  else
    return CTkAppCanvasShape::getShapeOpt(name, value);

  return true;
}

// returns true if handled, rc = false if set name/value failed
bool
CTkAppCanvasEllipseShape::
setShapeOpt(const QString &name, const QVariant &value, bool &rc)
{
  rc = true;

  auto *tk = canvas()->canvas()->app();

  if      (name == "-rx") {
    double rx;
    if (! tk->variantToReal(value, rx)) {
      rc = tk->invalidReal(value);
      return true;
    }

    this->setRadiusX(rx);
  }
  else if (name == "-ry") {
    double ry;
    if (! tk->variantToReal(value, ry)) {
      rc = tk->invalidReal(value);
      return true;
    }

    this->setRadiusY(ry);
  }
  else
    return CTkAppCanvasShape::setShapeOpt(name, value, rc);

  return true;
}
#endif

//---

CTkAppCanvasImageShape::
CTkAppCanvasImageShape(CTkAppCanvasWidget *canvas, const Point &p, const CTkAppImageRef &image) :
 CTkAppCanvasShape(canvas, ShapeType::IMAGE), p_(p), image_(image)
{
  calcDrawRect();
}

void
CTkAppCanvasImageShape::
setPos(const Point &p)
{
  p_ = p;

  calcDrawRect();
}

void
CTkAppCanvasImageShape::
setImage(const CTkAppImageRef &i)
{
  image_ = i;

  calcDrawRect();
}

QRectF
CTkAppCanvasImageShape::
calcDrawRect() const
{
  if (! getImage())
    return QRectF();

  auto qimage = getImage()->getQImage();

  auto iw = qimage.width();
  auto ih = qimage.height();

  auto a = anchor();

  double dtx = 0.0, dty = 0.0;
  if      (a & Qt::AlignLeft   ) { dtx = 0.0; }
  else if (a & Qt::AlignHCenter) { dtx = -iw/2.0; }
  else if (a & Qt::AlignRight  ) { dtx = -iw; }
  if      (a & Qt::AlignTop    ) { dty = 0.0; }
  else if (a & Qt::AlignVCenter) { dty = -ih/2.0; }
  else if (a & Qt::AlignBottom ) { dty = -ih; }

  auto rect = QRectF(pos().x + dtx, pos().y + dty, iw, ih);

  auto *th = const_cast<CTkAppCanvasImageShape *>(this);

  th->setDrawRect(rect);

  return rect;
}

bool
CTkAppCanvasImageShape::
getShapeOpt(const QString &name, QVariant &value) const
{
  auto *tk = canvas()->canvas()->app();

  if      (name == "-image") {
    value = tk->imageToVariant(this->getImage());
  }
  else if (name == "-activeimage") {
    value = tk->imageToVariant(this->getActiveImage());
  }
  else if (name == "-disabledimage") {
    value = tk->imageToVariant(this->getDisabledImage());
  }
  else
    return CTkAppCanvasShape::getShapeOpt(name, value);

  return true;
}

// returns true if handled, rc = false if set name/value failed
bool
CTkAppCanvasImageShape::
setShapeOpt(const QString &name, const QVariant &value, bool &rc)
{
  rc = true;

  auto *tk = canvas()->canvas()->app();

  if      (name == "-image") {
    auto appImage = tk->getImage(value);
    if (! appImage) {
      rc = tk->throwError(tk->msg() + "image \"" + value + "\" doesn't exist");
      return true;
    }

    this->setImage(appImage);
  }
  else if (name == "-activeimage") {
    auto appImage = tk->getImage(value);
    if (! appImage) {
      rc = tk->throwError(tk->msg() + "image \"" + value + "\" doesn't exist");
      return true;
    }

    this->setActiveImage(appImage);
  }
  else if (name == "-disabledimage") {
    auto appImage = tk->getImage(value);
    if (! appImage) {
      rc = tk->throwError(tk->msg() + "image \"" + value + "\" doesn't exist");
      return true;
    }

    this->setDisabledImage(appImage);
  }
  else
    return CTkAppCanvasShape::setShapeOpt(name, value, rc);

  return true;
}

//---

CTkAppCanvasLineShape::
CTkAppCanvasLineShape(CTkAppCanvasWidget *canvas, const Points &points) :
 CTkAppCanvasShape(canvas, ShapeType::LINE), points_(points)
{
  updatePath();
}

bool
CTkAppCanvasLineShape::
getShapeOpt(const QString &name, QVariant &value) const
{
  auto *tk = canvas()->canvas()->app();

  if      (name == "-arrow") {
    auto arrowType = this->arrowType();

    switch (arrowType) {
      case CTkAppCanvasLineShape::ArrowType::NONE : value = "none"; break;
      case CTkAppCanvasLineShape::ArrowType::FIRST: value = "first"; break;
      case CTkAppCanvasLineShape::ArrowType::LAST : value = "last"; break;
      case CTkAppCanvasLineShape::ArrowType::BOTH : value = "both"; break;
      default: break;
    }
  }
  else if (name == "-arrowshape") {
    const auto &arrowShape = this->arrowShape();

    QVariantList vars;
    vars.push_back(tk->realToVariant(arrowShape.d1));
    vars.push_back(tk->realToVariant(arrowShape.d2));
    vars.push_back(tk->realToVariant(arrowShape.d3));

    value = vars;
  }
  else if (name == "-capstyle") {
    auto capStyle = this->capStyle();

    switch (capStyle) {
      case Qt::RoundCap : value = "round"; break;
      case Qt::SquareCap: value = "projecting"; break;
      case Qt::FlatCap  : value = "butt"; break;
      default: break;
    }
  }
  else if (name == "-joinstyle") {
    auto joinStyle = this->joinStyle();

    switch (joinStyle) {
      case Qt::BevelJoin: value = "bevel"; break;
      case Qt::MiterJoin: value = "miter"; break;
      case Qt::RoundJoin: value = "round"; break;
      default: break;
    }
  }
  else if (name == "-smooth") {
    value = smooth();
  }
  else if (name == "-splinesteps") {
    value = tk->intToVariant(this->splineSteps());
  }
  else
    return CTkAppCanvasShape::getShapeOpt(name, value);

  return true;
}

// returns true if handled, rc = false if set name/value failed
bool
CTkAppCanvasLineShape::
setShapeOpt(const QString &name, const QVariant &value, bool &rc)
{
  rc = true;

  auto *tk = canvas()->canvas()->app();

  if      (name == "-arrow") {
    auto str = tk->variantToString(value);

    auto arrowType = this->arrowType();

    if      (str == "none" ) arrowType = CTkAppCanvasLineShape::ArrowType::NONE;
    else if (str == "first") arrowType = CTkAppCanvasLineShape::ArrowType::FIRST;
    else if (str == "last" ) arrowType = CTkAppCanvasLineShape::ArrowType::LAST;
    else if (str == "both" ) arrowType = CTkAppCanvasLineShape::ArrowType::BOTH;
    else tk->TODO(name);

    this->setArrowType(arrowType);
  }
  else if (name == "-arrowshape") {
    std::vector<QString> items;
    if (! tk->splitList(value, items) || items.size() != 3) {
      rc = tk->throwError(tk->msg() + "bad arrow shape \"" + value + "\": "
                          "must be list with three numbers");
      return true;
    }

    CTkAppDistance d1, d2, d3;
    if (! tk->variantToDistance(items[0], d1) ||
        ! tk->variantToDistance(items[1], d2) ||
        ! tk->variantToDistance(items[2], d3)) {
      rc = tk->throwError(tk->msg() + "bad arrow shape \"" + value + "\": "
                          "must be list with three numbers");
      return true;
    }

    ArrowShape arrowShape;
    arrowShape.d1 = d1.rvalue;
    arrowShape.d2 = d2.rvalue;
    arrowShape.d3 = d3.rvalue;
    this->setArrowShape(arrowShape);
  }
  else if (name == "-capstyle") {
    static auto styleNames = std::vector<QString>({"butt", "projecting", "round"});

    QString value1;
    if (! tk->lookupName("-capstyle", styleNames, value, value1)) {
      rc = false; return true;
    }

    Qt::PenCapStyle capStyle = Qt::FlatCap;

    if      (value == "round"     ) capStyle = Qt::RoundCap;
    else if (value == "projecting") capStyle = Qt::SquareCap;
    else if (value == "butt"      ) capStyle = Qt::FlatCap;

    this->setCapStyle(capStyle);
  }
  else if (name == "-joinstyle") {
    static auto styleNames = std::vector<QString>({"bevel", "miter", "round"});

    QString value1;
    if (! tk->lookupName("-joinstyle", styleNames, value, value1)) {
      rc = false; return true;
    }

    Qt::PenJoinStyle joinStyle = Qt::RoundJoin;

    if      (value == "bevel") joinStyle = Qt::BevelJoin;
    else if (value == "miter") joinStyle = Qt::MiterJoin;
    else if (value == "round") joinStyle = Qt::RoundJoin;

    this->setJoinStyle(joinStyle);
  }
  else if (name == "-smooth") {
    auto str = tk->variantToString(value);

    this->setSmooth(str);
  }
  else if (name == "-splinesteps") {
    long s;
    if (! tk->variantToInt(value, s)) {
      rc = tk->throwError(tk->msg() + "Invalid integer \"" + value + "\"");
      return true;
    }

    this->setSplineSteps(s);
  }
  else
    return CTkAppCanvasShape::setShapeOpt(name, value, rc);

  return true;
}

bool
CTkAppCanvasLineShape::
insertChars(const QString &ind, const QString &str)
{
  std::cerr << "CTkAppCanvasLineShape::insertChars " << ind.toStdString() << " " <<
               str.toStdString() << "\n";

  auto *tk = canvas()->canvas()->app();

  IndexData indexData;
  if (! parseIndex(ind, indexData))
    return tk->throwError(tk->msg() + "bad index \"" + ind + "\"");

  if (indexData.type == IndexType::END) {
    CTkAppCanvasWidget::Points points;
    if (! getItemPoints(str, points))
      return false;

    for (const auto &p : points)
      points_.push_back(p);
  }
  else {
    tk->TODO("insert " + ind + " " + str);
    return false;
  }

  return true;
}

bool
CTkAppCanvasLineShape::
replaceChars(const QString &start, const QString &end, const QString &str)
{
  //std::cerr << "CTkAppCanvasLineShape::replaceChars " << start.toStdString() << " " <<
  //             end.toStdString() << " " << str.toStdString() << "\n";

  auto *tk = canvas()->canvas()->app();

  long istart, iend;
  if (! tk->variantToInt(start, istart))
    return tk->throwError(tk->msg() + "bad index \"" + start + "\"");
  if (end != "end") {
    if (! tk->variantToInt(end, iend))
      return tk->throwError(tk->msg() + "bad index \"" + end + "\"");
  }
  else
    iend = points_.size() - 1;

  if ((istart & 1) || (iend & 1))
    return false;

  istart /= 2;
  iend   /= 2;

  if (iend < istart)
    return false;

  std::vector<QString> strs;
  if (! tk->splitList(str, strs) || strs.size() & 1)
    return tk->throwError(tk->msg() + "invalid points");

  Points newPoints;

  for (uint i = 0; i < strs.size(); i += 2) {
    double x, y;
    if (! tk->variantToReal(strs[i    ], x))
      return tk->throwError(tk->msg() + "Invalid real \"" + strs[i    ] + "\"");
    if (! tk->variantToReal(strs[i + 1], y))
      return tk->throwError(tk->msg() + "Invalid real \"" + strs[i + 1] + "\"");

    newPoints.emplace_back(x, y);
  }

  Points points;

  std::swap(points, points_);

  uint i = 0;
  uint n = points.size();

  while (i < n && i < istart)
    points_.push_back(points[i++]);

  for (const auto &p : newPoints)
    points_.push_back(p);

  i = iend + 1;

  while (i < n)
    points_.push_back(points[i++]);

  return true;
}

//---

CTkAppCanvasOvalShape::
CTkAppCanvasOvalShape(CTkAppCanvasWidget *canvas, double x1, double y1, double x2, double y2) :
 CTkAppCanvasShape(canvas, ShapeType::OVAL), x1_(x1), y1_(y1), x2_(x2), y2_(y2)
{
  updatePaths();
}

bool
CTkAppCanvasOvalShape::
getShapeOpt(const QString &name, QVariant &value) const
{
  return CTkAppCanvasShape::getShapeOpt(name, value);
}

bool
CTkAppCanvasOvalShape::
setShapeOpt(const QString &name, const QVariant &value, bool &rc)
{
  return CTkAppCanvasShape::setShapeOpt(name, value, rc);
}

//---

#ifdef CTK_APP_TKPATH
CTkAppCanvasPathShape::
CTkAppCanvasPathShape(CTkAppCanvasWidget *canvas, const QString &pathStr,
                      const QPainterPath &qpath) :
 CTkAppCanvasShape(canvas, ShapeType::PATH), pathStr_(pathStr), qpath_(qpath)
{
}

bool
CTkAppCanvasPathShape::
getShapeOpt(const QString &name, QVariant &value) const
{
  auto *tk = canvas()->canvas()->app();

  if      (name == "-matrix") {
    tk->TODO(name);
  }
  else
    return CTkAppCanvasShape::getShapeOpt(name, value);

  return true;
}

// returns true if handled, rc = false if set name/value failed
bool
CTkAppCanvasPathShape::
setShapeOpt(const QString &name, const QVariant &value, bool &rc)
{
  rc = true;

  auto *tk = canvas()->canvas()->app();

  if      (name == "-matrix") {
    auto str = tk->variantToString(value);

    QTransform t;
    if (! CTkAppUtil::stringToMatrix(tk, str, t)) {
      rc = tk->throwError(tk->msg() + "Invalid matrix \"" + value + "\"");
      return true;
    }

    this->setTransform(t);
  }
  else
    return CTkAppCanvasShape::setShapeOpt(name, value, rc);

  return true;
}
#endif

//---

CTkAppCanvasPolygonShape::
CTkAppCanvasPolygonShape(CTkAppCanvasWidget *canvas, const Points &points) :
 CTkAppCanvasShape(canvas, ShapeType::POLYGON), points_(points)
{
  updatePath();
}

bool
CTkAppCanvasPolygonShape::
getShapeOpt(const QString &name, QVariant &value) const
{
  auto *tk = canvas()->canvas()->app();

  if      (name == "-joinstyle") {
    auto joinStyle = this->joinStyle();

    switch (joinStyle) {
      case Qt::BevelJoin: value = "bevel"; break;
      case Qt::MiterJoin: value = "miter"; break;
      case Qt::RoundJoin: value = "round"; break;
      default: break;
    }
  }
  else if (name == "-smooth") {
    value = tk->boolToVariant(isSmooth());
  }
  else if (name == "-splinesteps") {
    value = tk->intToVariant(this->splineSteps());
  }
  else
    return CTkAppCanvasShape::getShapeOpt(name, value);

  return true;
}

bool
CTkAppCanvasPolygonShape::
setShapeOpt(const QString &name, const QVariant &value, bool &rc)
{
  auto *tk = canvas()->canvas()->app();

  if      (name == "-joinstyle") {
    static auto styleNames = std::vector<QString>({"bevel", "miter", "round"});

    QString value1;
    if (! tk->lookupName("-joinstyle", styleNames, value, value1)) {
      rc = false; return true;
    }

    Qt::PenJoinStyle joinStyle = Qt::RoundJoin;

    if      (value == "bevel") joinStyle = Qt::BevelJoin;
    else if (value == "miter") joinStyle = Qt::MiterJoin;
    else if (value == "round") joinStyle = Qt::RoundJoin;

    this->setJoinStyle(joinStyle);
  }
  else if (name == "-smooth") {
    bool b;
    if (! tk->variantToBool(value, b))
      return false;

    this->setSmooth(b);
  }
  else if (name == "-splinesteps") {
    long s;
    if (! tk->variantToInt(value, s)) {
      rc = tk->throwError(tk->msg() + "Invalid integer \"" + value + "\"");
      return true;
    }

    this->setSplineSteps(s);
  }
  else
    return CTkAppCanvasShape::setShapeOpt(name, value, rc);

  return true;
}

//---

bool
CTkAppCanvasPolygonShape::
insertChars(const QString &ind, const QString &str)
{
  std::cerr << "CTkAppCanvasPolygonShape::insertChars " << ind.toStdString() << " " <<
               str.toStdString() << "\n";

  auto *tk = canvas()->canvas()->app();

  IndexData indexData;
  if (! parseIndex(ind, indexData))
    return tk->throwError(tk->msg() + "bad index \"" + ind + "\"");

  if (indexData.type == IndexType::END) {
    CTkAppCanvasWidget::Points points;
    if (! getItemPoints(str, points))
      return false;

    for (const auto &p : points)
      points_.push_back(p);
  }
  else {
    tk->TODO("insert " + ind + " " + str);
    return false;
  }

  return true;
}

bool
CTkAppCanvasPolygonShape::
deleteChars(const QString &start, const QString &end)
{
  //std::cerr << "CTkAppCanvasPolygonShape::deleteChars " << start.toStdString() << " " <<
  //             end.toStdString()  << "\n";

  return replaceChars(start, end, "");
}

bool
CTkAppCanvasPolygonShape::
replaceChars(const QString &start, const QString &end, const QString &str)
{
  //std::cerr << "CTkAppCanvasPolygonShape::replaceChars " << start.toStdString() << " " <<
  //             end.toStdString() << " " << str.toStdString() << "\n";

  auto *tk = canvas()->canvas()->app();

  long istart, iend;
  if (! tk->variantToInt(start, istart))
    return tk->throwError(tk->msg() + "bad index \"" + start + "\"");
  if (end != "end") {
    if (! tk->variantToInt(end, iend))
      return tk->throwError(tk->msg() + "bad index \"" + end + "\"");
  }
  else
    iend = points_.size() - 1;

  if ((istart & 1) || (iend & 1))
    return false;

  istart /= 2;
  iend   /= 2;

  if (iend < istart)
    return false;

  std::vector<QString> strs;
  if (! tk->splitList(str, strs) || strs.size() & 1)
    return tk->throwError(tk->msg() + "invalid points");

  Points newPoints;

  for (uint i = 0; i < strs.size(); i += 2) {
    double x, y;
    if (! tk->variantToReal(strs[i    ], x))
      return tk->throwError(tk->msg() + "Invalid real \"" + strs[i    ] + "\"");
    if (! tk->variantToReal(strs[i + 1], y))
      return tk->throwError(tk->msg() + "Invalid real \"" + strs[i + 1] + "\"");

    newPoints.emplace_back(x, y);
  }

  Points points;

  std::swap(points, points_);

  uint i = 0;
  uint n = points.size();

  while (i < n && i < istart)
    points_.push_back(points[i++]);

  for (const auto &p : newPoints)
    points_.push_back(p);

  i = iend + 1;

  while (i < n)
    points_.push_back(points[i++]);

  return true;
}

//---

CTkAppCanvasRectangleShape::
CTkAppCanvasRectangleShape(CTkAppCanvasWidget *canvas, double x1, double y1, double x2, double y2) :
 CTkAppCanvasShape(canvas, ShapeType::RECTANGLE), p1_(x1, y1), p2_(x2, y2)
{
  updatePaths();
}

bool
CTkAppCanvasRectangleShape::
getShapeOpt(const QString &name, QVariant &value) const
{
  auto *tk = canvas()->canvas()->app();

  if      (name == "-joinstyle") {
    tk->TODO(name);
  }
  else if (name == "-smooth") {
    value = smooth();
  }
  else if (name == "-splinesteps") {
    value = tk->intToVariant(this->splineSteps());
  }
  else if (name == "-rx") {
    tk->TODO(name);
  }
  else
    return CTkAppCanvasShape::getShapeOpt(name, value);

  return true;
}

// returns true if handled, rc = false if set name/value failed
bool
CTkAppCanvasRectangleShape::
setShapeOpt(const QString &name, const QVariant &value, bool &rc)
{
  rc = true;

  auto *tk = canvas()->canvas()->app();

  if      (name == "-joinstyle") {
    tk->TODO(name);
  }
  else if (name == "-smooth") {
    auto str = tk->variantToString(value);

    this->setSmooth(str);
  }
  else if (name == "-splinesteps") {
    long s;
    if (! tk->variantToInt(value, s)) {
      rc = tk->throwError(tk->msg() + "Invalid integer \"" + value + "\"");
      return true;
    }

    this->setSplineSteps(s);
  }
  else if (name == "-rx") {
    tk->TODO(name);
  }
  else
    return CTkAppCanvasShape::setShapeOpt(name, value, rc);

  return true;
}

//---

CTkAppCanvasTextShape::
CTkAppCanvasTextShape(CTkAppCanvasWidget *canvas, const Point &p, const QString &text) :
 CTkAppCanvasShape(canvas, ShapeType::TEXT), p_(p), text_(text)
{
  calcRect();
}

void
CTkAppCanvasTextShape::
setPos(const Point &p)
{
  p_ = p;

  calcRect();
}

void
CTkAppCanvasTextShape::
setText(const QString &text)
{
  text_ = text;

  calcRect();

  (void) updateSelection();
}

void
CTkAppCanvasTextShape::
setAngle(double r)
{
  angle_ = r;

  calcRect();
}

void
CTkAppCanvasTextShape::
setFont(const QFont &f)
{
  font_ = f;

  calcRect();
}

void
CTkAppCanvasTextShape::
setJustify(const Qt::Alignment &a)
{
  justify_ = a;

  calcRect();
}

void
CTkAppCanvasTextShape::
setTextAnchor(const Qt::Alignment &textAnchor)
{
  textAnchor_ = textAnchor;

  calcRect();
}

void
CTkAppCanvasTextShape::
setUnderLine(int i)
{
  underLine_ = i;
}

void
CTkAppCanvasTextShape::
setWidth(double r)
{
  width_ = r;

  calcRect();
}

QRectF
CTkAppCanvasTextShape::
calcRect() const
{
  auto p = calcPos();

  auto text = getText();

  QFontMetricsF fm(font());

  auto p1 = p + QPointF(                       0.0, -fm.descent());
  auto p2 = p + QPointF(fm.horizontalAdvance(text),  fm.ascent ());

  QRectF rect(p1, p2);

  auto *th = const_cast<CTkAppCanvasTextShape *>(this);

  th->setDrawRect(rect);

  return rect;
}

QPointF
CTkAppCanvasTextShape::
calcPos() const
{
  auto text = getText();

  QFontMetricsF fm(font());
  auto tw = fm.horizontalAdvance(text);

  auto tx = pos().x;
  auto ty = pos().y;

  auto j = justify();

  double dtx = 0.0, dty = 0.0;
  if      (j & Qt::AlignLeft   ) { dtx = 0.0; }
  else if (j & Qt::AlignHCenter) { dtx = -tw/2.0; }
  else if (j & Qt::AlignRight  ) { dtx = -tw; }
  if      (j & Qt::AlignTop    ) { dty = fm.ascent(); }
  else if (j & Qt::AlignVCenter) { dty = (fm.ascent() - fm.descent())/2.0; }
  else if (j & Qt::AlignBottom ) { dty = -fm.descent(); }

  return QPointF(tx + dtx, ty + dty);
}

//---

bool
CTkAppCanvasTextShape::
insertChars(const QString &ind, const QString &str)
{
//std::cerr << "CTkAppCanvasTextShape::insertChars " << ind.toStdString() << " " <<
//             str.toStdString() << "\n";

  auto *tk = canvas()->canvas()->app();

  IndexData indexData;
  if (! parseIndex(ind, indexData))
    return tk->throwError(tk->msg() + "bad index \"" + ind + "\"");

  long iind;
  if (! mapIndex(indexData, iind))
    return false;

  auto lhs = text_.mid(0, iind + 1);
  auto rhs = text_.mid(iind + 1);

  setText(lhs + str + rhs);

  return true;
}

bool
CTkAppCanvasTextShape::
deleteChars(const QString &start, const QString &end)
{
//std::cerr << "CTkAppCanvasTextShape::deleteChars " << start.toStdString() << " " <<
//             end.toStdString()  << "\n";

  return replaceChars(start, end, "");
}

bool
CTkAppCanvasTextShape::
replaceChars(const QString &start, const QString &end, const QString &str)
{
//std::cerr << "CTkAppCanvasTextShape::replaceChars " << start.toStdString() << " " <<
//             end.toStdString() << " " << str.toStdString() << "\n";

  auto *tk = canvas()->canvas()->app();

  IndexData startIndexData;
  if (! parseIndex(start, startIndexData))
    return tk->throwError(tk->msg() + "bad index \"" + start + "\"");

  IndexData endIndexData;
  if (! parseIndex(end, endIndexData))
    return tk->throwError(tk->msg() + "bad index \"" + end + "\"");

  long istart, iend;
  if (! mapIndex(startIndexData, istart) || ! mapIndex(endIndexData, iend))
    return false;

  if (iend < istart)
    return false;

  auto lhs = text_.mid(0, istart);
  auto rhs = text_.mid(iend + 1);

  setText(lhs + str + rhs);

  return true;
}

//---


bool
CTkAppCanvasTextShape::
updateSelection()
{
  if (selectFrom_.type == IndexType::NONE || selectTo_.type == IndexType::NONE)
    return true;

  auto *tk = canvas()->canvas()->app();

  long start, end;
  if (! mapIndex(selectFrom_, start) || ! mapIndex(selectTo_, end))
    return false;

  if (start > end) {
    std::swap(start, end);

    --end;
  }

  start = std::max(start, 0L);
  end   = std::min(end  , long(text_.length() - 1));

  tk->setClipboardText(text_.mid(start, end - start + 1), CTkAppClipboard::Selection);

  return true;
}

bool
CTkAppCanvasTextShape::
mapIndex(const IndexData &indexData, long &pos) const
{
  auto *tk = canvas()->canvas()->app();

  if      (indexData.type == IndexType::POS) {
    pos = indexData.ind;

    return true;
  }
  else if (indexData.type == IndexType::END) {
    pos = text_.size() - 1;

    return true;
  }
  else if (indexData.type == IndexType::INSERT) {
    return mapIndex(insertCursor_, pos);
  }
  else if (indexData.type == IndexType::SEL_FIRST) {
    return mapIndex(selectFrom_, pos);
  }
  else if (indexData.type == IndexType::SEL_LAST) {
    return mapIndex(selectTo_, pos);
  }
  else if (indexData.type == IndexType::COORD) {
    // TODO: char at x, y pos
    pos = 0;
    return true;
  }

  return tk->throwError(tk->msg() + "bad index");
}

//---

bool
CTkAppCanvasTextShape::
getShapeOpt(const QString &name, QVariant &value) const
{
  auto *tk = canvas()->canvas()->app();

  if      (name == "-angle") {
    value = tk->realToVariant(this->angle());
  }
  else if (name == "-font") {
    value = tk->fontToVariant(this->font());
  }
  else if (name == "-justify") {
    auto align = this->justify();

    switch (align) {
      case Qt::AlignLeft   : value = "left"  ; break;
      case Qt::AlignRight  : value = "right" ; break;
      case Qt::AlignHCenter: value = "center"; break;
      default: break;
    }
  }
  else if (name == "-text") {
    value = text_;
  }
#ifdef CTK_APP_TKPATH
  else if (name == "-textanchor") {
    auto align = this->textAnchor();

    if      (align ==  Qt::AlignTop                     ) value = "n";
    else if (align == (Qt::AlignTop    | Qt::AlignRight)) value = "ne";
    else if (align ==                    Qt::AlignRight ) value = "e";
    else if (align == (Qt::AlignBottom | Qt::AlignRight)) value = "se";
    else if (align ==  Qt::AlignBottom                  ) value = "s";
    else if (align == (Qt::AlignBottom | Qt::AlignLeft )) value = "sw";
    else if (align ==                    Qt::AlignLeft  ) value = "w";
    else if (align == (Qt::AlignTop    | Qt::AlignLeft )) value = "nw";
    else if (align ==  Qt::AlignCenter                  ) value = "center";
  }
#endif
  else if (name == "-underline") {
    value = tk->intToVariant(this->underLine());
  }
  else if (name == "-width") {
    value = tk->realToVariant(this->width());
  }
  else
    return CTkAppCanvasShape::getShapeOpt(name, value);

  return true;
}

// returns true if handled, rc = false if set name/value failed
bool
CTkAppCanvasTextShape::
setShapeOpt(const QString &name, const QVariant &value, bool &rc)
{
  rc = true;

  auto *tk = canvas()->canvas()->app();

  //---

  static auto names = std::vector<QString>({
    "-angle", "-font", "-justify", "-text",
#ifdef CTK_APP_TKPATH
    "-textanchor",
#endif
    "-underline", "-width" });

  QString name1;
  int     nmatch;
  if (! CTkAppUtil::uniqueMatch(names, name, name1, nmatch))
    return CTkAppCanvasShape::setShapeOpt(name, value, rc);

  //---

  if      (name1 == "-angle") {
    double a;
    if (! tk->variantToReal(value, a)) {
      rc = tk->invalidReal(value);
      return true;
    }

    while (a <    0.0) a += 360.0;
    while (a >= 360.0) a -= 360.0;

    this->setAngle(a);
  }
  else if (name1 == "-font") {
    auto str = tk->variantToString(value);

    if (str == "") {
      rc = tk->throwError(tk->msg() + "font \"" + value + "\" doesn't exist");
      return true;
    }

    auto f = tk->getQFont(str);

    this->setFont(f);
  }
  else if (name1 == "-justify") {
    Qt::Alignment align;
    if (! CTkAppUtil::variantToJustify(value, align)) {
      rc = tk->throwError(tk->msg() + "bad justification \"" + value + "\": must be "
                          "left, right, or center");
      return true;
    }

    this->setJustify(align);
  }
  else if (name1 == "-text") {
    auto str = tk->variantToString(value);

    this->setText(str);
  }
#ifdef CTK_APP_TKPATH
  else if (name1 == "-textanchor") {
    Qt::Alignment align;
    if (! tk->variantToAlign(value, align)) {
      rc = tk->throwError(tk->msg() + "bad anchor position \"" + value + "\": must be "
                          "n, ne, e, se, s, sw, w, nw, or center");
      return true;
    }

    this->setTextAnchor(align);
  }
#endif
  else if (name1 == "-underline") {
    long pos = -1;
    if (! tk->getOptionInt(name1, value, pos)) {
      rc = tk->invalidInteger(value);
      return true;
    }

    this->setUnderLine(pos);
  }
  else if (name1 == "-width") {
    CTkAppDistance d;
    if (! tk->variantToDistance(value, d)) {
      rc = tk->invalidDistance(value);
      return true;
    }

    this->setWidth(d.rvalue);
  }

  return true;
}

//---

CTkAppCanvasWindowShape::
CTkAppCanvasWindowShape(CTkAppCanvasWidget *canvas, const Point &p) :
 CTkAppCanvasShape(canvas, ShapeType::WINDOW), p_(p)
{
}

void
CTkAppCanvasWindowShape::
updateRange()
{
  updateWidget();
}

void
CTkAppCanvasWindowShape::
moveBy(double dx, double dy)
{
  p_.moveBy(dx, dy);

  updateWidget();
}

void
CTkAppCanvasWindowShape::
updateWidget()
{
  if (! widget_)
    return;

  auto p = canvas_->windowToPixel(p_);

  widget_->move(p.x, p.y);
}

bool
CTkAppCanvasWindowShape::
getShapeOpt(const QString &name, QVariant &value) const
{
  auto *tk = canvas()->canvas()->app();

  if      (name == "-height") {
    value = tk->realToVariant(height());
  }
  else if (name == "-width") {
    value = tk->realToVariant(width());
  }
  else if (name == "-window") {
    value = window();
  }
  else
    return CTkAppCanvasShape::getShapeOpt(name, value);

  return true;
}

// returns true if handled, rc = false if set name/value failed
bool
CTkAppCanvasWindowShape::
setShapeOpt(const QString &name, const QVariant &value, bool &rc)
{
  rc = true;

  auto *tk = canvas()->canvas()->app();

  if      (name == "-height") {
    CTkAppDistance d;
    if (! tk->variantToDistance(value, d)) {
      rc = tk->invalidDistance(value);
      return true;
    }

    this->setHeight(d.rvalue);
  }
  else if (name == "-width") {
    CTkAppDistance d;
    if (! tk->variantToDistance(value, d)) {
      rc = tk->invalidDistance(value);
      return true;
    }

    this->setWidth(d.rvalue);
  }
  else if (name == "-window") {
    CTkAppWidget *window = nullptr;
    QString       windowName;

    if (tk->variantIsValid(value)) {
      windowName = tk->variantToString(value);

      window = tk->lookupWidgetByName(windowName, /*quiet*/true);
      if (! window) {
        rc = tk->throwError(tk->msg() + "bad window path name \"" + value + "\"");
        return true;
      }
    }

    if (widget_)
      widget_->setParent(nullptr);

    if (window)
      widget_ = window->getQWidget();
    else
      widget_ = WidgetP();

    if (widget_) {
      widget_->setParent(canvas_);
      widget_->show();
    }

    this->setWindow(windowName);

    updateWidget();
  }
  else
    return CTkAppCanvasShape::setShapeOpt(name, value, rc);

  return true;
}
