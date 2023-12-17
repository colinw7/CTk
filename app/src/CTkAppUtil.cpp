#include <CTkAppUtil.h>
#include <CTkApp.h>

#include <CQStrParse.h>
#include <CSVGUtil.h>
#include <CScreenUnits.h>
#include <CMatrix2D.h>
#include <CRGBName.h>
#include <CStrUtil.h>

#include <QFrame>
#include <QLineEdit>
#include <QTextEdit>
#include <QVariant>
#include <set>

namespace CTkAppUtil {

bool
stringToInt(const QString &str, long &i)
{
  try {
    size_t pos;
    i = std::stol(str.toStdString(), &pos);
    if (pos != size_t(str.length()))
      return false;
  }
    catch (...) {
    return false;
  }

  return true;
}

bool
stringToReal(const QString &str, double &r)
{
  try {
    size_t pos;
    r = std::stod(str.toStdString(), &pos);
    if (pos != size_t(str.length()))
      return false;
  }
    catch (...) {
    return false;
  }

  return true;
}

bool
stringToBool(const QString &str, bool &b)
{
  auto lstr = str.toLower();

  if      (lstr == "1" || lstr == "yes" || lstr == "true" || lstr == "y" || lstr == "t")
    b = true;
  else if (lstr == "0" || lstr == "no" || lstr == "false" || lstr == "n" || lstr == "f")
    b = false;
  else
    return false;

  return true;
}

bool
variantToQColor(CTclApp *app, const QVariant &var, QColor &c)
{
  auto str = variantToString(app, var);

  if (str == "green") {
    c = QColor(0, 128, 0); // html green
    return true;
  }

  std::vector<QString> strs;
  if (app->splitList(str, strs) && (strs.size() == 3 || strs.size() == 4)) {
    long r, g, b, a;
    if (strs.size() == 3) {
      if (stringToInt(strs[0], r) && stringToInt(strs[1], g) && stringToInt(strs[2], b)) {
        c = QColor(r, g, b);
        return true;
      }
    }
    else {
      if (stringToInt(strs[0], r) && stringToInt(strs[1], g) &&
          stringToInt(strs[2], b) && stringToInt(strs[3], a)) {
        c = QColor(r, g, b, a);
        return true;
      }
    }
  }

  double r, g, b, a;
  if (CRGBName::lookup(str.toStdString(), &r, &g, &b, &a))
    c = QColor(255*r, 255*g, 255*b, 255*a);
  else {
    if (str[0] == '#')
      return false;

    auto pos = str.indexOf('#');

    if (pos > 0) {
      auto lhs = str.mid(0, pos);
      auto rhs = str.mid(pos + 1);

      if (! CStrUtil::isBaseInteger(rhs.toStdString(), 16))
        return false;

      auto a = CStrUtil::toBaseInteger(rhs.toStdString(), 16);

      c = QColor(lhs);

      c.setAlpha(a);
    }
    else
      c = QColor(str);
  }

  return c.isValid();
}

bool
variantToQFont(CTclApp *app, const QVariant &var, QFont &f)
{
  auto str = variantToString(app, var);

  CQStrParse parse(str);

  parse.skipSpace();

  QString family;

  if (! parse.readNonSpace(family))
    return false;

  //std::cerr << family << " ";

  f.setFamily(family);

  //---

  parse.skipSpace();

  int  size   = 0;
  bool points = true;

  if (! parse.readInteger(&size))
    return false;

  if (size < 0) {
    size   = -size;
    points = false;
  }

  //std::cerr << size << (points ? "pt" : "px") << " ";

  if (points)
    f.setPointSize(size);
  else
    f.setPixelSize(size);

  //---

  parse.skipSpace();

  std::vector<QString> styles;

  while (! parse.eof()) {
    QString style;

    if (! parse.readNonSpace(style))
      return false;

    styles.push_back(style);
  }

  //for (const auto &style : styles)
  //  std::cerr << style << " ";

  for (const auto &style : styles) {
    if      (style == "bold")
      f.setBold(true);
    else if (style == "italic")
      f.setItalic(true);
    else if (style == "underline")
      f.setUnderline(true);
    else
      std::cerr << "Unhandled style '" << style.toStdString() << "'\n";
  }

  //---

  //std::cerr << "\n";

  return true;
}

bool
variantToDistanceI(CTclApp *app, const QVariant &var, Distance &d)
{
  if (! variantToDistance(app, var, d))
    return false;

  if (d.isReal)
    return false;

  d.ivalue = long(d.rvalue);
  d.rvalue = double(d.ivalue);

  return true;
}

bool
variantToDistance(CTclApp *app, const QVariant &var, Distance &d)
{
  d = Distance();

  auto str = variantToString(app, var);

  CQStrParse parse(str);

  parse.skipSpace();

  if (! parse.readNumber(d.r, d.i, d.isReal))
    return false;

  if (! d.isReal)
    d.r = d.i;
  else
    d.i = long(d.r);

  parse.skipSpace();

  if (! parse.eof()) {
    auto c = parse.readChar();

    if      (c == 'm') {
      d.units = Distance::Units::MM;
      d.f     = 1.0/25.4;
    }
    else if (c == 'c') {
      d.units = Distance::Units::CM;
      d.f     = 1.0/2.54;
    }
    else if (c == 'i') {
      d.units = Distance::Units::IN;
      d.f     = 1.0;
    }
    else if (c == 'p') {
      d.units = Distance::Units::PT;
      d.f     = 1.0/72.0;
    }
    else
      return false;

    auto dpi = CScreenUnitsMgrInst->dpi();
  //double dpi = 72.0;

    d.rvalue = d.r*dpi*d.f;
    d.ivalue = long(d.rvalue);
  }
  else {
    d.rvalue = d.r;
    d.ivalue = d.i;
  }

  return true;
}

bool
variantToOrient(CTclApp *app, const QVariant &var, Qt::Orientation &orient)
{
  static auto names = std::vector<QString>({ "horizontal", "vertical" });

  auto str = variantToString(app, var);

  QString match;
  int     nmatch;
  if (! uniqueMatch(names, str, match, nmatch))
    return false;

  orient = Qt::Horizontal;

  if (match == "horizontal") orient = Qt::Horizontal;
  if (match == "vertical"  ) orient = Qt::Vertical;

  return true;
}

bool
variantToAlign(CTclApp *app, const QVariant &var, Qt::Alignment &align)
{
  auto value = variantToString(app, var);

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
  else return false;

  return true;
}

QString
alignToString(const Qt::Alignment &align)
{
  if      (align ==  Qt::AlignTop                     ) return "n";
  else if (align == (Qt::AlignTop    | Qt::AlignRight)) return "ne";
  else if (align ==                    Qt::AlignRight ) return "e";
  else if (align == (Qt::AlignBottom | Qt::AlignRight)) return "se";
  else if (align ==  Qt::AlignBottom                  ) return "s";
  else if (align == (Qt::AlignBottom | Qt::AlignLeft )) return "sw";
  else if (align ==                    Qt::AlignLeft  ) return "w";
  else if (align == (Qt::AlignTop    | Qt::AlignLeft )) return "nw";
  else if (align ==  Qt::AlignCenter                  ) return "center";
  return "nw";
}

bool
variantToCapStyle(CTclApp *app, const QVariant &var, Qt::PenCapStyle &capStyle)
{
  auto value = variantToString(app, var);

  capStyle = Qt::FlatCap;

  if      (value == "round" ) capStyle = Qt::RoundCap;
  else if (value == "square") capStyle = Qt::SquareCap;
  else if (value == "flat"  ) capStyle = Qt::FlatCap;
  else return false;

  return true;
}

QString
capStyleToString(const Qt::PenCapStyle &capStyle)
{
  if      (capStyle == Qt::RoundCap ) return "round";
  else if (capStyle == Qt::SquareCap) return "square";
  else if (capStyle == Qt::FlatCap  ) return "flat";

  return "none";
}

bool
variantToFillRule(CTclApp *app, const QVariant &var, Qt::FillRule &fillRule)
{
  auto value = variantToString(app, var);

  fillRule = Qt::OddEvenFill;

  if      (value == "evenodd") fillRule = Qt::OddEvenFill;
  else if (value == "nonzero") fillRule = Qt::WindingFill;
  else return false;

  return true;
}

QString fillRuleToString(const Qt::FillRule &fillRule)
{
  if      (fillRule == Qt::OddEvenFill) return "evenodd";
  else if (fillRule == Qt::WindingFill) return "nonzero";

  return "none";
}

bool
uniqueMatch(const std::vector<QString> &values, const QString &str, QString &match, int &nmatch)
{
  std::set<QString> partialValues;

  for (const auto &value : values) {
    if (value == str) {
      match  = value;
      nmatch = 1;
      return true;
    }

    if (value.indexOf(str) == 0)
      partialValues.insert(value);
  }

  nmatch = partialValues.size();

  if (nmatch != 1)
    return false;

  match = *partialValues.begin();

  return true;
}

bool
variantToInt(CTclApp *app, const QVariant &var, long &i)
{
  if      (var.type() == QVariant::Int) {
    i = var.toInt();
  }
  else if (var.type() == QVariant::LongLong) {
    i = var.toLongLong();
  }
  else if (var.type() == QVariant::Double) {
    i = long(var.toDouble());
  }
  else {
    auto str = variantToString(app, var);

    return stringToInt(str, i);
  }

  return true;
}

bool
variantToReal(CTclApp *app, const QVariant &var, double &r)
{
  if      (var.type() == QVariant::Int) {
    r = double(var.toInt());
  }
  else if (var.type() == QVariant::LongLong) {
    r = double(var.toLongLong());
  }
  else if (var.type() == QVariant::Double) {
    r = var.toDouble();
  }
  else {
    auto str = variantToString(app, var);

    return stringToReal(str, r);
  }

  return true;
}

bool
variantToBool(CTclApp *app, const QVariant &var, bool &b)
{
  auto str = variantToString(app, var);

  return stringToBool(str, b);
}

QString
variantToString(CTclApp *app, const QVariant &var, bool quote)
{
  if (! var.isValid())
    return "";

  QString str;

  if      (var.type() == QVariant::String) {
    str = var.toString();
  }
  else if (var.type() == QVariant::Double) {
    str = QString::number(var.toDouble());
  }
  else if (var.type() == QVariant::Int) {
    str = QString::number(var.toInt());
  }
  else if (var.type() == QVariant::LongLong) {
    str = QString::number(var.toLongLong());
  }
  else if (var.type() == QVariant::Bool) {
    str = (var.toBool() ? "1" : "0");
  }
  else if (var.type() == QVariant::List) {
    auto vars = var.toList();

    std::vector<QString> strs;

    for (int i = 0; i < vars.length(); ++i) {
      auto str1 = variantToString(app, vars[i], quote);

      strs.push_back(str1);
    }

    str = app->mergeList(strs);
  }
  else if (var.canConvert(QVariant::String)) {
    str = var.toString();
  }
  else {
    assert(false);
  }

  return str;
}

QColor
grayColor(const QColor &c)
{
  auto gray = qGray(c.rgba());
  return QColor(gray, gray, gray);
}

bool
stringToIndex(CTkApp *app, const QVariant &var, int &ind)
{
  auto str = app->variantToString(var);

  CQStrParse parse(str);

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

bool
stringToLineChar(const QString &str, int &lineNum, int &charNum)
{
  CQStrParse parse(str);

  parse.skipSpace();

  if (! parse.readInteger(&lineNum))
    return false;

  if (parse.isChar('.')) {
    parse.skipChar();

    if (parse.isString("end")) {
      charNum = CTkAppTextInd::END;
    }
    else {
      if (! parse.readInteger(&charNum))
        return false;
    }
  }
  else {
    charNum = CTkAppTextInd::END;
  }

  return true;
}

bool
stringToTextInd(CTkApp *app, const QVariant &var, CTkAppTextInd &ind)
{
  auto str = app->variantToString(var);

  if (str == "end") {
    ind = CTkAppTextInd::end();
    return true;
  }

  int lineNum { -1 };
  int charNum { -1 };

  if (! stringToLineChar(str, lineNum, charNum))
    return false;

  ind = CTkAppTextInd(lineNum, charNum);

  return true;
}

bool
variantToRelief(CTkApp *app, const QVariant &var, CTkAppWidgetRelief &relief)
{
  static auto names = std::vector<QString>({
    "flat", "groove", "raised", "ridge", "solid", "sunken"});

  auto str = variantToString(app, var);

  QString match;
  int     nmatch;
  if (! uniqueMatch(names, str, match, nmatch))
    return false;

  if      (match == "raised") relief = CTkAppWidgetRelief::RAISED;
  else if (match == "sunken") relief = CTkAppWidgetRelief::SUNKEN;
  else if (match == "flat"  ) relief = CTkAppWidgetRelief::FLAT;
  else if (match == "ridge" ) relief = CTkAppWidgetRelief::RIDGE;
  else if (match == "solid" ) relief = CTkAppWidgetRelief::SOLID;
  else if (match == "groove") relief = CTkAppWidgetRelief::GROOVE;
  else return false;

  return true;
}

bool
variantToState(CTkApp *app, const QVariant &var, CTkAppWidgetState &state)
{
  static auto names = std::vector<QString>({
    "active", "disabled", "hidden", "normal"});

  auto str = variantToString(app, var);

  QString match;
  int     nmatch;
  if (! uniqueMatch(names, str, match, nmatch))
    return false;

  if      (match == "active"  ) state = CTkAppWidgetState::ACTIVE;
  else if (match == "disabled") state = CTkAppWidgetState::DISABLED;
  else if (match == "hidden"  ) state = CTkAppWidgetState::HIDDEN;
  else if (match == "normal"  ) state = CTkAppWidgetState::NORMAL;
  else return false;

  return true;
}

// set relief
void
setFrameRelief(QWidget *w, const CTkAppWidgetRelief &relief)
{
  auto *frame = qobject_cast<QFrame *>(w);
  auto *edit  = qobject_cast<QLineEdit *>(w);
  auto *text  = qobject_cast<QTextEdit *>(w);

  if      (frame) {
    if      (relief == CTkAppWidgetRelief::RAISED) {
      frame->setFrameShadow(QFrame::Raised);
      frame->setFrameShape (QFrame::Panel);
    }
    else if (relief == CTkAppWidgetRelief::SUNKEN) {
      frame->setFrameShadow(QFrame::Sunken);
      frame->setFrameShape (QFrame::Panel);
    }
    else if (relief == CTkAppWidgetRelief::FLAT) {
      frame->setFrameShadow(QFrame::Plain);
      frame->setFrameShape (QFrame::NoFrame);
    }
    else if (relief == CTkAppWidgetRelief::RIDGE) {
      frame->setFrameShadow(QFrame::Raised);
      frame->setFrameShape (QFrame::Box);
    }
    else if (relief == CTkAppWidgetRelief::SOLID) {
      frame->setFrameShadow(QFrame::Plain);
      frame->setFrameShape (QFrame::Panel);
    }
    else if (relief == CTkAppWidgetRelief::GROOVE) {
      frame->setFrameShadow(QFrame::Sunken);
      frame->setFrameShape (QFrame::Box);
    }
  }
  else if (edit) {
    edit->setFrame(true);
  }
  else if (text) {
    if      (relief == CTkAppWidgetRelief::RAISED) {
      text->setFrameShadow(QFrame::Raised);
      text->setFrameShape (QFrame::Panel);
    }
    else if (relief == CTkAppWidgetRelief::SUNKEN) {
      text->setFrameShadow(QFrame::Sunken);
      text->setFrameShape (QFrame::Panel);
    }
    else if (relief == CTkAppWidgetRelief::FLAT) {
      text->setFrameShadow(QFrame::Plain);
      text->setFrameShape (QFrame::NoFrame);
    }
    else if (relief == CTkAppWidgetRelief::RIDGE) {
      text->setFrameShadow(QFrame::Raised);
      text->setFrameShape (QFrame::Box);
    }
    else if (relief == CTkAppWidgetRelief::SOLID) {
      text->setFrameShadow(QFrame::Plain);
      text->setFrameShape (QFrame::Panel);
    }
    else if (relief == CTkAppWidgetRelief::GROOVE) {
      text->setFrameShadow(QFrame::Sunken);
      text->setFrameShape (QFrame::Box);
    }
  }
}

bool
variantToJustify(const QVariant &var, Qt::Alignment &align)
{
  auto str = var.toString();

  align = Qt::AlignCenter;

  if      (str == "left"  ) align = Qt::AlignLeft;
  else if (str == "right" ) align = Qt::AlignRight;
  else if (str == "center") align = Qt::AlignHCenter;
  else return false;

  return true;
}

QString
underlineLabel(const QString &label, long pos)
{
  auto label1 = label;

  if (pos >= 0 && pos < long(label1.size()))
    label1 = label1.mid(0, pos) + "&" + label1.mid(pos);

  return label1;
}

bool
variantToCompound(CTkApp *app, const QVariant &var, CTkAppCompoundType &type)
{
  static auto optionNames = std::vector<QString>({
    "bottom", "center", "left", "none", "right", "top"});

  auto str = var.toString();

  QString option;
  if (! app->lookupOptionName(optionNames, str, option))
    return false;

  type = CTkAppCompoundType::NONE;

  // none, bottom, top, left, right, or center.
  if      (option == "bottom") type = CTkAppCompoundType::BOTTOM;
  else if (option == "center") type = CTkAppCompoundType::CENTER;
  else if (option == "left"  ) type = CTkAppCompoundType::LEFT;
  else if (option == "none"  ) type = CTkAppCompoundType::NONE;
  else if (option == "right" ) type = CTkAppCompoundType::RIGHT;
  else if (option == "top"   ) type = CTkAppCompoundType::TOP;
  else return false;

  return true;
}

bool
stringToPath(const QString &str, QPainterPath &path)
{
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

bool
stringToMatrix(CTkApp *tk, const QString &str, QTransform &t)
{
  std::vector<QPointF> points;

  std::vector<QString> strs;
  if (! tk->splitList(str, strs) || strs.size() != 3)
    return false;

  for (uint j = 0; j < 3; ++j) {
    std::vector<QString> strs1;
    if (! tk->splitList(strs[j], strs1) || strs1.size() != 2)
      return false;

    double x, y;
    if (! CTkAppUtil::stringToReal(strs1[0], x) ||
        ! CTkAppUtil::stringToReal(strs1[1], y))
      return false;

    points.push_back(QPointF(x, y));
  }

  CMatrix2D m(points[0].x(), points[1].x(),
              points[0].y(), points[1].y(),
              points[2].x(), points[2].y());

  t = tk->toQTransform(m);

  return true;
}

QString
formatStringInWidth(const QString &text, int wrapWidth, const QFont &font)
{
  assert(wrapWidth > 0);

  QFontMetricsF fm(font);

  auto tw = fm.horizontalAdvance(text);

  if (tw < wrapWidth)
    return text;

  QStringList lines;

  QString line;
  int     lineWidth = 0;

  int pos = 0;
  int len = text.length();

  //---

  auto isSplitter = [&]() {
    auto c = text[pos];

    // Space is splitter
    if (c.isSpace())
      return true;

    auto c1 = c.toLatin1();

    // Punctuation Characters followed by an End of String or a Space are splitters
    if (ispunct(c1) && (pos == len - 1 || text[pos + 1].isSpace()))
      return true;

    // Comma, Colon, Semi Colon, Question Mark and Exclamation Mark followed by an
    // End of String or a non-Punctuation Character is a Splitter

    if (strchr(",:;?!", c1) != nullptr &&
        (pos == len - 1 || ! ispunct(text[pos + 1].toLatin1())))
      return true;

    // Any other characters are not Splitters
    return false;
  };

  //---

  while (pos < len) {
    // get next word
    QString word;

    word += text[pos++];

    while (pos < len) {
      if (isSplitter())
        break;

      word += text[pos++];
    }

    int wordWidth = fm.horizontalAdvance(word);

    if (lineWidth > 0 && lineWidth + wordWidth > wrapWidth) {
      lines += line;

      line      = word;
      lineWidth = wordWidth;
    }
    else {
      line      += word;
      lineWidth += wordWidth;

      while (pos < len && isSplitter()) {
        auto splitChar = text.mid(pos, 1);

        int tw1 = fm.horizontalAdvance(splitChar);
        if (lineWidth + tw1 > wrapWidth)
          break;

        line      += splitChar;
        lineWidth += tw1;

        ++pos;
      }
    }
  }

  if (line != "")
    lines += line;

  return lines.join("\n");
}

}
