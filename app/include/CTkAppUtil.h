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

namespace CTkAppUtil {

enum { END_INDEX=INT_MAX };

bool stringToInt(const QString &str, long &i);
bool stringToReal(const QString &str, double &r);
bool stringToBool(const QString &str, bool &b);

bool uniqueMatch(const std::vector<QString> &values, const QString &str, QString &match);

bool variantToQColor(CTclApp *app, const QVariant &var, QColor &c);
bool variantToQFont (CTclApp *app, const QVariant &var, QFont  &f);

bool variantToOrient(CTclApp *app, const QVariant &var, Qt::Orientation &orient);

bool variantToAlign(CTclApp *app, const QVariant &var, Qt::Alignment &align, bool quiet=false);
QString alignToString(const Qt::Alignment &align);

bool variantToCapStyle(CTclApp *app, const QVariant &var, Qt::PenCapStyle &capStyle);
QString capStyleToString(const Qt::PenCapStyle &capStyle);

bool variantToFillRule(CTclApp *app, const QVariant &var, Qt::FillRule &fillRule);
QString fillRuleToString(const Qt::FillRule &fillRule);

bool variantToDistance (CTclApp *app, const QVariant &var, double &r);
bool variantToDistanceI(CTclApp *app, const QVariant &var, long &i);

bool variantToInt (CTclApp *app, const QVariant &var, long &i);
bool variantToReal(CTclApp *app, const QVariant &var, double &r);
bool variantToBool(CTclApp *app, const QVariant &var, bool &b);

QString variantToString(CTclApp *app, const QVariant &var, bool quote=false);

bool stringToIndex(CTkApp *app, const QVariant &var, int &ind);

bool stringToLineChar(const QString &str, int &lineNum, int &charNum);

bool stringToTextInd(CTkApp *app, const QVariant &var, CTkAppTextInd &ind);

bool variantToRelief(CTkApp *app, const QVariant &var, CTkAppWidgetRelief &relief);
void setFrameRelief(QWidget *w, const CTkAppWidgetRelief &relief);

Qt::Alignment stringToJustify(const QString &value);

QString underlineLabel(const QString &label, long pos);

bool stringToCompound(const QString &value, CTkAppCompoundType &type);

bool stringToPath(const QString &str, QPainterPath &path);

bool stringToMatrix(CTkApp *tk, const QString &str, QTransform &t);

QColor grayColor(const QColor &c);

QString formatStringInWidth(const QString &text, int width, const QFont &font);

}

#endif
