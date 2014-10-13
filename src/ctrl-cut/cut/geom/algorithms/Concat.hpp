/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef CONCAT_HPP_
#define CONCAT_HPP_

#include "cut/geom/Path.hpp"
#include "cut/geom/Route.hpp"
#include <cut/geom/algorithms/Append.hpp>
#include <cut/geom/algorithms/Prepend.hpp>
#include <boost/geometry/algorithms/append.hpp>

inline bool concat_back(Path& path, const Segment& seg) {
  //FIXME where should we filter zero length segments
  if(seg.first == seg.second)
    return true;
  if (path.empty()) {
    append(path, seg);
    return true;
  } else {
    const Point& last = path.back();
    if (last == seg.first) {
      append(path, seg.second);
      return true;
    }
    else if (last == seg.second) {
      append(path, seg.first);
      return true;
    }
  }
  return false;
}

inline bool concat_front(Path& path, const Segment& seg) {
  //FIXME where should we filter zero length segments
  if(seg.first == seg.second)
    return true;
  if (path.empty()) {
    append(path, seg);
    return true;
  } else {
    const Point& first = path.front();
    if (first == seg.first) {
      prepend(path, seg.second);
      return true;
    }
    else if (first == seg.second) {
      prepend(path, seg.first);
      return true;
    }
  }
  return false;
}

inline bool concat(Path& path, const Segment& seg) {
  //FIXME where should we filter zero length segments
  if(seg.first == seg.second)
    return true;
  return concat_back(path, seg) || concat_front(path, seg);
}


template<typename TpointRange>
inline bool concat_back(Path& path, const TpointRange& pointRange) {
  for(const SegmentPtr seg : segments(pointRange)) {
    if (path.empty()) {
      append(path,*seg.get());
    } else if (!concat_back(path,*seg.get())) {
      return false;
    }
    return true;
  }
}

template<typename TpointRange>
inline bool concat_front(Path& path, const TpointRange& pointRange) {
  for(const SegmentPtr seg : segments(pointRange)) {
    if (path.empty()) {
      prepend(path,*seg.get());
    } else if (!concat_front(path,*seg.get())) {
      return false;
    }
    return true;
  }
}

template<typename TpointRange>
inline bool concat(Path& path, const TpointRange& pointRange) {
  return concat_back(path, pointRange) || concat_front(path, pointRange);
}

inline bool concat_front(Route& route, const Segment& seg) {
  //FIXME where should we filter zero length segments
  if(seg.first == seg.second)
    return true;
  if (route.empty()) {
    prepend(route, seg);
    return true;
  } else  {
    return concat_front(route.front(),seg);
  }
}

inline bool concat_back(Route& route, const Segment& seg) {
  //FIXME where should we filter zero length segments
  if(seg.first == seg.second)
    return true;
  if (route.empty()) {
    append(route, seg);
    return true;
  } else  {
    return concat_back(route.back(),seg);
  }
}

inline bool concat(Route& route, const Segment& seg) {
  //FIXME where should we filter zero length segments
  if(seg.first == seg.second)
    return true;
  if (route.empty()) {
    append(route,seg);
    return true;
  } else if(concat_back(route,seg) || concat_front(route,seg)) {
    return true;
  } else {
    for(Route::iterator it = route.begin(); it != route.end(); ++it) {
      if(concat(*it,seg))
        return true;
    }
  }
  return false;
}

template<typename TpointRange>
inline bool concat_back(Route& route, const TpointRange& pointRange) {
  for(const SegmentPtr seg : segments(pointRange)) {
    if (route.empty()) {
      append(route, *seg.get());
    } else if(!concat_back(route, *seg.get())) {
      return false;
    }
  }
  return true;
}

template<typename TpointRange>
inline bool concat_front(Route& route, const TpointRange& pointRange) {
  for(const SegmentPtr seg : segments(pointRange)) {
    if (route.empty()) {
      prepend(route, *seg.get());
    } else if(!concat_front(route, *seg.get())) {
      return false;
    }
  }
  return true;
}

template<typename TpointRange>
inline bool concat(Route& route, const TpointRange& pointRange) {
  return concat_back(route, pointRange) || concat_front(route, pointRange);
}

#endif
