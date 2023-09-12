#ifndef CTkAppUtil_H
#define CTkAppUtil_H

#include <string>

#include <QColor>
#include <QFont>

class CTclApp;

namespace CTkAppUtil {

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

}

#endif
