#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <iostream>
#include <stdint.h>
#include <deque>
#include <list>
#include <assert.h>
#include <kdtree++/kdtree.hpp>
#include <boost/math/special_functions.hpp>

struct CutSettings {
  int power;
  int speed;
  int frequency;

  CutSettings(): power(0), speed(0), frequency(0) {};
  CutSettings(int power, int speed, int frequency = 5000): power(power), speed(speed), frequency(frequency) {}
};

class Point {
public:
  int32_t x;
  int32_t y;

  Point(): x(0), y(0) {}
  Point(const Point& other): x(other.x), y(other.y) {}
  Point(int32_t x, int32_t y): x(x), y(y) {}

  const int32_t &operator[](size_t idx) const {
    assert(idx < 2);
    if (idx == 0)
      return x;
    else
      return y;
  }

  const bool operator<(const Point& other) const {
      return (! (this->x > other.x)) || (this->y < other.y);
  }

  const bool operator==(const Point& other) const {
    return this->x == other.x && this->y == other.y;
  }

  const bool operator!=(const Point& other) const {
    return this->x != other.x || this->y != other.y;
  }

  Point& operator-(const Point& other) const {
    int32_t x_diff = this->x - other.x;
    int32_t y_diff = this->y - other.y;
    return (* new Point(x_diff, y_diff));
  }

  void operator=(const Point& other) {
    this->x = other.x;
    this->y = other.y;
  }

  float distance(const Point& other) const {
    return hypot(float(this->x - other.x), float(this->y - other.y));
  }
};

class Box {
public:
  const Point min_corner;
  const Point max_corner;

  Box(const Point& min_corner, const Point& max_corner) : min_corner(min_corner) , max_corner(max_corner) {}
};

class Sphere {
public:
  const Point center;
  const uint32_t radius;

  Sphere(const Point& center, uint32_t radius) : center(center) , radius(radius) {}
};

class Segment {
public:
  const Point& first;
  const Point& second;
  CutSettings& settings;

  Segment(const Point& first, const Point& second, CutSettings& settings) : first(first), second(second), settings(settings), box(NULL), sphere(NULL) {
  }

  const Point &operator[](size_t idx) const {
    assert(idx < 2);
    if (idx == 0)
      return first;
    else
      return second;
  }

  const bool operator==(const Segment& other) const {
    return this->first == other.first && this->second == other.second;
  }

  const Box& getBox() {
    if(this->box == NULL) {
      if(first < second)
        this->box = new Box(first, second);
      else
        this->box = new Box(second, first);
    }

    return *this->box;
  }

  const Sphere& getSphere() {
    if(this->sphere == NULL) {
      const Box& box = this->getBox();
      Point& diff = box.max_corner - box.min_corner;
      Point center = (* new Point(box.min_corner.x + (diff.x / 2), box.min_corner.y + (diff.y / 2)));
      uint32_t radius = boost::math::hypot((diff.x / 2), (diff.y / 2));
      this->sphere = new Sphere(center, radius);
    }

    return *this->sphere;
  }
private:
  Box* box;
  Sphere* sphere;
};


typedef std::deque<const Segment*> SegmentString;
typedef std::list<Segment*> SegmentList;

enum intersection_result { ALIGN_NONE, ALIGN_INTERSECT, ALIGN_COINCIDENCE, ALIGN_PARALLEL };

/*!
  Calculates alignment of two segments to each other and sets the supplied point to the resulting point if they intersect
  Tip intersections are reported as ordinary intersections.
*/
inline intersection_result intersects(const Segment& s1, const Segment &s2, Point& intersection)
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
    // Get the intersection LSPoint.
    intersection.x = s1[0][0] + ua * (s1[1][0] - s1[0][0]);
    intersection.y = s1[0][1] + ua * (s1[1][1] - s1[0][1]);

    return ALIGN_INTERSECT;
  }

  return ALIGN_NONE; //NOT_INTERSECTING;
}

inline std::ostream& operator<<(std::ostream &os, const Point &p) {
  os << "{" << p.x << "," << p.y << "}";
  return os;
}

inline std::ostream& operator<<(std::ostream &os, const Segment& segment) {
  os << "{" << segment.first << "," << segment.second << "}";
  return os;
}

struct SpatialItem {
  SegmentList::iterator owner;
  Point end_point;

  SpatialItem(const SegmentList::iterator owner, const Point& end_point) : owner(owner), end_point(end_point) {}

  const bool operator==(const SpatialItem& other) const {
    return *this->owner == *other.owner;
  }
};

inline int32_t spatial_item_ac( SpatialItem item, int k ) { return item.end_point[k]; }

typedef KDTree::KDTree<2, SpatialItem, std::pointer_to_binary_function<SpatialItem,int,int32_t> > SpatialTree;

#endif /* GEOMETRY_H_ */
