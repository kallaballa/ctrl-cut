/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef PREPREND_HPP_
#define PREPREND_HPP_

#include "cut/geom/Path.hpp"
#include "cut/geom/Route.hpp"
#include <boost/geometry/algorithms/reverse.hpp>
#include <boost/geometry/algorithms/append.hpp>

inline void prepend(Path& path, const Point& p) {
  boost::geometry::reverse(path);
  boost::geometry::append(path, p);
}

inline void prepend(Path& path, const Segment& seg) {
  boost::geometry::reverse(path);
  boost::geometry::append(path,seg.first);
  boost::geometry::append(path,seg.second);
}

template<typename TpointRange>
inline void prepend(Path& path, const TpointRange& pointRange) {
  boost::geometry::reverse(path);
  boost::geometry::append(path, pointRange);
}

inline void prepend(Route& route, const Point& p) {
  Path path;
  append(path, p);
  route.push_back(path);
}

inline void prepend(Route& route, const Segment& seg) {
  Path path;
  append(path, seg);
  route.push_back(path);
}

template<typename TpointRange>
inline void prepend(Route& route, const TpointRange& pointRange) {
  Path path;
  append(path, pointRange);
  route.push_back(path);
}

#endif
