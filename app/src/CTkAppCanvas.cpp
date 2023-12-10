#include <CTkAppCanvas.h>
#include <CTkAppCanvasWidget.h>
#include <CTkAppCanvasShape.h>

#include <QMouseEvent>

CTkAppCanvas::
CTkAppCanvas(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qcanvas_ = new CTkAppCanvasWidget(this);

  qcanvas_->setFocusPolicy(Qt::StrongFocus);

  setQWidget(qcanvas_);
}

bool
CTkAppCanvas::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-closeenough") {
    double r;
    if (! tk_->variantToReal(var, r))
      return tk_->invalidReal(var);

    qcanvas_->setCloseEnough(r);
  }
  else if (name == "-confine") {
    bool b;
    if (! tk_->variantToBool(var, b))
      return tk_->invalidBool(var);

    qcanvas_->setConfine(b);
  }
  else if (name == "-height") {
    CTkAppDistance height;
    if (! tk_->variantToDistance(var, height) || height.rvalue < 0)
      return tk_->invalidDistance(var);

    setHeight(height.rvalue);
  }
  else if (name == "-scrollregion") {
    auto value = tk_->variantToString(var);

    std::vector<QString> strs;
    if (! tk_->splitList(value, strs) || strs.size() != 4)
      return tk_->throwError(tk_->msg() + "invalid region \"" + var + "\"");

    CTkAppDistance x1, y1, x2, y2;
    if (! tk_->variantToDistance(strs[0], x1))
      return tk_->invalidDistance(strs[0]);
    if (! tk_->variantToDistance(strs[1], y1))
      return tk_->invalidDistance(strs[1]);
    if (! tk_->variantToDistance(strs[2], x2))
      return tk_->invalidDistance(strs[2]);
    if (! tk_->variantToDistance(strs[3], y2))
      return tk_->invalidDistance(strs[3]);

    qcanvas_->setScrollRegion(QRectF(x1.rvalue, y1.rvalue, x2.rvalue - x1.rvalue,
                                     y2.rvalue - y1.rvalue));
  }
  else if (name == "-state") {
    auto value = tk_->variantToString(var);

    if (! setWidgetStateFromString(value))
      return tk_->throwError(tk_->msg() + "bad state \"" + value + "\": must be "
                             "active, disabled, or normal");
  }
  else if (name == "-width") {
    CTkAppDistance width;
    if (! tk_->variantToDistance(var, width) || width.rvalue < 0)
      return tk_->invalidDistance(var);

    setWidth(width.rvalue);
  }
  else if (name == "-xscrollincrement") {
    CTkAppDistance xs;
    if (! tk_->variantToDistance(var, xs))
      return tk_->invalidDistance(var);

    qcanvas_->setXScrollIncrement(xs.rvalue);
  }
  else if (name == "-yscrollincrement") {
    CTkAppDistance ys;
    if (! tk_->variantToDistance(var, ys))
      return tk_->invalidDistance(var);

    qcanvas_->setYScrollIncrement(ys.rvalue);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppCanvas::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  //---

  static auto optionNames = std::vector<QString>({
    "addtag", "bbox", "bind", "canvasx", "canvasy", "cget", "configure", "coords",
    "create", "dchars", "delete", "dtag", "find", "focus", "gettags",
#ifdef CTK_APP_TKPATH
    "gradient",
#endif
    "icursor", "image", "imove", "index", "insert", "itemcget", "itemconfigure",
    "lower", "move", "moveto", "postscript", "raise", "rchars", "rotate", "scale",
    "scan", "select",
#ifdef CTK_APP_TKPATH
    "style",
#endif
    "type", "xview", "yview"
  });

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  auto variantToDistance = [&](const QVariant &var, CTkAppDistance &r) {
    if (! tk_->variantToDistance(var, r))
      return tk_->invalidDistance(var);
    return true;
  };

  if      (option == "addtag") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getFullName() + " addtag tag searchCommand ?arg ...?");

    auto tag = tk_->variantToString(args[1]);
    auto cmd = tk_->variantToString(args[2]);

    if      (cmd == "above") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " addtag tag above tagOrId");

      auto aboveTag = tk_->variantToString(args[3]);

      CTkAppCanvasWidget::Shapes shapes;
      if (! qcanvas_->getShapes(aboveTag, shapes))
        return false;

      if (! shapes.empty()) {
        auto *shape = shapes.back();

        auto *aboveShape = qcanvas_->getAboveShape(shape);

        if (aboveShape)
          aboveShape->addTag(tag);
      }
    }
    else if (cmd == "all") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " addtag tag all");

      const auto &shapes = qcanvas_->getShapes();

      for (auto *s : shapes)
        s->addTag(tag);
    }
    else if (cmd == "below") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " addtag tag below tagOrId");

      auto belowTag = tk_->variantToString(args[3]);

      CTkAppCanvasWidget::Shapes shapes;
      if (! qcanvas_->getShapes(belowTag, shapes))
        return false;

      if (! shapes.empty()) {
        auto *shape = shapes.back();

        auto *belowShape = qcanvas_->getBelowShape(shape);

        if (belowShape)
          belowShape->addTag(tag);
      }
    }
    else if (cmd == "closest") {
      if (numArgs < 5)
        return tk_->wrongNumArgs(getFullName() + " addtag tag closest x y ?halo? ?start?");

      double x, y;
      if (! tk_->variantToReal(args[3], x))
        return tk_->invalidReal(args[3]);
      if (! tk_->variantToReal(args[4], y))
        return tk_->invalidReal(args[4]);

      long halo = -1;
      if (numArgs > 4) {
        if (! tk_->variantToInt(args[5], halo))
          return tk_->invalidInteger(args[5]);
      }

      long start;
      if (numArgs > 5) {
        if (! tk_->variantToInt(args[6], start))
          return tk_->invalidInteger(args[6]);
      }

      auto p = CTkAppPoint(x, y);

      CTkAppCanvasWidget::Shapes shapes;
      qcanvas_->findNearestShapes(p, shapes);

      auto *shape = (! shapes.empty() ? shapes[0] : nullptr);

      if (shape)
        shape->addTag(tag);
    }
    else if (cmd == "enclosed") {
      if (numArgs != 7)
        return tk_->wrongNumArgs(getFullName() + " addtag tag enclosed x1 y1 x2 y2");

      double x1, y1, x2, y2;
      if (! tk_->variantToReal(args[3], x1))
        return tk_->invalidReal(args[3]);
      if (! tk_->variantToReal(args[4], y1))
        return tk_->invalidReal(args[4]);
      if (! tk_->variantToReal(args[5], x2))
        return tk_->invalidReal(args[5]);
      if (! tk_->variantToReal(args[6], y2))
        return tk_->invalidReal(args[6]);

      auto r = QRectF(x1, y1, x2 - x1, y2 - y1);

      CTkAppCanvasWidget::Shapes shapes;
      qcanvas_->findEnclosedShapes(r, shapes);

      for (auto *shape : shapes)
        shape->addTag(tag);
    }
    else if (cmd == "overlapping") {
      if (numArgs != 7)
        return tk_->wrongNumArgs(getFullName() + " addtag tag overlapping x1 y1 x2 y2");

      double x1, y1, x2, y2;
      if (! tk_->variantToReal(args[3], x1))
        return tk_->invalidReal(args[3]);
      if (! tk_->variantToReal(args[4], y1))
        return tk_->invalidReal(args[4]);
      if (! tk_->variantToReal(args[5], x2))
        return tk_->invalidReal(args[5]);
      if (! tk_->variantToReal(args[6], y2))
        return tk_->invalidReal(args[5]);

      auto r = QRectF(x1, y1, x2 - x1, y2 - y1);

      CTkAppCanvasWidget::Shapes shapes;
      qcanvas_->findOverlappingShapes(r, shapes);

      for (auto *shape : shapes)
        shape->addTag(tag);
    }
    else if (cmd == "withtag") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " addtag tag withtag tagOrId");

      auto withTag = tk_->variantToString(args[3]);

      CTkAppCanvasWidget::Shapes shapes;
      if (! qcanvas_->getShapes(withTag, shapes))
        return false;

      for (auto *shape : shapes)
        shape->addTag(tag);
    }
    else
      return tk_->throwError("bad addtag command \"" + cmd + "\": must be "
               "above, all, below, closest, enclosed, overlapping, or withtag");
  }
  else if (option == "bbox") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " bbox tagOrId ?tagOrId ...?");

    QRectF bbox;

    uint i = 1;

    for ( ; i < numArgs; ++i) {
      auto id = tk_->variantToString(args[1]);

      auto bbox1 = qcanvas_->getShapeBBox(id);
      if (bbox1.isNull()) continue;

      if (! bbox.isNull())
        bbox = bbox.united(bbox1);
      else
        bbox = bbox1;
    }

    if (! bbox.isNull())
      tk_->setIntegerArrayResult({{ int(bbox.left()), int(bbox.top()),
                                    int(bbox.right()), int(bbox.bottom()) }});
  }
  else if (option == "bind") {
    if (numArgs < 2 || numArgs > 4)
      return tk_->wrongNumArgs(getFullName() + " bind tagOrId ?sequence? ?command?");

    auto id = tk_->variantToString(args[1]);

    QString sequence, command;

    CTkAppEventData data;

    if (numArgs > 2) {
      sequence = tk_->variantToString(args[2]);

      if (sequence != "" && ! tk_->parseEvent(sequence, data))
        return tk_->throwError(tk_->msg() + "Invalid event \"" + sequence + "\"");
    }

    if (numArgs > 3)
      command = tk_->variantToString(args[3]);

    // list bound events
    if      (numArgs == 2) {
      auto pe = idEventDatas_.find(id);
      if (pe == idEventDatas_.end()) return true;

      std::vector<QString> strs;

      for (const auto &eventData : (*pe).second) {
        strs.push_back(eventData.pattern);
      }

      tk_->setStringArrayResult(strs);
    }
    // list event commands
    else if (numArgs == 3) {
      auto pe = idEventDatas_.find(id);
      if (pe == idEventDatas_.end()) return true;

      std::vector<QString> strs;

      for (const auto &eventData : (*pe).second) {
        if (command == sequence)
          continue;

        strs.push_back(eventData.command);
      }

      tk_->setStringArrayResult(strs);
    }
    else {
      idEventDatas_[id].push_back(data);
    }
  }
  else if (option == "canvasx") {
    if (numArgs < 2 || numArgs > 3)
      return tk_->wrongNumArgs(getFullName() + " canvasx screenx ?gridspacing?");

    double x;
    if (! tk_->variantToReal(args[1], x))
      return tk_->invalidReal(args[1]);

    auto p = qcanvas_->pixelToWindow(CTkAppPoint(x, 0.0));

    tk_->setRealResult(p.x);
  }
  else if (option == "canvasy") {
    if (numArgs < 2 || numArgs > 3)
      return tk_->wrongNumArgs(getFullName() + " canvasy screenx ?gridspacing?");

    double y;
    if (! tk_->variantToReal(args[1], y))
      return tk_->invalidReal(args[1]);

    auto p = qcanvas_->pixelToWindow(CTkAppPoint(0.0, y));

    tk_->setRealResult(p.y);
  }
  else if (option == "coords") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " coords tagOrId ?x y x y ...?");

    auto id = tk_->variantToString(args[1]);

    CTkAppCanvasWidget::Points points;

    if      (numArgs == 3) {
      std::vector<QString> strs;
      if (! tk_->splitList(args[2], strs))
        return false;

      if (strs.size() & 1)
        return tk_->throwError(tk_->msg() + "wrong # coordinates: expected an even number, "
                               "got " + QString::number(strs.size()));

      for (uint i = 0; i < strs.size(); i += 2) {
        CTkAppDistance x, y;
        if (! variantToDistance(strs[i], x) || ! variantToDistance(strs[i + 1], y))
          return false;

        points.push_back(CTkAppCanvasWidget::Point(x.rvalue, y.rvalue));
      }

      if (points.empty())
        return false;

      if (! qcanvas_->setShapePoints(id, points))
        return false;
    }
    else if (numArgs > 2) {
      for (uint i = 2; i < numArgs; i += 2) {
        auto xarg = tk_->variantToString(args[i]);

        if (xarg[0] == '-' && ! isdigit(xarg[1].toLatin1()))
          break;

        if (i + 1 >= numArgs)
          return tk_->throwError(tk_->msg() + "odd # coordinates");;

        auto yarg = tk_->variantToString(args[i + 1]);

        CTkAppDistance x, y;
        if (! variantToDistance(xarg, x) || ! variantToDistance(yarg, y))
          return false;

        points.push_back(CTkAppCanvasWidget::Point(x.rvalue, y.rvalue));
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

      std::vector<double> reals;

      for (const auto &p : points) {
        reals.push_back(p.x);
        reals.push_back(p.y);
      }

      tk_->setRealArrayResult(reals);
    }
  }
  else if (option == "create") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " create type coords ?arg ...?");

    //---

    // get type
    static auto typeNames = std::vector<QString>({
      "arc", "bitmap", "image", "line", "oval",
      "polygon", "rectangle", "text", "window"
#ifdef CTK_APP_TKPATH
    , "circle", "ellipse", "group", "path", "pimage", "pline",
      "polyline", "ppolygon", "prect", "ptext"
#endif
    });

    auto type = tk_->variantToString(args[1]);

    if (type == "poly")
      type = "polygon";

    if (! tk_->lookupName("type", typeNames, type, type))
      return false;

    //---

    // get points
    auto getPointStrs = [&](uint &i, std::vector<QString> &pointStrs, uint maxPoints=1000) {
      for ( ; i < numArgs; ++i) {
        auto argi = tk_->variantToString(args[i]);
        if (argi[0] == '-' && ! argi[1].isDigit()) break;

        pointStrs.push_back(argi);

        if (pointStrs.size() >= maxPoints) {
          ++i;
          break;
        }
      }

      if (pointStrs.size() == 1) {
        std::vector<QString> pointStrs1;
        if (! tk_->splitList(pointStrs[0], pointStrs1))
          return false;

        pointStrs = pointStrs1;
      }

      return true;
    };

    auto strsToPoints = [&](const std::vector<QString> &pointStrs,
                            CTkAppCanvasWidget::Points &points) {
      uint pi = 0;

      while (pi < pointStrs.size()) {
        CTkAppDistance x, y;
        if (! variantToDistance(pointStrs[pi], x) || ! variantToDistance(pointStrs[pi + 1], y))
          return false;

        points.push_back(CTkAppCanvasWidget::Point(x.rvalue, y.rvalue));

        pi += 2;
      }

      return true;
    };

    //---

    auto processShapeOpts = [&](CTkAppCanvasShape *shape, uint &i) {
      for ( ; i < numArgs; i += 2) {
        auto name = tk_->variantToString(args[i]);

        if (name[0] == '-') {
          if (i + 1 >= numArgs)
            return false;

          auto var = args[i + 1];

          bool rc;
          if (shape->setShapeOpt(name, var, rc)) {
            if (! rc) return false;
            continue;
          }
          else
            return tk_->throwError(tk_->msg() + "unknown option \"" + name + "\"");
        }
        else {
          return tk_->throwError(tk_->msg() + "unknown option \"" + name + "\"");
        }
      }

      return true;
    };

    //---

    if      (type == "arc") {
      uint i = 2;

      std::vector<QString> pointStrs;
      if (! getPointStrs(i, pointStrs) || pointStrs.empty())
        return tk_->wrongNumArgs(getFullName() + " create arc coords ?arg ...?");

      if (pointStrs.size() & 1)
        return tk_->throwError(tk_->msg() + "wrong # coordinates: expected an even number, "
                               "got " + QString::number(pointStrs.size()));

      CTkAppCanvasWidget::Points points;
      if (! strsToPoints(pointStrs, points))
        return false;

      if (points.size() != 2)
        return tk_->wrongNumArgs(getFullName() + " create arc coords ?arg ...?");

      //---

      auto *arc = qcanvas_->addArc(points);

      if (! processShapeOpts(arc, i))
        return false;

      tk_->setIntegerResult(arc->id());
    }
    else if (type == "bitmap") {
      uint i = 2;

      std::vector<QString> pointStrs;
      if (! getPointStrs(i, pointStrs, 2) || pointStrs.empty())
        return tk_->wrongNumArgs(getFullName() + " create bitmap coords ?arg ...?");

      if (pointStrs.size() & 1)
        return tk_->throwError(tk_->msg() + "wrong # coordinates: expected 2, got 1");

      CTkAppCanvasWidget::Points points;
      if (! strsToPoints(pointStrs, points))
        return false;

      if (points.size() != 1)
        return tk_->wrongNumArgs(getFullName() + " create bitmap coords ?arg ...?");

      auto *bitmap = qcanvas_->addBitmap(points[0]);

      if (! processShapeOpts(bitmap, i))
        return false;

      tk_->setIntegerResult(bitmap->id());
    }
    else if (type == "image" || type == "pimage") {
#ifndef CTK_APP_TKPATH
      if (type == "pimage")
        return tk_->throwError("pimage supported without tkpath support");
#endif

      uint i = 2;

      std::vector<QString> pointStrs;
      if (! getPointStrs(i, pointStrs, 2) || pointStrs.empty())
        return tk_->wrongNumArgs(getFullName() + " create image coords ?arg ...?");

      if (pointStrs.size() & 1)
        return tk_->throwError(tk_->msg() + "wrong # coordinates: expected 2, got 1");

      CTkAppCanvasWidget::Points points;
      if (! strsToPoints(pointStrs, points))
        return false;

      if (points.size() != 1)
        return tk_->wrongNumArgs(getFullName() + " create image coords ?arg ...?");

      //---

      auto *image = qcanvas_->addImage(points[0]);

      if (! processShapeOpts(image, i))
        return false;

      tk_->setIntegerResult(image->id());
    }
    else if (type == "line" || type == "pline") {
#ifndef CTK_APP_TKPATH
      if (type == "pline")
        return tk_->throwError("pline supported without tkpath support");
#endif

      uint i = 2;

      std::vector<QString> pointStrs;
      if (! getPointStrs(i, pointStrs) || pointStrs.empty())
        return tk_->wrongNumArgs(getFullName() + " create line coords ?arg ...?");

      if (pointStrs.size() & 1)
        return tk_->throwError(tk_->msg() + "wrong # coordinates: expected an even number, "
                               "got " + QString::number(pointStrs.size()));

      CTkAppCanvasWidget::Points points;
      if (! strsToPoints(pointStrs, points))
        return false;

      if (points.size() < 2)
        return tk_->wrongNumArgs(getFullName() + " create line coords ?arg ...?");

      //---

      auto *line = qcanvas_->addLine(points);

      if (! processShapeOpts(line, i))
        return false;

      tk_->setIntegerResult(line->id());
    }
    else if (type == "oval") {
      if (numArgs < 3)
        return tk_->wrongNumArgs(getFullName() + " create oval coords ?arg ...?");

      uint i = 2;

      std::vector<QString> pointStrs;
      if (! getPointStrs(i, pointStrs, 4) || pointStrs.empty())
        return tk_->wrongNumArgs(getFullName() + " create oval coords ?arg ...?");

      if (pointStrs.size() != 4)
        return tk_->throwError(tk_->msg() + "wrong # coordinates: expected 4 got " +
                               QString::number(pointStrs.size()));

      CTkAppCanvasWidget::Points points;
      if (! strsToPoints(pointStrs, points))
        return false;

      if (points.size() != 2)
        return tk_->wrongNumArgs(getFullName() + " create oval coords ?arg ...?");

      auto *oval = qcanvas_->addOval(points[0], points[1]);

      if (! processShapeOpts(oval, i))
        return false;

      tk_->setIntegerResult(oval->id());
    }
