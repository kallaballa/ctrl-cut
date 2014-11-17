/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef POINTTREE_H_
#define POINTTREE_H_

#include "cut/geom/Concepts.hpp"
#include "Geometry.hpp"
#include <kdtree++/kdtree.hpp>
#include "boost/range.hpp"
#include "boost/foreach.hpp"

namespace kdt = KDTree;

static inline Coord_t acc_point(Point item, int k) {
  return item[k];
}

template<
  typename TpointNode,
  typename Taccessor
>
class PointTreeImpl : public kdt::KDTree<2, TpointNode, Taccessor> {
private:
  typedef TpointNode _PointNode;
  typedef kdt::KDTree<2, TpointNode, Taccessor> _Base;
public:
  PointTreeImpl() :
    _Base(std::ptr_fun(acc_point))
  {}

  void insert(const Point& point) {
    _Base::insert(point);
  }

  std::pair<typename _Base::const_iterator, typename _Base::distance_type> find_nearest_not_self(const Point& point) {
    auto result = this->find_nearest_if(point, std::numeric_limits<Coord_t>().max(), [&](const Point& candidate) {
      return candidate != point;
    });
    assert(point != (*result.first));

    return result;
  }

  std::pair<typename _Base::const_iterator, typename _Base::distance_type> find_nearest(const Point& point) {
    return _Base::find_nearest(point);
  }
};

typedef PointTreeImpl<
    Point,
    std::pointer_to_binary_function<Point, int, Coord_t>
> PointTree;

#endif
