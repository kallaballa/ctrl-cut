/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef ALGORITHMS_HPP_
#define ALGORITHMS_HPP_

#include "Path.hpp"
#include "Route.hpp"
#include "cut/model/Cut.hpp"
#include <boost/foreach.hpp>
#include <boost/geometry/algorithms/append.hpp>

// Path
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
inline void add(Path& path , const Segment& seg) {
  //FIXME where should we filter zero length segments
  if(seg.first == seg.second)
    return true;
  if(!concat(path, seg))
    return append(path, seg);
}

template<typename TpointRange>
inline bool concat_back(Path& path, const TpointRange& pointRange) {
  BOOST_FOREACH(const Segment& seg, segmentView(pointRange)) {
    if (path.empty()) {
      append(path,seg);
    } else if (!concat_back(path,seg)) {
      return false;
    }
    return true;
  }
}

template<typename TpointRange>
inline bool concat_front(Path& path, const TpointRange& pointRange) {
  BOOST_FOREACH(const Segment& seg, segmentView(pointRange)) {
    if (path.empty()) {
      prepend(path,seg);
    } else if (!concat_front(path,seg)) {
      return false;
    }
    return true;
  }
}

template<typename TpointRange>
inline bool concat(Path& path, const TpointRange& pointRange) {
  return concat_back(path, pointRange) || concat_front(path, pointRange);
}

template<typename TpointRange>
inline void add(Path& path ,  const TpointRange& pointRange) {
  if(!concat(path, pointRange))
    return append(path, pointRange);
}


// ### ROUTE ### //

inline void append(Route& route, const Point& p) {
  Path path(route.settings);
  append(path, p);
  route.push_back(path);
}

inline void append(Route& route, const Segment& seg) {
  Path path(route.settings);
  append(path, seg);
  route.push_back(path);
}

template<typename TpointRange>
inline void append(Route& route, const TpointRange& pointRange) {
  Path path(route.settings);
  append(path, pointRange);
  route.push_back(path);
}

inline void prepend(Route& route, const Point& p) {
  Path path(route.settings);
  append(path, p);
  route.push_back(path);
}

inline void prepend(Route& route, const Segment& seg) {
  Path path(route.settings);
  append(path, seg);
  route.push_back(path);
}

template<typename TpointRange>
inline void prepend(Route& route, const TpointRange& pointRange) {
  Path path(route.settings);
  append(path, pointRange);
  route.push_back(path);
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
inline void add(Route& route , const Segment& seg) {
  //FIXME where should we filter zero length segments
  if(seg.first == seg.second)
    return true;
  if(!concat(route, seg))
    return append(route, seg);
}

template<typename TpointRange>
inline bool concat_back(Route& route, const TpointRange& pointRange) {
  BOOST_FOREACH(const Segment& seg, segmentConstView(pointRange)) {
    if (route.empty()) {
      append(route, seg);
    } else if(!concat_back(route, seg)) {
      return false;
    }
  }
  return true;
}

template<typename TpointRange>
inline bool concat_front(Route& route, const TpointRange& pointRange) {
  BOOST_FOREACH(const Segment& seg, segmentConstView(pointRange)) {
    if (route.empty()) {
      prepend(route, seg);
    } else if(!concat_front(route, seg)) {
      return false;
    }
  }
  return true;
}

template<typename TpointRange>
inline bool concat(Route& route, const TpointRange& pointRange) {
  return concat_back(route, pointRange) || concat_front(route, pointRange);
}

template<typename TpointRange>
inline void add(Route& route, const TpointRange& pointRange) {
  if(!concat(route, pointRange))
    return append(route, pointRange);
}

template<typename TpointRange>
inline TpointRange make_from(const TpointRange& pointRange) {
  return TpointRange(pointRange.settings);
}

#endif /* ALGORITHMS_HPP_ */
