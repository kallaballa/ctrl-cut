#ifndef SEGMENTTREE_H_
#define SEGMENTTREE_H_

#include <kdtree++/kdtree.hpp>
#include "Geometry.h"
#include "Segment.h"

namespace kdt = KDTree;

struct SegmentNode : public Segment {
  enum BaseOrientation {
    FIRST,
    SECOND
  };

  BaseOrientation orientation;

  SegmentNode(const Point& base_point) : Segment(base_point, Point()), orientation(FIRST) {}
  SegmentNode(const Segment& seg, BaseOrientation orientation) : Segment(seg), orientation(orientation) {}

  bool operator==(const SegmentNode& other) const {
    return this->getBasePoint() == other.getBasePoint()
        && this->getDanglingPoint() == other.getDanglingPoint();
      return true;
  }

  Point getBasePoint() const {
    if(orientation == FIRST) {
      return this->first;
    } else {
      return this->second;
    }
  }

  Point getDanglingPoint() const {
    if(orientation == FIRST) {
      return this->first;
    } else {
      return this->second;
    }
  }
};

inline int32_t segment_node_ac( SegmentNode item, int k ) {
  return (item.getBasePoint())[k];
}

class SegmentTree: public kdt::KDTree<2, SegmentNode, std::pointer_to_binary_function<SegmentNode,int,int32_t> > {
public:
  SegmentTree(): kdt::KDTree<2, SegmentNode, std::pointer_to_binary_function<SegmentNode,int,int32_t> > (std::ptr_fun(segment_node_ac)) {}
  virtual ~SegmentTree() {};

  void add(const Segment& seg) {
    this->insert(SegmentNode(seg, SegmentNode::FIRST));
    this->insert(SegmentNode(seg, SegmentNode::SECOND));
  }

  void remove(const Segment& seg) {
    this->erase_exact(SegmentNode(seg, SegmentNode::FIRST));
    this->erase_exact(SegmentNode(seg, SegmentNode::SECOND));
  }

  const Point findNearest(const Point&  p) const {
    SegmentNode pos(p);
    return (*(this->find_nearest(pos).first)).getBasePoint();
  }

  void findWithinRange(const Segment& seg, std::list<Segment>& v) const {
    const Sphere& bsphere = seg.getSphere();
    this->find_within_range(SegmentNode(bsphere.center), bsphere.radius, std::back_inserter(v));
  }
};

template<typename _SegmentInputIterator>
void build(SegmentTree& tree, _SegmentInputIterator first, _SegmentInputIterator last) {
  BOOST_CONCEPT_ASSERT((SegmentInputIterator<_SegmentInputIterator>));
  for(_SegmentInputIterator it = first; it != last; ++it) {
    tree.add(*it);
  }
}

#endif
