#ifndef ADD_HPP_
#define ADD_HPP_

#include "Concat.hpp"
#include "cut/geom/Route.hpp"
#include "cut/geom/Path.hpp"

template<typename TpointRange>
inline void add(Path& path , const Segment& seg) {
  //FIXME where should we filter zero length segments
  if(seg.first == seg.second)
    return;

  if(!concat(path, seg))
    append(path, seg);
}

template<typename TpointRange>
inline void add(Path& path ,  const TpointRange& pointRange) {
  if(!concat(path, pointRange))
    append(path, pointRange);
}

template<typename TpointRange>
inline void add(Route& route , const Segment& seg) {
  //FIXME where should we filter zero length segments
  if(seg.first == seg.second)
    return;

  if(!concat(route, seg))
    append(route, seg);
}

template<typename TpointRange>
inline void add(Route& route, const TpointRange& pointRange) {
  if(!concat(route, pointRange))
    append(route, pointRange);
}

#endif
