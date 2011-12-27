#ifndef SEGMENTSTRING_H_
#define SEGMENTSTRING_H_

#include "Geometry.h"
#include <algorithm>
#include "cut/model/CutModel.h"

class SegmentString : public CutModel {
public:
  typedef std::list<Point> PointList;
  typedef PointList::iterator PointIter;
  typedef PointList::const_iterator PointConstIter;

  SegmentString(const CutModel& model);

  void push_front(const Segment& seg);
  void push_back(const Segment& seg);

  PointIter beginPoints() { return this->points.begin(); }
  PointConstIter beginPoints() const  { return this->points.begin(); }
  PointIter endPoints() { return this->points.end(); }
  PointConstIter endPoints() const  { return this->points.end(); }
  PointList::reference frontPoints()  { return this->points.front(); }
  PointList::reference backPoints()  { return this->points.back(); }
  PointList::const_reference frontPoints() const { return this->points.front(); }
  PointList::const_reference backPoints() const { return this->points.back(); }


  size_t numPoints() const { return this->points.size(); }
  bool pointsEmpty() const { return this->points.empty(); }

  bool isClosed() const { return this->frontPoints() == this->backPoints(); }
  bool append(const Segment& seg);
  bool splice(SegmentString& other);
  void splice(iterator pos, iterator begin, iterator end) {
    assert(!"not implemented");
  }

  void copy(const SegmentString& other);
  friend std::ostream& operator<<(std::ostream &os, const SegmentString& string) {
    os << "<string>" << std::endl;
    for(SegmentString::PointConstIter it=string.beginPoints(); it != string.endPoints(); ++it)
      os << "  " << *it << std::endl;

    os << std::endl;

    for(SegmentString::const_iterator it=string.begin(); it != string.end(); ++it)
      os << *it << std::endl;

    os << "</string>" << std::endl;
    return os;
  }
private:
  bool appendFront(const Segment& seg);
  bool appendBack(const Segment& seg);
  bool spliceBack(SegmentString& other);
  bool spliceFront(SegmentString& seg);
  PointList points;
};

#endif
