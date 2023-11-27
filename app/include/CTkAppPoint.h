#ifndef CTkAppPoint_H
#define CTkAppPoint_H

#include <QPointF>

#include <vector>
#include <cmath>

struct CTkAppPoint {
  CTkAppPoint() = default;

  CTkAppPoint(double x1, double y1) :
   x(x1), y(y1) {
  }

  double x { 0.0 };
  double y { 0.0 };

  QPointF qpoint() const { return QPointF(x, y); }

  double distance(double x1, double y1) const {
    return std::hypot(x - x1, y - y1);
  }

  void moveBy(double dx, double dy) {
    x += dx;
    y += dy;
  }

  bool operator==(CTkAppPoint &rhs) const {
    return (x == rhs.x && y == rhs.y);
  }
};

using CTkAppPoints = std::vector<CTkAppPoint>;

#endif
