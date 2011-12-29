#ifndef SEGMENTTREE_H_
#define SEGMENTTREE_H_

#include <kdtree++/kdtree.hpp>
#include "Geometry.h"
#include "Segment.h"

namespace kdt = KDTree;

struct SegmentNode {
  enum BaseOrientation {
    FIRST,
    SECOND
  };

  Segment owner;
  BaseOrientation orientation;

  SegmentNode(const Point& base_point) : orientation(FIRST) {
    owner.first = base_point;
  }
  SegmentNode(const Segment& seg, BaseOrientation orientation) : owner(seg), orientation(orientation) {}
   bool operator==(const SegmentNode& other) const {
      return this->getBasePoint() == other.getBasePoint() &&
          this->owner == other.owner;
  }

  Point getBasePoint() const {
    if(orientation == FIRST) {
      return owner.first;
    } else {
      return owner.second;
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
    std::pair<SegmentNode, SegmentNode> items = createSegmentNodes(seg);
    this->insert(items.first);
    this->insert(items.second);
  }

  void remove(const Segment& seg) {
    SegmentNode nf(seg, SegmentNode::FIRST);
    SegmentNode ns(seg, SegmentNode::SECOND);
    SegmentTree::const_iterator it_t = this->find(nf);
    if(it_t != this->end())
      this->erase(it_t);

    it_t = this->find(ns);
    if(it_t != this->end())
      this->erase(it_t);
  }

  const Point findNearest(const Point&  p) const {
    SegmentNode pos(p);
    return (*(this->find_nearest(pos).first)).getBasePoint();
  }

  void findWithinRange(const Segment& seg, std::list<SegmentNode>& v) const {
    const Sphere& bsphere = seg.getSphere();
    this->find_within_range(SegmentNode(bsphere.center), bsphere.radius, std::back_inserter(v));
  }

  static void build(SegmentTree& tree, SegmentList::iterator first, SegmentList::iterator last) {
    for(SegmentList::iterator it = first; it != last; ++it) {
      tree.add(*it);
    }
  }
private:
  std::pair<SegmentNode, SegmentNode> createSegmentNodes(const Segment& seg) {
    SegmentNode si_first(seg, SegmentNode::FIRST);
    SegmentNode si_second(seg, SegmentNode::SECOND);
    return std::pair<SegmentNode, SegmentNode>(si_first, si_second);
  }
};

#endif