#ifdef CTK_APP_TKPATH
    else if (type == "circle") {
      if (numArgs < 4)
        return tk_->wrongNumArgs(getFullName() + " create circle xc yc ?arg ...?");

      double xc, yc;
      if (! tk_->variantToReal(args[2], xc) || ! tk_->variantToReal(args[3], yc))
        return tk_->invalidReal(args[2]);

      auto *circle = qcanvas_->addCircle(xc, yc);

      uint i = 4;

      if (! processShapeOpts(circle, i))
        return false;

      tk_->setIntegerResult(circle->id());
    }
    else if (type == "ellipse") {
      if (numArgs < 4)
        return tk_->wrongNumArgs(getFullName() + " create ellipse xc yc ?arg ...?");

      double xc;
      if (! tk_->variantToReal(args[2], xc))
        return tk_->invalidReal(args[2]);
      double yc;
      if (! tk_->variantToReal(args[3], yc))
        return tk_->invalidReal(args[3]);

      auto *ellipse = qcanvas_->addEllipse(xc, yc);

      uint i = 4;

      if (! processShapeOpts(ellipse, i))
        return false;

      tk_->setIntegerResult(ellipse->id());
    }
    else if (type == "group") {
      tk_->TODO(args);
    }
    else if (type == "path") {
      if (numArgs < 3)
        return tk_->wrongNumArgs(getFullName() + " create path path ?arg ...?");

      auto pathStr = tk_->variantToString(args[2]);

      QPainterPath qpath;
      if (! CTkAppUtil::stringToPath(pathStr, qpath))
        return tk_->throwError("Invalid path \"" + pathStr + "\"");

      uint i = 3;

      //---

      auto *path = qcanvas_->addPath(pathStr, qpath);

      if (! processShapeOpts(path, i))
        return false;

      tk_->setIntegerResult(path->id());
    }
