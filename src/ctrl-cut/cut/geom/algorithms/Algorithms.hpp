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

#include "Append.hpp"
#include "Prepend.hpp"
#include "Concat.hpp"
#include "Add.hpp"

#ifndef ALGORITHMS_HPP_
#define ALGORITHMS_HPP_

template<typename TpointRange>
inline TpointRange make_from(const TpointRange& pointRange) {
  return TpointRange(pointRange.settings);
}

inline Path make_from(const Path& path) {
  return Path();
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

  BOOST_FOREACH(const Point& p, pointRange) {
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
