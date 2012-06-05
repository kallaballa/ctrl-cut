/*
 * Algorithms.cpp
 *
 *  Created on: May 24, 2012
 *      Author: elchaschab
 */

//Path
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

//Route
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
