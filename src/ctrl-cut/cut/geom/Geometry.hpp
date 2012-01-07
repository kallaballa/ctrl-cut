#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <deque>
#include <limits>
#include <vector>
#include <list>
#include <assert.h>
#include <boost/math/special_functions.hpp>
#include <boost/tuple/tuple.hpp>

#include <iostream>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/concepts/check.hpp>

static const float CC_PI = atan2(0,-1);

#include <iostream>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/box.hpp>
#include <boost/geometry/geometries/register/segment.hpp>

typedef int32_t Coord_t;
class Point {
public:
  Coord_t x;
  Coord_t y;

  Point(): x(0), y(0) {}
  Point(Coord_t x, Coord_t y): x(x), y(y) {}

  Coord_t &operator[](size_t idx) {
    assert(idx < 2);
    if (idx == 0)
      return x;
    else
      return y;
  }

  const Coord_t &operator[](size_t idx) const {
    assert(idx < 2);
    if (idx == 0)
      return x;
    else
      return y;
  }

  bool operator<(const Point&  other) const {
      return  (this->x < other.x) || ((this->x == other.x) && (this->y < other.y));
  }

  bool operator==(const Point&  other) const {
    return this->x == other.x && this->y == other.y;
  }

  bool operator!=(const Point&  other) const {
    return this->x != other.x || this->y != other.y;
  }

  Point& operator-(const Point&  other) const {
    Coord_t x_diff = this->x - other.x;
    Coord_t y_diff = this->y - other.y;
    return (* new Point(x_diff, y_diff));
  }

  void operator=(const Point&  other) {
    this->x = other.x;
    this->y = other.y;
  }

  double distance(const Point&  other) const {
    return hypot(double(this->x - other.x), double(this->y - other.y));
  }

  Point make() const {
    return Point();
  }
};

BOOST_GEOMETRY_REGISTER_POINT_2D(Point, Coord_t, cs::cartesian, x, y)

class Segment {
public:
  Point first;
  Point second;

  Segment() {};
  Segment(const Segment& other) : first(other.first), second(other.second)  {};
  Segment(Point first, Point second) : first(first), second(second)  {}

  const Point& operator[](size_t idx) const {
    assert(idx < 2);
    if (idx == 0)
      return this->first;
    else
      return this->second;
  }

  Point& operator[](size_t idx) {
    assert(idx < 2);
    if (idx == 0)
      return this->first;
    else
      return this->second;
  }

  const Point& back() {
    return this->first;
  };

  const Point& front() {
    return this->second;
  };

  bool operator<(const Segment& other) const {
      return  (this->first < other.first) || ((this->first == other.first) && (this->second < other.second));
  }

  bool operator==(const Segment& other) const {
    return this->first == other.first && this->second == other.second;
  }

  bool operator!=(const Segment& other) const {
    return ! (*this == other);
  }

  Segment swap() const {
    return Segment(second,first);
  }

  Segment make() const {
    return Segment();
  }

  /*!
    Calculates the distance from the Point to the infinite segment
  */
  float distance(const Point& p) const;
  /*!
    Returns angle to the positive Y axis
  */
  float getSlope(bool invert = false) const;
};

BOOST_GEOMETRY_REGISTER_SEGMENT(Segment, Point, first, second);


enum intersection_result { ALIGN_NONE, ALIGN_INTERSECT, ALIGN_COINCIDENCE, ALIGN_PARALLEL };

/*!
  Calculates alignment of two segments to each other and sets the supplied point to the resulting point if they intersect
  Tip intersections are reported as ordinary intersections.
*/
inline intersection_result intersects(const Segment& s1, const Segment&s2, Point& intersection)
{
  float denom =
    ((s2[1][1] - s2[0][1]) * (s1[1][0] - s1[0][0])) -
    ((s2[1][0] - s2[0][0]) * (s1[1][1] - s1[0][1]));

  float nume_a =
    ((s2[1][0] - s2[0][0]) * (s1[0][1] - s2[0][1])) -
    ((s2[1][1] - s2[0][1]) * (s1[0][0] - s2[0][0]));

  float nume_b =
    ((s1[1][0] - s1[0][0]) * (s1[0][1] - s2[0][1])) -
    ((s1[1][1] - s1[0][1]) * (s1[0][0] - s2[0][0]));

  if (denom == 0.0f) {
    if (nume_a == 0.0f && nume_b == 0.0f)
      return ALIGN_COINCIDENCE;
    else
      return ALIGN_PARALLEL;
  }

  float ua = nume_a / denom;
  float ub = nume_b / denom;

  if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f) {
    intersection[0] = (s1[0][0] + ua * (s1[1][0] - s1[0][0]));
    intersection[1] = (s1[0][1] + ua * (s1[1][1] - s1[0][1]));
    return ALIGN_INTERSECT;
  }

  return ALIGN_NONE;
}
inline std::ostream& operator<<(std::ostream &os, const Point& p)  {
  os << "<point x=\"" << p.x << "\" y=\"" << p.y << "\" key=\"" << p.x << "/" << p.y << "\" />";
  return os;
}

inline std::istream& operator>>(std::istream &is, Point& p)  {
  Coord_t x,y;
  char comma;
  is >> x;
  is >> comma;
  is >> y;

  p = Point(x,y);
  return is;
}

inline std::ostream& operator<<(std::ostream &os, const Segment& segment) {
  os << "<segment>" << std::endl;
  os << "  " << segment.first << std::endl;
  os << "  " << segment.second << std::endl;
  os << "</segment>";
  return os;
}

class Box {
public:
  Point min_corner;
  Point max_corner;
  Box(const Segment& seg) {
    if(seg.first < seg.second) {
      min_corner = seg.first;
      max_corner = seg.second;
    }
    else {
      min_corner = seg.second;
      max_corner = seg.first;
    }
  }

  Box(const Point&  min_corner, const Point&  max_corner) : min_corner(min_corner) , max_corner(max_corner) {}

  Coord_t width() {
    return max_corner[0] - min_corner[0];
  }

  Coord_t height() {
    return max_corner[1] - min_corner[1];
  }
};

BOOST_GEOMETRY_REGISTER_BOX(Box, Point, min_corner, max_corner);


class Sphere {
public:
  Point center;
  uint32_t radius;
  template<typename Tgeom>
  Sphere(const Tgeom& seg) {
    Box box(seg);

    Point& diff = box.max_corner - box.min_corner;
    this->center = Point(box.min_corner.x + (diff.x / 2), box.min_corner.y + (diff.y / 2));
    this->radius = boost::math::hypot((diff.x / 2), (diff.y / 2));
  }

  Sphere(const Point&  center, const uint32_t& radius) : center(center) , radius(radius) {}
};

// explicit mixed sign comparison to prevent warnings
inline bool greater_than(Coord_t s, uint32_t us) {
  return s > (Coord_t)us;
}

// explicit mixed sign comparison to prevent warnings
inline bool lesser_than(Coord_t s, uint32_t us) {
  return s < (Coord_t)us;
}

template<
  typename Tgeom,
  template<typename,typename> class Tcontainer = std::vector,
  template<typename> class Tallocator = std::allocator
>
struct GeometryGroup : Tcontainer<Tgeom, Tallocator<Tgeom> >
{};

#endif /* GEOMETRY_H_ */
