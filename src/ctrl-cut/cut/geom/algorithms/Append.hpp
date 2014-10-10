#ifndef APPEND_HPP_
#define APPEND_HPP_

#include "cut/geom/Path.hpp"
#include "cut/geom/Route.hpp"
#include <boost/geometry/algorithms/append.hpp>

inline void append(Path& path, const Point& p) {
  boost::geometry::append(path, p);
}

inline void append(Path& path, const Segment& seg) {
  boost::geometry::append(path,seg.first);
  boost::geometry::append(path,seg.second);
}

template<typename TpointRange>
inline void append(Path& path, const TpointRange& pointRange) {
  boost::geometry::append(path, pointRange);
}

inline void append(Route& route, const Point& p) {
  Path path;
  append(path, p);
  route.push_back(path);
}

inline void append(Route& route, const Segment& seg) {
  Path path;
  append(path, seg);
  route.push_back(path);
}

template<typename TpointRange>
inline void append(Route& route, const TpointRange& pointRange) {
  Path path;
  append(path, pointRange);
  route.push_back(path);
}

#endif
