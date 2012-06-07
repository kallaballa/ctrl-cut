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

#ifndef COMPLETETIPGRAPH_HPP_
#define COMPLETETIPGRAPH_HPP_

#include "cut/geom/Path.hpp"
#include <boost/function_output_iterator.hpp>
#include <boost/foreach.hpp>
/*
template<
  typename Tgeom
>
void make_distance_graph(const TmultiPointRange& src, Tgraph& sink) {
  const Point origin;
  std::vector<Segment> tips;

  BOOST_FOREACH(const Segment& seg, segments(src)) {
      const Segment seg(geom.back(), geom.front());

      graph.add(geom);
      graph.create(Segment(seg[0], origin));
      graph.create(Segment(seg[1], origin));

      for (std::vector<Segment>::iterator it = tips.begin(); it != tips.end(); ++it) {
        Segment t = *it;
        if(seg != t) {
          if(seg[0] != t[0])
            graph.create(Segment(seg[0], t[0]));
          if(seg[0] != t[1])
            graph.create(Segment(seg[0], t[1]));
          if(seg[1] != t[0])
            graph.create(Segment(seg[0], t[0]));
          if(seg[1] != t[1])
            graph.create(Segment(seg[0], t[1]));
        }
      }

      tips.push_back(seg);
  }
}*/

#endif