#endif
    else if (type == "polygon") {
      uint i = 2;

      std::vector<QString> pointStrs;
      if (! getPointStrs(i, pointStrs) || pointStrs.empty())
        return tk_->wrongNumArgs(getFullName() + " create polygon coords ?arg ...?");

      if (pointStrs.size() & 1)
        return tk_->throwError(tk_->msg() + "wrong # coordinates: expected an even number, "
                               "got " + QString::number(pointStrs.size()));

      CTkAppCanvasWidget::Points points;
      if (! strsToPoints(pointStrs, points))
        return false;

      if (points.size() < 2)
        return tk_->wrongNumArgs(getFullName() + " create polygon coords ?arg ...?");

      //---

      auto *polygon = qcanvas_->addPolygon(points);

      if (! processShapeOpts(polygon, i))
        return false;

      tk_->setIntegerResult(polygon->id());
    }
    else if (type == "rectangle" || type == "prect") {
#ifndef CTK_APP_TKPATH
      if (type == "prect")
        return tk_->throwError("prect supported without tkpath support");
#endif

      if (numArgs < 6)
        return tk_->wrongNumArgs(getFullName() + " create rectangle coords ?arg ...?");

      CTkAppDistance x1, y1, x2, y2;
      if (! variantToDistance(args[2], x1) || ! variantToDistance(args[3], y1) ||
          ! variantToDistance(args[4], x2) || ! variantToDistance(args[5], y2))
        return false;

      //---

      auto *rectangle = qcanvas_->addRectangle(x1.rvalue, y1.rvalue, x2.rvalue, y2.rvalue);

      uint i = 6;

      if (! processShapeOpts(rectangle, i))
        return false;

      tk_->setIntegerResult(rectangle->id());
    }
    else if (type == "text" || type == "ptext") {
#ifndef CTK_APP_TKPATH
      if (type == "ptext")
        return tk_->throwError("ptext supported without tkpath support");
#endif

      if (numArgs < 3)
        return tk_->wrongNumArgs(getFullName() + " create text coords ?arg ...?");

      uint i = 2;

      std::vector<QString> pointStrs;
      if (! getPointStrs(i, pointStrs, 2) || pointStrs.empty())
        return tk_->wrongNumArgs(getFullName() + " create text coords ?arg ...?");

      if (pointStrs.size() != 2)
       return tk_->wrongNumArgs("wrong # coordinates: expected 2, got " +
                                QString::number(pointStrs.size()));

      CTkAppCanvasWidget::Points points;
      if (! strsToPoints(pointStrs, points))
        return false;

      auto *text = qcanvas_->addText(points[0], "");

      if (! processShapeOpts(text, i))
        return false;

      tk_->setIntegerResult(text->id());
    }
    else if (type == "window") {
      if (numArgs < 3)
        return tk_->wrongNumArgs(getFullName() + " create window coords ?arg ...?");

      uint i = 2;

      std::vector<QString> pointStrs;
      if (! getPointStrs(i, pointStrs, 2) || pointStrs.empty())
        return tk_->wrongNumArgs(getFullName() + " create window coords ?arg ...?");

      if (pointStrs.size() != 2)
       return tk_->wrongNumArgs("wrong # coordinates: expected 2, got " +
                                QString::number(pointStrs.size()));

      CTkAppCanvasWidget::Points points;
      if (! strsToPoints(pointStrs, points))
        return false;

      //---

      auto *window = qcanvas_->addWindow(points[0]);

      if (! processShapeOpts(window, i))
        return false;

      tk_->setIntegerResult(window->id());
    }
    else
      return tk_->throwError(tk_->msg() + "unknown type \"" + type + "\"");

    qcanvas_->update();
  }
  else if (option == "dchars") {
    if (numArgs < 3 || numArgs > 4)
      return tk_->wrongNumArgs(getFullName() + " dchars tagOrId first ?last?");

    auto name  = tk_->variantToString(args[1]);
    auto first = tk_->variantToString(args[2]);

    QString last;
    if (numArgs > 3)
      last = tk_->variantToString(args[3]);

    CTkAppCanvasWidget::Shapes shapes;
    if (! qcanvas_->getShapes(name, shapes))
      return false;

    for (auto *shape : shapes) {
      if (! shape->deleteChars(first, last))
        return false;
    }
  }
  else if (option == "delete") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " delete ?arg ...?"); // delete all ?

    auto rc = true;

    for (uint i = 1; i < numArgs; ++i) {
      auto name = tk_->variantToString(args[i]);

      if (! qcanvas_->deleteShape(name))
        rc = false;
    }

    if (! rc)
      return false;

    qcanvas_->update();
  }
  else if (option == "dtag") {
    if (numArgs < 2 || numArgs > 3)
      return tk_->wrongNumArgs(getFullName() + " dtag tagOrId ?tagToDelete?");

    auto name = tk_->variantToString(args[1]);
    auto tag  = (numArgs > 2 ? tk_->variantToString(args[2]) : name);

    CTkAppCanvasWidget::Shapes shapes;
    if (! qcanvas_->getShapes(name, shapes))
      return false;

    for (auto *shape : shapes)
      shape->deleteTag(tag);
  }
  else if (option == "find") {
    auto setShapesResult = [&](const CTkAppCanvasWidget::Shapes &shapes) {
      std::vector<int> ids;
      for (auto *shape : shapes)
        ids.push_back(shape->id());

      tk_->setIntegerArrayResult(ids);
    };

    //---

    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " find searchCommand ?arg ...?");

    auto cmd = tk_->variantToString(args[1]);

    static auto cmdNames = std::vector<QString>({
      "above", "all", "below", "closest", "enclosed", "overlapping", "withtag"
    });

    QString cmd1;
    if (! tk_->lookupOptionName(cmdNames, cmd, cmd1))
      return false;

    if      (cmd1 == "above") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " find above tagOrId");

      auto aboveTag = tk_->variantToString(args[2]);

      CTkAppCanvasWidget::Shapes shapes;
      if (! qcanvas_->getShapes(aboveTag, shapes))
        return false;

      CTkAppCanvasWidget::Shapes aboveShapes;

      if (! shapes.empty()) {
        auto *shape = shapes.back();

        qcanvas_->getAboveShapes(shape, aboveShapes);
      }

      setShapesResult(aboveShapes);
    }
    else if (cmd1 == "all") {
      if (numArgs != 2)
        return tk_->wrongNumArgs(getFullName() + " find all");

      CTkAppCanvasWidget::Shapes shapes;
      qcanvas_->getAllShapes(shapes);

      setShapesResult(shapes);
    }
    else if (cmd1 == "below") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " find below tagOrId");

      auto belowTag = tk_->variantToString(args[2]);

      CTkAppCanvasWidget::Shapes shapes;
      if (! qcanvas_->getShapes(belowTag, shapes))
        return false;

      CTkAppCanvasWidget::Shapes belowShapes;

      if (! shapes.empty()) {
        auto *shape = shapes.back();

        qcanvas_->getBelowShapes(shape, belowShapes);
      }

      setShapesResult(belowShapes);
    }
    else if (cmd1 == "closest") {
      if (numArgs < 4 || numArgs > 6)
        return tk_->wrongNumArgs(getFullName() + " find closest x y ?halo? ?start?");

      double x, y;
      if (! tk_->variantToReal(args[2], x))
        return tk_->invalidReal(args[2]);
      if (! tk_->variantToReal(args[3], y))
        return tk_->invalidReal(args[3]);

      long halo = -1;
      if (numArgs > 4) {
        if (! tk_->variantToInt(args[4], halo))
          return tk_->invalidInteger(args[4]);
      }

      long start;
      if (numArgs > 5) {
        if (! tk_->variantToInt(args[5], start))
          return tk_->invalidInteger(args[5]);
      }

      auto p = CTkAppPoint(x, y);

      CTkAppCanvasWidget::Shapes shapes;
      qcanvas_->findNearestShapes(p, shapes);

      auto *shape = (! shapes.empty() ? shapes[0] : nullptr);

      if (shape)
        tk_->setIntegerResult(shape->id());
      else
        tk_->setStringResult("");
    }
    else if (cmd1 == "enclosed") {
      if (numArgs != 6)
        return tk_->wrongNumArgs(getFullName() + " find enclosed x1 y1 x2 y2");

      double x1, y1, x2, y2;
      if (! tk_->variantToReal(args[2], x1))
        return tk_->invalidReal(args[2]);
      if (! tk_->variantToReal(args[3], y1))
        return tk_->invalidReal(args[3]);
      if (! tk_->variantToReal(args[4], x2))
        return tk_->invalidReal(args[4]);
      if (! tk_->variantToReal(args[5], y2))
        return tk_->invalidReal(args[5]);

      auto r = QRectF(x1, y1, x2 - x1, y2 - y1);

      CTkAppCanvasWidget::Shapes shapes;
      qcanvas_->findEnclosedShapes(r, shapes);

      setShapesResult(shapes);
    }
    else if (cmd1 == "overlapping") {
      if (numArgs != 6)
        return tk_->wrongNumArgs(getFullName() + " find overlapping x1 y1 x2 y2");

      double x1, y1, x2, y2;
      if (! tk_->variantToReal(args[2], x1))
        return tk_->invalidReal(args[2]);
      if (! tk_->variantToReal(args[3], y1))
        return tk_->invalidReal(args[3]);
      if (! tk_->variantToReal(args[4], x2))
        return tk_->invalidReal(args[4]);
      if (! tk_->variantToReal(args[5], y2))
        return tk_->invalidReal(args[5]);

      auto r = QRectF(x1, y1, x2 - x1, y2 - y1);

      CTkAppCanvasWidget::Shapes shapes;
      qcanvas_->findOverlappingShapes(r, shapes);

      setShapesResult(shapes);
    }
    else if (cmd1 == "withtag") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " find withtag tagOrId");

      auto withTag = tk_->variantToString(args[2]);

      CTkAppCanvasWidget::Shapes shapes;

      if (! qcanvas_->getShapes(withTag, shapes))
        return false;

      if (! shapes.empty())
        setShapesResult(shapes);
      else
        tk_->setStringResult("");
    }
  }
  else if (option == "focus") {
    if (numArgs != 1 && numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " focus ?tagOrId?");

    if (numArgs == 1) {
      auto *shape = qcanvas_->focusShape();

      if (shape)
        tk_->setIntegerResult(shape->id());
      else
        tk_->setStringResult("");
    }
    else {
      auto withTag = tk_->variantToString(args[1]);

      CTkAppCanvasWidget::Shapes shapes;
      if (! qcanvas_->getShapes(withTag, shapes))
        return false;

      for (auto *shape : shapes) {
        if (shape->canFocus()) {
          qcanvas_->setFocusShape(shape);
          break;
        }
      }
    }
  }
  else if (option == "gettags") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " gettags tagOrId");

    auto id = tk_->variantToString(args[1]);

    auto *shape = qcanvas_->getShape(id);

    if (shape) {
      auto tags = shape->tags();

      std::vector<QString> tags1;
      for (const auto &tag : tags)
        tags1.push_back(tag);

      tk_->setStringArrayResult(tags1);
    }
    else
      tk_->setStringResult("");
  }
