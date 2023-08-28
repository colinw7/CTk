#include <CTkAppUtil.h>

#include <CQStrParse.h>
#include <CScreenUnits.h>

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
stringToQColor(const QString &str, QColor &c)
{
  c = QColor(str);

  return true;
}

bool
stringToQFont(const QString &str, QFont &f)
{
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
stringToDistance(const QString &str, double &r)
{
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
stringToDistanceI(const QString &str, int &i)
{
  double r;
  if (! stringToDistance(str, r))
    return false;
  i = int(r);
  return true;
}

bool
stringToOrient(const QString &str, Qt::Orientation &orient)
{
  static auto names = std::vector<QString>({ "horizontal", "vertical" });

  QString match;
  if (! uniqueMatch(names, str, match))
    return false;

  orient = Qt::Horizontal;

  if (match == "horizontal") orient = Qt::Horizontal;
  if (match == "vertical"  ) orient = Qt::Vertical;

  return true;
}

bool
stringToAlign(const QString &value, Qt::Alignment &align)
{
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
  else { std::cerr << "Invalid align string '" << value.toStdString() << "'\n"; return false; }

  return true;
}

QString
alignToString(Qt::Alignment &align)
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

}
