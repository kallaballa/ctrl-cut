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

#ifndef REDUCE_H_
#define REDUCE_H_

#include "util/Logger.hpp"
#include "cut/geom/Geometry.hpp"
#include "cut/Cut.hpp"
#include "cut/graph/SegmentGraph.hpp"
#include "cut/geom/algorithms/Algorithms.hpp"

#include <boost/foreach.hpp>

inline bool is_collapsed(const Path& path, const Path& simplified) {
  return is_self_intersecting(simplified)
      ||(is_closed(path) && (!is_closed(simplified) || simplified.size() <= 2));
}

template<typename TmultiPointRange>
void reduce(TmultiPointRange& src, TmultiPointRange& sink, double maxDistance) {
  LOG_INFO_STR("Reduce");

  typedef SegmentGraphImpl<boost::setS, boost::setS> UniqueSegmentGraph;
  UniqueSegmentGraph g;

  BOOST_FOREACH(const Segment& seg, segments(src)) {
    g.add(seg);
  }

  std::map<Point, UniqueSegmentGraph::Vertex> index;
  BOOST_FOREACH(UniqueSegmentGraph::Vertex v, vertices(g)) {
    index[g[v]] = v;
  }

  BOOST_FOREACH(Path& path, src) {
    Path singleBranch;
    Path simplified;
    Point last = path.front();
    Point current;
    BOOST_FOREACH(const Segment& seg, segments(path)) {
      concat(singleBranch,seg);
      if(last == seg.first)
        current = seg.second;
      else
        assert(false);

      last = current;
      if(boost::degree(index[current],g) > 2) {
        boost::geometry::simplify(singleBranch, simplified, maxDistance);
        if(!is_collapsed(singleBranch, simplified))
          add(sink, simplified);
        else {
          Box b;
          boost::geometry::envelope(simplified, b);
          if(b.width() > maxDistance && b.height() > maxDistance) {
            add(sink, simplified);
          }
        }

        singleBranch.clear();
        simplified.clear();
      }
    }

    if (!singleBranch.empty()) {
      boost::geometry::simplify(singleBranch, simplified, maxDistance);
      if (!is_collapsed(singleBranch, simplified))
        add(sink, simplified);
      else {
        Box b;
        boost::geometry::envelope(simplified, b);
        if(b.width() > maxDistance && b.height() > maxDistance) {
          add(sink, simplified);
        }
      }
    }

    singleBranch.clear();
    simplified.clear();
  }
  LOG_DEBUG(sink.size());
}
#endif
