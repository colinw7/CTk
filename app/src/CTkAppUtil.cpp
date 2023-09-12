#include <CTkAppUtil.h>
#include <CTclApp.h>

#include <CQStrParse.h>
#include <CScreenUnits.h>

#include <QVariant>
#include <set>

namespace CTkAppUtil {

bool
stringToInt(const QString &str, long &i)
{
  try {
    i = std::stol(str.toStdString());
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
    r = std::stod(str.toStdString());
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

  c = QColor(str);

  return true;
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
    else
      std::cerr << "Unhandled style '" << style.toStdString() << "'\n";
  }

  //---

  //std::cerr << "\n";

  return true;
}

bool
variantToDistanceI(CTclApp *app, const QVariant &var, long &i)
{
  double r;
  if (! variantToDistance(app, var, r))
    return false;
  i = long(r);
  return true;
}

bool
variantToDistance(CTclApp *app, const QVariant &var, double &r)
{
  auto str = variantToString(app, var);

  CQStrParse parse(str);

  parse.skipSpace();

  double r1;

  if (! parse.readReal(&r1))
    return false;

  parse.skipSpace();

  if (parse.eof()) {
    r = r1;
    return true;
  }

  auto c = parse.readChar();

  double f = 1.0;

  if      (c == 'c') // centimeters
    f = 1.0/2.54;
  else if (c == 'i') // inches
    f = 1.0;
  else if (c == 'm') // millimeters
    f = 1.0/25.4;
  else if (c == 'p') // points
    f = 1.0/72.0;
  else
    return false;

  auto dpi = CScreenUnitsMgrInst->dpi();

  r = r1*dpi*f;

  return true;
}

bool
variantToOrient(CTclApp *app, const QVariant &var, Qt::Orientation &orient)
{
  static auto names = std::vector<QString>({ "horizontal", "vertical" });

  auto str = variantToString(app, var);

  QString match;
  if (! uniqueMatch(names, str, match))
    return false;

  orient = Qt::Horizontal;

  if (match == "horizontal") orient = Qt::Horizontal;
  if (match == "vertical"  ) orient = Qt::Vertical;

  return true;
}

bool
variantToAlign(CTclApp *app, const QVariant &var, Qt::Alignment &align, bool quiet)
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
  else {
    if (! quiet)
      std::cerr << "Invalid align string '" << value.toStdString() << "'\n";
    return false;
  }

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
uniqueMatch(const std::vector<QString> &values, const QString &str, QString &match)
{
  std::set<QString> partialValues;

  for (const auto &value : values) {
    if (value == str) {
      match = value;
      return true;
    }

    if (value.indexOf(str) == 0)
      partialValues.insert(value);
  }

  if (partialValues.size() != 1)
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

}
