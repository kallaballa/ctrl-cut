#ifndef ALGORITHMS_HPP_
#define ALGORITHMS_HPP_

#include "Append.hpp"
#include "Prepend.hpp"
#include "Concat.hpp"
#include "Add.hpp"

template<typename TpointRange>
inline TpointRange make_from(const TpointRange& pointRange) {
  return TpointRange(pointRange.settings);
}

template<typename TpointRange>
inline bool is_closed(const TpointRange& pointRange) {
  return pointRange.front() == pointRange.back();
}

template<typename TpointRange>
inline bool is_self_intersecting(const TpointRange& pointRange) {
  bool first = true;
  bool isClosed = is_closed(pointRange);
  std::set<Point> pointset;

  for(const Point& p : pointRange) {
    if(isClosed && !first) {
      if(!pointset.insert(p).second) {
        return true;
      }
    }
    first = false;
  }
  return false;
}
#endif
