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
#ifndef ADD_HPP_
#define ADD_HPP_

#include "Concat.hpp"
#include "cut/geom/Route.hpp"
#include "cut/geom/Path.hpp"
#include <boost/foreach.hpp>

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
