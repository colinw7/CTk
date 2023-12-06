#ifndef CTkAppUtil_H
#define CTkAppUtil_H

#include <CTkAppTypes.h>

#include <QPainterPath>
#include <QColor>
#include <QFont>

#include <string>

class CTkApp;
class CTclApp;

struct CTkAppTextInd {
  enum { END = INT_MAX };

  int lineNum { -1 };
  int charNum { -1 };

  CTkAppTextInd() { }

  CTkAppTextInd(int l, int c) :
    lineNum(l), charNum(c) {
  }

  static CTkAppTextInd end() {
    return CTkAppTextInd(END, END);
  }

  static int cmp(const CTkAppTextInd &ind1, const CTkAppTextInd &ind2) {
    if (ind1.lineNum > ind2.lineNum) return  1;
    if (ind1.lineNum < ind2.lineNum) return -1;
    if (ind1.charNum > ind2.charNum) return  1;
    if (ind1.charNum < ind2.charNum) return -1;
    return 0;
  }

  QString toString() const {
    return QString::number(lineNum) + "." + QString::number(charNum);
  }

  friend std::ostream &operator<<(std::ostream &os, const CTkAppTextInd &ind) {
    os << ind.toString().toStdString();
    return os;
  }
};

//---

enum class CTkAppUnits {
  NONE,
  CM,  // Centimeters
  MM,  // Millimeters
  IN,  // Inches
  PT   // Points
};

struct CTkAppDistance {
  using Units = CTkAppUnits;

  CTkAppDistance() { }

  Units  units  { Units::NONE };
  double r      { 0.0 };
  long   i      { 0 };
  bool   isReal { true };
  double f      { 1.0 };
  double rvalue { 0.0 };
  long   ivalue { 0 };
};

//---

namespace CTkAppUtil {

using Distance = CTkAppDistance;

enum { END_INDEX=INT_MAX };

bool stringToInt(const QString &str, long &i);
bool stringToReal(const QString &str, double &r);
bool stringToBool(const QString &str, bool &b);

bool uniqueMatch(const std::vector<QString> &values, const QString &str,
                 QString &match, int &nmatch);

bool variantToQColor(CTclApp *app, const QVariant &var, QColor &c);
bool variantToQFont (CTclApp *app, const QVariant &var, QFont  &f);

bool variantToOrient(CTclApp *app, const QVariant &var, Qt::Orientation &orient);

bool variantToAlign(CTclApp *app, const QVariant &var, Qt::Alignment &align);
QString alignToString(const Qt::Alignment &align);

bool variantToCapStyle(CTclApp *app, const QVariant &var, Qt::PenCapStyle &capStyle);
QString capStyleToString(const Qt::PenCapStyle &capStyle);

bool variantToFillRule(CTclApp *app, const QVariant &var, Qt::FillRule &fillRule);
QString fillRuleToString(const Qt::FillRule &fillRule);

bool variantToDistance (CTclApp *app, const QVariant &var, Distance &d);
bool variantToDistanceI(CTclApp *app, const QVariant &var, Distance &d);

bool variantToInt (CTclApp *app, const QVariant &var, long &i);
bool variantToReal(CTclApp *app, const QVariant &var, double &r);
bool variantToBool(CTclApp *app, const QVariant &var, bool &b);

QString variantToString(CTclApp *app, const QVariant &var, bool quote=false);

bool stringToIndex(CTkApp *app, const QVariant &var, int &ind);

bool stringToLineChar(const QString &str, int &lineNum, int &charNum);

bool stringToTextInd(CTkApp *app, const QVariant &var, CTkAppTextInd &ind);

bool variantToRelief(CTkApp *app, const QVariant &var, CTkAppWidgetRelief &relief);
void setFrameRelief(QWidget *w, const CTkAppWidgetRelief &relief);

bool variantToState(CTkApp *app, const QVariant &var, CTkAppWidgetState &state);

bool variantToJustify(const QVariant &value, Qt::Alignment &align);

QString underlineLabel(const QString &label, long pos);

bool variantToCompound(CTkApp *app, const QVariant &var, CTkAppCompoundType &type);

bool stringToPath(const QString &str, QPainterPath &path);

bool stringToMatrix(CTkApp *tk, const QString &str, QTransform &t);

QColor grayColor(const QColor &c);

QString formatStringInWidth(const QString &text, int width, const QFont &font);

}

#endif