#ifdef CTK_APP_TKPATH
  else if (option == "gradient") {
    if (numArgs != 4)
      return tk_->wrongNumArgs(getFullName() + " gradient create type args");

    auto opt = tk_->variantToString(args[1]);

    if      (opt == "cget") {
      tk_->TODO(args);
    }
    else if (opt == "configure" || opt == "config") {
      tk_->TODO(args);
    }
    else if (opt == "create") {
      auto type = tk_->variantToString(args[2]);

      if      (type == "linear") {
        double x1 { 0.0 }, y1 { 0.0 }, x2 { 1.0 }, y2 { 1.0 };

        std::map<double, QColor> stops;

        uint i = 3;

        for ( ; i < numArgs; ++i) {
          auto name = tk_->variantToString(args[i]);

          if      (name == "-stops") {
            ++i;

            auto value = (i < numArgs ? tk_->variantToString(args[i]) : "");

            std::vector<QString> strs;
            if (! tk_->splitList(value, strs))
              return tk_->throwError("Invalid -stops \"" + value + "\"");

            for (auto &str : strs) {
              std::vector<QString> strs1;
              if (! tk_->splitList(str, strs1))
                return tk_->throwError("Invalid -stops \"" + value + "\"");

              if (strs1.size() < 2)
                return tk_->throwError("Invalid -stops \"" + value + "\"");

              double r;
              if (! tk_->variantToReal(strs1[0], r))
                return tk_->invalidReal(strs1[0]);

              QColor c;
              if (! tk_->variantToQColor(strs1[1], c))
                return tk_->invalidQColor(strs1[1]);
              stops[r] = c;
            }
          }
          else if (name == "-lineartransition") {
            ++i;

            auto value = (i < numArgs ? tk_->variantToString(args[i]) : "");

            std::vector<QString> strs;
            if (! tk_->splitList(value, strs))
              return tk_->throwError("Invalid -lineartransition \"" + value + "\"");

            if (strs.size() != 4)
              return tk_->throwError("Invalid -lineartransition \"" + value + "\"");

            if (! tk_->variantToReal(strs[0], x1))
              return tk_->invalidReal(strs[0]);
            if (! tk_->variantToReal(strs[1], y1))
              return tk_->invalidReal(strs[1]);
            if (! tk_->variantToReal(strs[2], x2))
              return tk_->invalidReal(strs[2]);
            if (! tk_->variantToReal(strs[3], y2))
              return tk_->invalidReal(strs[3]);
          }
          else if (name == "-units") {
            ++i;

            tk_->TODO(args);
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
          auto name = tk_->variantToString(args[i]);

          if      (name == "-stops") {
            ++i;

            auto value = (i < numArgs ? tk_->variantToString(args[i]) : "");

            std::vector<QString> strs;
            if (! tk_->splitList(value, strs))
              return tk_->throwError("Invalid -stops \"" + value + "\"");

            for (auto &str : strs) {
              std::vector<QString> strs1;
              if (! tk_->splitList(str, strs1))
                return tk_->throwError("Invalid -stops \"" + value + "\"");

              if (strs1.size() < 2)
                return tk_->throwError("Invalid -stops \"" + value + "\"");

              double r;
              if (! tk_->variantToReal(strs1[0], r))
                return tk_->invalidReal(strs1[0]);

              QColor c;
              if (! tk_->variantToQColor(strs1[1], c))
                return tk_->invalidQColor(strs1[1]);
              stops[r] = c;
            }
          }
          else if (name == "-units") {
            ++i;

            tk_->TODO(args);
          }
          else if (name == "-radialtransition") {
            ++i;

            tk_->TODO(args);
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
        tk_->TODO(args);
    }
    else if (opt == "delete") {
      tk_->TODO(args);
    }
    else if (opt == "inuse") {
      tk_->TODO(args);
    }
    else if (opt == "names") {
      tk_->TODO(args);
    }
    else if (opt == "type") {
      tk_->TODO(args);
    }
    else
      tk_->TODO(args);
  }
#endif
  else if (option == "icursor") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " icursor tagOrId index");

    auto id = tk_->variantToString(args[1]);

    CTkAppCanvasWidget::Shapes shapes;
    if (! qcanvas_->getShapes(id, shapes))
      return false;

    auto ind = tk_->variantToString(args[2]);

    for (auto *shape : shapes)
      shape->setInsertCursor(ind);
  }
  else if (option == "image") {
    if (numArgs < 2 || numArgs > 4)
      return tk_->wrongNumArgs(getFullName() + " image imagename ?subsample? ?zoom?");

    auto imageName = tk_->variantToString(args[1]);

    long subsample = 1;
    if (numArgs > 2) {
      if (! tk_->variantToInt(args[2], subsample))
        return tk_->invalidInteger(args[2]);
    }

    long zoom = 1;
    if (numArgs > 3) {
      if (! tk_->variantToInt(args[3], zoom))
        return tk_->invalidInteger(args[3]);
    }

    CTkAppImageRef image;
    if (! qcanvas_->saveImage(imageName, subsample, zoom, image))
      return false;
  }
  else if (option == "imove") {
    if (numArgs != 5)
      return tk_->wrongNumArgs(getFullName() + " imove tagOrId index x y");

    auto id  = tk_->variantToString(args[1]);
    auto ind = tk_->variantToString(args[2]);

    CTkAppDistance x, y;
    if (! tk_->variantToDistance(args[3], x))
      return tk_->invalidDistance(args[3]);
    if (! tk_->variantToDistance(args[4], y))
      return tk_->invalidDistance(args[4]);

    CTkAppPoint p(x.rvalue, y.rvalue);

    CTkAppCanvasWidget::Shapes shapes;
    if (! qcanvas_->getShapes(id, shapes))
      return false;

    for (auto *shape : shapes) {
      if (! shape->movePoint(ind, p))
        return false;
    }
  }
  else if (option == "index") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " index tagOrId string");

    auto id  = tk_->variantToString(args[1]);
    auto ind = tk_->variantToString(args[2]);

    auto *shape = qcanvas_->getShape(id);

    long pos;
    if (! shape->indexPos(ind, pos))
      return false;

    tk_->setIntegerResult(pos);
  }
  else if (option == "insert") {
    if (numArgs != 4)
      return tk_->wrongNumArgs(getFullName() + " insert tagOrId beforeThis string");

    auto id  = tk_->variantToString(args[1]);
    auto pos = tk_->variantToString(args[2]);
    auto str = tk_->variantToString(args[3]);

    auto *shape = qcanvas_->getShape(id);

    if (! shape->insertChars(pos, str))
      return false;
  }
  else if (option == "itemcget") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " itemcget tagOrId option");

    auto id = tk_->variantToString(args[1]);

    CTkAppCanvasWidget::Shapes shapes;

    if (id == "root")
      qcanvas_->getAllShapes(shapes);
    else {
      if (! qcanvas_->getShapes(id, shapes))
        return false;
    }

    std::vector<QVariant> values;

    for (uint i = 2; i < numArgs; ++i) {
      auto name = tk_->variantToString(args[i]);

      if (name[0] == '-') {
        bool rc = false;

        for (auto *shape : shapes) {
          QVariant value;
          if (shape->getShapeOpt(name, value)) {
            values.push_back(value);
            rc = true;
          }
        }

        if (! rc)
          return false;
      }
      else
        return false;
    }

    if (! values.empty())
      tk_->setResult(values[0]);
  }
  else if (option == "itemconfigure") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " itemconfigure tagOrId ?-option value ...?");

    auto id = tk_->variantToString(args[1]);

    CTkAppCanvasWidget::Shapes shapes;
    if (! qcanvas_->getShapes(id, shapes))
      return false;

    uint i = 2;

    for ( ; i < numArgs; i += 2) {
      auto name = tk_->variantToString(args[i]);

      if (name[0] == '-') {
        if (numArgs == 3) {
          for (auto *shape : shapes) {
            QVariant value;
            if (shape->getShapeOpt(name, value)) {
              QVariant defValue;
              (void) shape->getShapeDefault(name, defValue);

              std::vector<QVariant> vars;

              vars.push_back(name);
              vars.push_back(QString());
              vars.push_back(QString());
              vars.push_back(defValue);
              vars.push_back(value);

              tk_->setVariantArrayResult(vars);

              break;
            }
          }

          break;
        }

        if (i + 1 >= numArgs)
          return false;

        auto value = tk_->variantToString(args[i + 1]);

        bool rc = true;

        // setShapeOpt: returns false if not handled, rc is error state
        for (auto *shape : shapes) {
          bool rc1;
          if (shape->setShapeOpt(name, value, rc1) && ! rc1)
            rc = false;
        }

        if (! rc)
          return false;
      }
      else
        return false;
    }
  }
  else if (option == "lower") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " lower tagOrId ?belowThis?");

    auto name = tk_->variantToString(args[1]);

    if (! qcanvas_->lowerShape(name))
      return false;
  }
  else if (option == "move") {
    if (numArgs != 4)
      return tk_->wrongNumArgs(getFullName() + " move tagOrId xAmount yAmount");

    auto id = tk_->variantToString(args[1]);

    CTkAppDistance dx, dy;
    if (! variantToDistance(args[2], dx) || ! variantToDistance(args[3], dy))
      return false;

    if (! qcanvas_->moveShapeBy(id, dx.rvalue, dy.rvalue))
      return false;
  }
  else if (option == "moveto") {
    if (numArgs != 4)
      return tk_->wrongNumArgs(getFullName() + " moveto tagOrId x y");

    auto id = tk_->variantToString(args[1]);

    OptReal x, y;
    if (tk_->variantIsValid(args[2])) {
      CTkAppDistance dx;
      if (! variantToDistance(args[2], dx))
        return false;
      x = dx.rvalue;
    }
    if (tk_->variantIsValid(args[3])) {
      CTkAppDistance dy;
      if (! variantToDistance(args[3], dy))
        return false;
      y = dy.rvalue;
    }

    if (! qcanvas_->moveShapeTo(id, x, y))
      return false;
  }
  else if (option == "postscript") {
    if (! (numArgs & 1))
      return tk_->wrongNumArgs(getFullName() + " postscript ?option value option value ...?");

    uint i = 1;

    for ( ; i < numArgs; i += 2) {
      auto name  = tk_->variantToString(args[i]);
      auto value = args[i + 1];

      if      (name == "-channel") {
        tk_->TODO(name);
      }
      else if (name == "-colormap") {
        tk_->TODO(name);
      }
      else if (name == "-file") {
        tk_->TODO(name);
      }
      else if (name == "-fontmap") {
        tk_->TODO(name);
      }
      else if (name == "-height") {
        tk_->TODO(name);
      }
      else if (name == "-pageanchor") {
        tk_->TODO(name);
      }
      else if (name == "-pageheight") {
        tk_->TODO(name);
      }
      else if (name == "-pagetwidth") {
        tk_->TODO(name);
      }
      else if (name == "-pagex") {
        tk_->TODO(name);
      }
      else if (name == "-pagey") {
        tk_->TODO(name);
      }
      else if (name == "-rotate") {
        tk_->TODO(name);
      }
      else if (name == "-width") {
        tk_->TODO(name);
      }
      else if (name == "-x") {
        tk_->TODO(name);
      }
      else if (name == "-y") {
        tk_->TODO(name);
      }
      else {
        tk_->throwError("Invalid option \"" + name + "\"");
      }
    }

    tk_->TODO(option, args);
  }
  else if (option == "raise") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " raise tagOrId ?belowThis?");

    auto name = tk_->variantToString(args[1]);

    if (! qcanvas_->raiseShape(name))
      return false;
  }
  else if (option == "rchars") {
    if (numArgs != 5)
      return tk_->wrongNumArgs(getFullName() + " rchars tagOrId first last string");

    auto id = tk_->variantToString(args[1]);

    auto first = tk_->variantToString(args[2]);
    auto last  = tk_->variantToString(args[3]);
    auto str   = tk_->variantToString(args[4]);

    CTkAppCanvasWidget::Shapes shapes;
    if (! qcanvas_->getShapes(id, shapes))
      return false;

    for (auto *shape : shapes) {
      if (! shape->replaceChars(first, last, str))
        return false;
    }
  }
  else if (option == "rotate") {
    if (numArgs != 5)
      return tk_->wrongNumArgs(getFullName() + " rotate tagOrId x y angle");

    auto id = tk_->variantToString(args[1]);

    CTkAppDistance x, y;
    if (! variantToDistance(args[2], x) || ! variantToDistance(args[3], y))
      return false;

    double a;
    if (! tk_->variantToReal(args[4], a))
      return tk_->invalidReal(args[4]);

    if (! qcanvas_->rotateShape(id, x.rvalue, y.rvalue, a))
      return false;
  }
  else if (option == "scale") {
    if (numArgs != 6)
      return tk_->wrongNumArgs(getFullName() + " scale tagOrId xOrigin yOrigin xScale yScale");

    auto name = tk_->variantToString(args[1]);

    double xc, yc, sx, sy;
    if (! tk_->variantToReal(args[2], xc))
      return tk_->invalidReal(args[2]);
    if (! tk_->variantToReal(args[3], yc))
      return tk_->invalidReal(args[3]);
    if (! tk_->variantToReal(args[4], sx))
      return tk_->invalidReal(args[4]);
    if (! tk_->variantToReal(args[5], sy))
      return tk_->invalidReal(args[5]);

    if (! qcanvas_->scaleShape(name, xc, yc, sx, sy))
      return false;
  }
  else if (option == "scan") {
    if (numArgs != 4 && numArgs != 5)
      return tk_->wrongNumArgs(getFullName() + " scan mark|dragto x y ?dragGain?");

    static auto scanNames = std::vector<QString>({"mark", "dragto"});

    QString scanName;
    if (! tk_->lookupOptionName(scanNames, args[1], scanName))
      return false;

    double x, y;
    if (! tk_->variantToReal(args[2], x))
      return tk_->invalidReal(args[2]);
    if (! tk_->variantToReal(args[3], y))
      return tk_->invalidReal(args[3]);

    long gain = 10;
    if (numArgs == 5) {
      if (! tk_->variantToInt(args[4], gain))
        return tk_->invalidInteger(args[4]);
    }

    tk_->TODO(option, args);
  }
  else if (option == "select") {
    if (numArgs < 2 || numArgs > 4)
      return tk_->wrongNumArgs(getFullName() + " select option ?tagOrId? ?arg?");

    static auto selectNames = std::vector<QString>({"adjust", "clear", "from", "item", "to"});

    QString selectName;
    if (! tk_->lookupOptionName(selectNames, args[1], selectName))
      return false;

    if      (selectName == "adjust") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " select adjust tagOrId index");

      auto id  = tk_->variantToString(args[2]);
      auto ind = tk_->variantToString(args[3]);

      auto *shape = qcanvas_->getShape(id);

      if (shape && ! shape->adjustSelect(ind))
        return false;
    }
    else if (selectName == "clear") {
      if (numArgs != 2)
        return tk_->wrongNumArgs(getFullName() + " select clear");

      CTkAppCanvasWidget::Shapes shapes;
      qcanvas_->getAllShapes(shapes);

      for (auto *shape : shapes)
        shape->clearSelect();
    }
    else if (selectName == "from") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " select from tagOrId index");

      auto id  = tk_->variantToString(args[2]);
      auto ind = tk_->variantToString(args[3]);

      auto *shape = qcanvas_->getShape(id);

      if (shape && ! shape->setSelectFrom(ind))
        return false;
    }
    else if (selectName == "item") {
      if (numArgs != 2)
        return tk_->wrongNumArgs(getFullName() + " select item");

      tk_->TODO(args);
    }
    else if (selectName == "to") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " select to tagOrId index");

      auto id  = tk_->variantToString(args[2]);
      auto ind = tk_->variantToString(args[3]);

      auto *shape = qcanvas_->getShape(id);

      if (shape && ! shape->setSelectTo(ind))
        return false;
    }
    else
      return tk_->wrongNumArgs("Invalid options \"" + selectName + "\"");
  }
