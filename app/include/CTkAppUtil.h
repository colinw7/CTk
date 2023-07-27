#ifndef CTkAppUtil_H
#define CTkAppUtil_H

#include <CScreenUnits.h>
#include <CStrMap.h>

namespace CTkAppUtil {

inline bool stringToInt(const std::string &str, long &i) {
  try {
    i = std::stol(str);
  }
    catch (...) {
    return false;
  }

  return true;
}

inline bool stringToReal(const std::string &str, double &r) {
  try {
    r = std::stod(str);
  }
    catch (...) {
    return false;
  }

  return true;
}

inline bool stringToBool(const std::string &str, bool &b) {
  if      (str == "1" || str == "yes" || str == "true" || str == "y")
    b = true;
  else if (str == "0" || str == "no" || str == "false" || str == "n")
    b = false;
  else
    return false;

  return true;
}

inline QColor stringToQColor(const std::string &str) {
  double r = 0.0, g = 0.0, b = 0.0, a = 0.0;

  if (str != "")
    CRGBName::lookup(str, &r, &g, &b, &a);

  return QColor(255*r, 255*g, 255*b, 255*a);
}

inline QFont stringToQFont(const std::string &str) {
  QFont f;

  CStrParse parse(str);

  parse.skipSpace();

  std::string family;

  if (! parse.readNonSpace(family))
    return f;

  //std::cerr << family << " ";

  f.setFamily(QString::fromStdString(family));

  //---

  parse.skipSpace();

  int  size   = 0;
  bool points = true;

  if (! parse.readInteger(&size))
    return f;

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

  std::vector<std::string> styles;

  while (! parse.eof()) {
    std::string style;

    if (! parse.readNonSpace(style))
      return f;

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
      std::cerr << "Unhandled style '" << style << "'\n";
  }

  //---

  //std::cerr << "\n";

  return f;
}

inline bool stringToDistance(const std::string &str, double &r) {
  CStrParse parse(str);

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

inline bool stringToDistanceI(const std::string &str, int &i) {
  double r;
  if (! stringToDistance(str, r))
    return false;
  i = int(r);
  return true;
}

inline bool stringToOrient(const std::string &str, Qt::Orientation &orient) {
  static CStrMap<Qt::Orientation, int> orientMap(
    "horizontal", Qt::Horizontal,
    "vertical"  , Qt::Vertical  ,
    0);

  return orientMap.map(str, orient);
}

}

#endif
