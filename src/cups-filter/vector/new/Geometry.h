#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <stdint.h>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <deque>

namespace bg = boost::geometry;

typedef bg::point<int32_t, 2, bg::cs::cartesian> Point;
typedef bg::segment<const Point> Segment;
typedef bg::linestring<Point > Linestring;
typedef bg::box<Point> Box;

inline std::ostream& operator<<(std::ostream &os, const Point &p) {
  os << "{" << p.get<0>() << "," << p.get<1>() << "}";
  return os;
}

inline std::ostream& operator<<(std::ostream &os, const Segment& segment) {
  os << "{" << segment.first << "," << segment.second << "}";
  return os;
}

inline std::ostream& operator<<(std::ostream &os, const Linestring& ls) {
//FIXME
  return os;
}

inline std::ostream& operator<<(std::ostream &os, const Linestring* ls) {
//FIXME
  return os;
}

inline std::ostream& operator<<(std::ostream &os, const Box& box) {
//FIXME
  return os;
}
#endif /* GEOMETRY_H_ */
