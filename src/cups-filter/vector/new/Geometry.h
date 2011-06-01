#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <stdint.h>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <deque>

namespace bg = boost::geometry;

struct Point {
  Point() {}
  Point(int32_t x, int32_t y): x(x), y(y) {}

  int32_t x;
  int32_t y;
  unsigned long int vertexID;

  bool operator==(const Point& other) const {
    return this->x == other.x && this->y == other.y;
  }
};

BOOST_GEOMETRY_REGISTER_POINT_2D(Point, int32_t, bg::cs::cartesian, x,y)

typedef bg::segment<Point> Segment;
typedef bg::segment<const Point> ConstSegment;
typedef bg::linestring<Point, std::deque > Linestring;
typedef bg::box<Point> Box;

inline std::ostream& operator<<(std::ostream &os, const Point &p) {
  os << "{" << p.x << "," << p.y << "}";
  return os;
}

inline std::ostream& operator<<(std::ostream &os, const ConstSegment& segment) {
  os << "{" << segment.first << "," << segment.second << "}";
  return os;
}

inline std::ostream& operator<<(std::ostream &os, const Linestring& ls) {
//FIXME  os << "{" << segment.first << "," << segment.second << "}";
  return os;
}
#endif /* GEOMETRY_H_ */
