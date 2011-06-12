#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <iostream>
#include <stdint.h>
#include <deque>
#include <list>
#include <assert.h>
#include <kdtree++/kdtree.hpp>
#include <boost/math/special_functions.hpp>
#include <boost/tuple/tuple.hpp>

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
      return  (this->x < other.x) || ((this->x == other.x) && (this->y < other.y));
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

  double distance(const Point& other) const {
    return hypot(double(this->x - other.x), double(this->y - other.y));
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
    if(first < second)
      this->box = new Box(first, second);
    else
      this->box = new Box(second, first);

    Point& diff = this->box->max_corner - this->box->min_corner;
    Point center = (* new Point(this->box->min_corner.x + (diff.x / 2), this->box->min_corner.y + (diff.y / 2)));
    uint32_t radius = boost::math::hypot((diff.x / 2), (diff.y / 2));
    this->sphere = new Sphere(center, radius);
  }

  const Point &operator[](size_t idx) const {
    assert(idx < 2);
    if (idx == 0)
      return first;
    else
      return second;
  }

  const bool operator<(const Segment& other) const {
      return  (this->first < other.first) || ((this->first == other.first) && (this->second < other.second));
  }

  const bool operator==(const Segment& other) const {
    return this->first == other.first && this->second == other.second;
  }

  const Box& getBox() const {
    return *this->box;
  }

  const Sphere& getSphere() const {
    return *this->sphere;
  }
private:
  Box* box;
  Sphere* sphere;
};

class SegmentString {
public:
  typedef std::deque<const Segment*> SegmentDeque;
  typedef std::deque<const Point*> PointDeque;
  typedef SegmentDeque::iterator SegmentIter;
  typedef SegmentDeque::const_iterator SegmentConstIter;
  typedef PointDeque::iterator PointIter;
  typedef PointDeque::const_iterator PointConstIter;

  SegmentIter beginSegments() { return this->segments.begin(); }
  SegmentConstIter beginSegments() const  { return this->segments.begin(); }
  SegmentIter endSegments() { return this->segments.end(); }
  SegmentConstIter endSegments() const  { return this->segments.end(); }
  SegmentDeque::reference frontSegments() { return this->segments.front(); }
  SegmentDeque::reference backSegments() { return this->segments.back(); }
  SegmentDeque::const_reference frontSegments() const { return this->segments.front(); }
  SegmentDeque::const_reference backSegments() const { return this->segments.back(); }
  size_t numSegments() const { return this->segments.size(); }
  bool segmentsEmpty() const { return this->segments.empty(); }

  PointIter beginPoints() { return this->points.begin(); }
  PointConstIter beginPoints() const  { return this->points.begin(); }
  PointIter endPoints() { return this->points.end(); }
  PointConstIter endPoints() const  { return this->points.end(); }
  PointDeque::reference frontPoints()  { return this->points.front(); }
  PointDeque::reference backPoints()  { return this->points.back(); }
  PointDeque::const_reference frontPoints() const { return this->points.front(); }
  PointDeque::const_reference backPoints() const { return this->points.back(); }

  size_t numPoints() const { return this->points.size(); }
  bool pointsEmpty() const { return this->points.empty(); }

  bool isClosed() const { return *this->frontPoints() == *this->backPoints(); }

  bool addSegment(const Segment& segref) {
    const Segment* seg = &segref;

    if (segments.empty()) {
      segments.push_back(seg);
      points.push_back(&seg->first);
      points.push_back(&seg->second);
      return true;
    } else {
      const Point& last = *points.back();
      bool front = false;
      if (last == seg->first) {
        segments.push_back(seg);
        points.push_back(&seg->second);
      } else if (last == seg->second) {
        segments.push_back(new Segment(seg->second, seg->first, seg->settings));
        points.push_back(&seg->first);
      } else {
        front = true;
        const Point& first = *points.front();
        if (first == seg->first) {
          segments.push_front(new Segment(seg->second, seg->first, seg->settings));
          points.push_front(&seg->second);
        } else if (first == seg->second) {
          segments.push_front(seg);
          points.push_front(&seg->first);
        } else {
          return false;
        }
      }
      return true;
    }
    return false;
  }
private:
  SegmentDeque segments;
  PointDeque points;
};

typedef std::list<const Segment*> SegmentList;
typedef std::list<const SegmentString*> StringList;

class GeometryMapping : public boost::tuple<const Point*, const Segment*, const SegmentString*> {
public:
  GeometryMapping() : boost::tuple<const Point*, const Segment*, const SegmentString*>(0,0,0){}
  GeometryMapping(const Point* p, const Segment* seg, const SegmentString* string) : boost::tuple<const Point*, const Segment*, const SegmentString*>(p,seg,string){}

  const bool operator<(const GeometryMapping& other) const {
    const Point* p1 = this->get<0>();
    const Point* p2 = other.get<0>();
    const Segment* seg1 = this->get<1>();
    const Segment* seg2 = other.get<1>();
    const SegmentString* string1 = this->get<2>();
    const SegmentString* string2 = other.get<2>();


    bool stringLess = string1 < string2;
    bool stringEqual = string1 == string2;
    bool segLess = seg1 < seg2;
    bool segEqual = seg1 == seg2;
    bool pLess;
    bool pEqual;

    if(p1 != NULL && p2 != NULL) {
      if(string1 != NULL && string1 == string2 && string1->isClosed() && *p1 == *p2) {
        pLess = p1 < p2;
        pEqual = p1 == p2;
      } else {
        pLess = *p1 < *p2;
        pEqual = *p1 == *p2;
      }
    } else {
      pLess = p1 < p2;
      pEqual = p1 == p2;
    }

    return stringLess || (stringEqual && segLess) || (stringEqual && segEqual && pLess);
  }
};

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
    intersection.x = s1[0][0] + ua * (s1[1][0] - s1[0][0]);
    intersection.y = s1[0][1] + ua * (s1[1][1] - s1[0][1]);

    return ALIGN_INTERSECT;
  }

  return ALIGN_NONE;
}

inline std::ostream& operator<<(std::ostream &os, const Point &p) {
  os << "{" << p.x << "," << p.y << "}";
  return os;
}

inline std::ostream& operator<<(std::ostream &os, const Segment& segment) {
  os << "{" << segment.first << "," << segment.second << "}";
  return os;
}

struct SegmentNode {
  SegmentList::iterator owner;
  Point end_point;

  SegmentNode(const SegmentList::iterator owner, const Point& end_point) : owner(owner), end_point(end_point) {}

  const bool operator==(const SegmentNode& other) const {
    return *this->owner == *other.owner;
  }
};

struct StringNode {
  StringList::iterator owner;
  Point point;

  StringNode(const StringList::iterator owner, const Point& point) : owner(owner), point(point) {}

  const bool operator==(const StringNode& other) const {
    return *this->owner == *other.owner;
  }
};

inline int32_t segment_node_ac( SegmentNode item, int k ) { return item.end_point[k]; }
inline int32_t string_node_ac( StringNode item, int k ) { return item.point[k]; }

typedef KDTree::KDTree<2, SegmentNode, std::pointer_to_binary_function<SegmentNode,int,int32_t> > SegmentTree;
typedef KDTree::KDTree<2, StringNode, std::pointer_to_binary_function<StringNode,int,int32_t> > StringTree;

#endif /* GEOMETRY_H_ */