#ifdef CTK_APP_TKPATH
  else if (option == "style") {
    tk_->TODO(args);
  }
#endif
  else if (option == "type") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " type tag");

    auto id = tk_->variantToString(args[1]);

    auto *shape = qcanvas_->getShape(id);

    if (shape)
      tk_->setStringResult(shape->typeName());
    else
      tk_->setStringResult("");
  }
  else if (option == "xview") {
    if      (numArgs == 1) {
      double x1 = qcanvas_->scrollRelX1();
      double x2 = qcanvas_->scrollRelX2();

      tk_->setRealArrayResult({x1, x2});
    }
    else if (args[1] == "moveto") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " xview moveto fraction");

      double f;
      if (! tk_->variantToReal(args[2], f))
        return tk_->invalidReal(args[2]);

      if (f < 0.0 || f > 1.0)
        return false;

      qcanvas_->setScrollX(f);
    }
    else if (args[1] == "scroll") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " xview scroll number what");

      double r;
      if (! tk_->variantToReal(args[2], r))
        return false;
      long i = long(std::ceil(r));

      static auto names = std::vector<QString>({"pages", "units"});

      QString name;
      if (! tk_->lookupName("what", names, args[3], name))
        return tk_->throwError(tk_->msg() + "bad argument \"" + args[3] +
                               "\": must be pages or units");

      auto scrollX = qcanvas_->scrollX();

      double pixels = double(i);

      if (name == "pages")
        pixels *= qcanvas_->width();
      else {
        if (qcanvas_->xScrollIncrement() > 0)
          pixels *= qcanvas_->xScrollIncrement();
      }

      double dx = 1.0/double(qcanvas_->width());

      qcanvas_->setScrollX(scrollX + pixels*dx);
    }
    else
      return false;
  }
  else if (option == "yview") {
    if      (numArgs == 1) {
      double y1 = qcanvas_->scrollRelY1();
      double y2 = qcanvas_->scrollRelY2();

      tk_->setRealArrayResult({y1, y2});
    }
    else if (args[1] == "moveto") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " yview moveto fraction");

      double f;
      if (! tk_->variantToReal(args[2], f))
        return tk_->invalidReal(args[2]);

      qcanvas_->setScrollY(f);
    }
    else if (args[1] == "scroll") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " yview scroll number what");

      double r;
      if (! tk_->variantToReal(args[2], r))
        return false;
      long i = long(std::ceil(r));

      static auto names = std::vector<QString>({"pages", "units"});

      QString name;
      if (! tk_->lookupName("what", names, args[3], name))
        return tk_->throwError(tk_->msg() + "bad argument \"" + args[3] +
                               "\": must be pages or units");

      auto scrollY = qcanvas_->scrollY();

      double pixels = double(i);

      if (name == "pages")
        pixels *= qcanvas_->height();
      else {
        if (qcanvas_->yScrollIncrement() > 0)
          pixels *= qcanvas_->yScrollIncrement();
      }

      double dy = 1.0/double(qcanvas_->height());

      qcanvas_->setScrollY(scrollY + pixels*dy);
    }
    else
      return false;
  }
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

