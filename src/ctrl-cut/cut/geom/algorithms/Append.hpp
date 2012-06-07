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
#ifndef APPEND_HPP_
#define APPEND_HPP_

#include "cut/geom/Path.hpp"
#include "cut/geom/Route.hpp"
#include <boost/foreach.hpp>
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
