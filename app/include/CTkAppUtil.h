#ifndef CTkAppUtil_H
#define CTkAppUtil_H

#include <string>

#include <QColor>
#include <QFont>

namespace CTkAppUtil {

bool stringToInt(const QString &str, long &i);
bool stringToReal(const QString &str, double &r);
bool stringToBool(const QString &str, bool &b);

bool stringToQColor(const QString &str, QColor &c);
bool stringToQFont(const QString &str, QFont &f);

bool stringToDistance(const QString &str, double &r);
bool stringToDistanceI(const QString &str, int &i);

bool stringToOrient(const QString &str, Qt::Orientation &orient);

bool stringToAlign(const QString &value, Qt::Alignment &align, bool quiet=false);
QString alignToString(const Qt::Alignment &align);

bool stringToCapStyle(const QString &value, Qt::PenCapStyle &capStyle);
QString capStyleToString(const Qt::PenCapStyle &capStyle);

bool stringToFillRule(const QString &value, Qt::FillRule &fillRule);
QString fillRuleToString(const Qt::FillRule &fillRule);

bool uniqueMatch(const std::vector<QString> &values, const QString &str, QString &match);

}

#endif
