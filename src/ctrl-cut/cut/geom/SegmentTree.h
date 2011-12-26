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

  SegmentList::iterator* it;
  Segment owner;
  BaseOrientation orientation;

  SegmentNode(const Point& base_point) : it(NULL), orientation(FIRST) {
    owner.first = base_point;
  }
  SegmentNode(const Segment& seg, BaseOrientation orientation) : it(NULL), owner(seg), orientation(orientation) {}
  SegmentNode(SegmentList::iterator& it, BaseOrientation orientation) : it(new SegmentList::iterator(it)), owner(*it), orientation(orientation) {}

  bool operator==(const SegmentNode& other) const {
      return this->owner == other.owner && this->getBasePoint() == other.getBasePoint();
  }

  Point getBasePoint() const {
    if(orientation == FIRST) {
      return owner.first;
    } else {
      return owner.second;
    }
  }

  SegmentList::iterator& getIterator() {
    assert(this->it != NULL);
    return *this->it;
  }
};

inline int32_t segment_node_ac( SegmentNode item, int k ) {
  return (item.getBasePoint())[k];
}

class SegmentTree: public kdt::KDTree<2, SegmentNode, std::pointer_to_binary_function<SegmentNode,int,int32_t> > {
public:
  SegmentTree(): kdt::KDTree<2, SegmentNode, std::pointer_to_binary_function<SegmentNode,int,int32_t> > (std::ptr_fun(segment_node_ac)) {}
  virtual ~SegmentTree() {};

  void add(SegmentList::iterator& it_seg) {
    std::pair<SegmentNode, SegmentNode> items = createSegmentNodes(it_seg);
    this->insert(items.first);
    this->insert(items.second);
  }

  void remove(SegmentList::iterator& it_seg) {
    std::pair<SegmentNode, SegmentNode> items = createSegmentNodes(it_seg);
    iterator it_node = this->find_exact(items.first);
    if(it_node != end())
      this->erase(it_node);
    else
      assert(false);

    it_node = this->find_exact(items.second);
    if(it_node != end())
      this->erase(it_node);
    else
      assert(false);
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
    // FIXME
    SegmentNode pos = *new SegmentNode(p);
    return (*(this->find_nearest(pos).first)).getBasePoint();
  }

  void findWithinRange(SegmentList::iterator& it_seg, std::list<SegmentNode>& v) const {
    const Sphere& bsphere = (*it_seg).getSphere();
    //REFACTOR
    //SegmentNode scenter = *new SegmentNode(it_seg, bsphere.center);
    //this->find_within_range(scenter, bsphere.radius, std::back_inserter(v));
  }

  static SegmentTree& build(SegmentList::iterator first, SegmentList::iterator last) {
    SegmentTree* segTree = new SegmentTree();

    for(SegmentList::iterator it = first; it != last; ++it) {
      segTree->add(it);
    }

    return *segTree;
  }
private:
  std::pair<SegmentNode, SegmentNode> createSegmentNodes(SegmentList::iterator& it_seg) {
    //FIXME
    SegmentNode si_first = *new SegmentNode(it_seg, SegmentNode::FIRST);
    SegmentNode si_second = *new SegmentNode(it_seg, SegmentNode::SECOND);
    return std::pair<SegmentNode, SegmentNode>(si_first, si_second);
  }
};

#endif
