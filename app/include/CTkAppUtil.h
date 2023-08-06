#ifndef CTkAppUtil_H
#define CTkAppUtil_H

#include <string>

#include <QColor>
#include <QFont>

namespace CTkAppUtil {

bool stringToInt(const std::string &str, long &i);
bool stringToReal(const std::string &str, double &r);
bool stringToBool(const std::string &str, bool &b);

bool stringToQColor(const std::string &str, QColor &c);
bool stringToQFont(const std::string &str, QFont &f);

bool stringToDistance(const std::string &str, double &r);
bool stringToDistanceI(const std::string &str, int &i);

bool stringToOrient(const std::string &str, Qt::Orientation &orient);

}

#endif
