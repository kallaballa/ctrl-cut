#ifndef SEGMENTTREE_H_
#define SEGMENTTREE_H_

#include <kdtree++/kdtree.hpp>
#include "Geometry.h"
#include "Segment.h"

namespace kdt = KDTree;

struct SegmentNode : public Segment {
  Point center;
  uint32_t range;

  SegmentNode(const Point& center) : center(center) {}
  SegmentNode(const Segment& seg) : Segment(seg) {
    Sphere sphere(seg);
    center = sphere.center;
    range = sphere.radius;
  }

  bool operator==(const SegmentNode& other) const {
    return Segment::operator==(other);
  }
};

inline int32_t segment_node_ac( SegmentNode item, int k ) {
  return (item.center)[k];
}

class SegmentTree: public kdt::KDTree<2, SegmentNode, std::pointer_to_binary_function<SegmentNode,int,int32_t> > {
public:
  typedef  kdt::KDTree<2, SegmentNode, std::pointer_to_binary_function<SegmentNode,int,int32_t> > _Parent;

  SegmentTree(): kdt::KDTree<2, SegmentNode, std::pointer_to_binary_function<SegmentNode,int,int32_t> > (std::ptr_fun(segment_node_ac)) {}
  virtual ~SegmentTree() {};

  void findWithinRange(const SegmentNode& seg, std::list<SegmentNode>& v) const {
    this->find_within_range(seg.center, seg.range, std::back_inserter(v));
  }

  void insert(const SegmentNode& seg) {
    if(seg[0] == seg[1])
        return;

    _Parent::insert(seg);
  }

  void erase_exact(const SegmentNode& seg) {
    //FIxME find out why exploded can't find picks always via exact search
    const_iterator it = this->find_exact(seg);

    if(it != end())
      this->erase(it);
  }
};

template<typename _SegmentInputIterator>
void build(SegmentTree& tree, _SegmentInputIterator first, _SegmentInputIterator last) {
  BOOST_CONCEPT_ASSERT((SegmentInputIterator<_SegmentInputIterator>));
  for(_SegmentInputIterator it = first; it != last; ++it) {
    const Segment& seg = *it;
    if(seg[0] == seg[1])
        return;
    tree.insert(SegmentNode(seg));
  }
}

#endif