bool
CTkAppCanvas::
triggerMousePressEvents(QEvent *e, int button)
{
  auto *currentShape = qcanvas_->currentShape();

  if (currentShape) {
    auto *me = static_cast<QMouseEvent *>(e);

    CTkAppEventData matchEventData;

    tk_->encodeEvent(me, CTkAppEventMode::Press, button, matchEventData);

    processShapeEvents(currentShape, e, matchEventData);
  }

  return CTkAppWidget::triggerMousePressEvents(e, button);
}

bool
CTkAppCanvas::
triggerMouseMoveEvents(QEvent *e, int button, bool pressed)
{
  auto *currentShape = qcanvas_->currentShape();

  if (! pressed) {
    auto *me = static_cast<QMouseEvent *>(e);

    auto p = CTkAppPoint(me->x(), me->y());

    auto p1 = qcanvas_->pixelToWindow(p);

    auto *newInsideShape = qcanvas_->insideShape(p1);
    auto *oldInsideShape = currentShape;

    if (newInsideShape != oldInsideShape) {
      // Leave old inside shape
      if (oldInsideShape) {
        //std::cerr << "Leave " << oldInsideShape->id() << "\n";

        CTkAppEventData matchEventData;
        matchEventData.type = CTkAppEventType::Leave;

        processShapeEvents(oldInsideShape, e, matchEventData);
      }

      qcanvas_->setCurrentShape(newInsideShape);

      // Enter new inside shape
      if (newInsideShape) {
        //std::cerr << "Enter " << newInsideShape->id() << "\n";

        CTkAppEventData matchEventData;
        matchEventData.type = CTkAppEventType::Enter;

        processShapeEvents(newInsideShape, e, matchEventData);
      }

      qcanvas_->update();
    }
  }
  else {
    if (currentShape) {
      auto *me = static_cast<QMouseEvent *>(e);

      CTkAppEventData matchEventData;

      tk_->encodeEvent(me, CTkAppEventMode::Motion, button, matchEventData);

      processShapeEvents(currentShape, e, matchEventData);
    }
  }

  return CTkAppWidget::triggerMouseMoveEvents(e, button, pressed);
}

void
CTkAppCanvas::
processShapeEvents(Shape *shape, QEvent *e, const CTkAppEventData &matchEventData)
{
  auto idStr = QString::number(shape->id());

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
