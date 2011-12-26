#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <deque>
#include <limits>
#include <list>
#include <assert.h>
#include <boost/math/special_functions.hpp>
#include <boost/tuple/tuple.hpp>

static const float CC_PI = atan2(0,-1);

struct OpParams {
  int power;
  int speed;
  int frequency;

  OpParams(): power(0), speed(0), frequency(0) {};
  OpParams(int power, int speed, int frequency = 5000): power(power), speed(speed), frequency(frequency) {}
};

class Point {
public:
  int32_t x;
  int32_t y;

  Point(): x(0), y(0) {}
  Point(int32_t x, int32_t y): x(x), y(y) {}

  const int32_t &operator[](size_t idx) const {
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
    int32_t x_diff = this->x - other.x;
    int32_t y_diff = this->y - other.y;
    return (* new Point(x_diff, y_diff));
  }

  void operator=(const Point&  other) {
    this->x = other.x;
    this->y = other.y;
  }

  double distance(const Point&  other) const {
    return hypot(double(this->x - other.x), double(this->y - other.y));
  }
};

class Box {
public:
  const Point min_corner;
  const Point max_corner;

  Box(const Point&  min_corner, const Point&  max_corner) : min_corner(min_corner) , max_corner(max_corner) {}
};

class Sphere {
public:
  const Point center;
  const uint32_t radius;

  Sphere(const Point&  center, const uint32_t& radius) : center(center) , radius(radius) {}
};

// explicit mixed sign comparison to prevent warnings
inline bool greater_than(int32_t s, uint32_t us) {
  return s > (int32_t)us;
}

// explicit mixed sign comparison to prevent warnings
inline bool lesser_than(int32_t s, uint32_t us) {
  return s < (int32_t)us;
}

#endif /* GEOMETRY_H_ */
