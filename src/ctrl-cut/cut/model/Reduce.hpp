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
#include "util/Logger.hpp"
#include "cut/model/Cut.hpp"
#include "cut/graph/SegmentGraph.hpp"
#include <boost/foreach.hpp>

bool is_collapsed(const Path& path, const Path& simplified) {
  return is_self_intersecting(simplified)
      ||(is_closed(path) && (!is_closed(simplified) || simplified.size() <= 2));
}

template<typename TmultiPointRange>
void reduce(TmultiPointRange& src, TmultiPointRange& sink, double maxDistance) {
  LOG_INFO_STR("Reduce");

  AddSink<TmultiPointRange> addSink(sink);
  SegmentGraph g;
  build(src, g);
  LOG_DEBUG(num_vertices(g));
  std::map<Point, SegmentGraph::Vertex> index;
  make_vertex_index(g, index);

  BOOST_FOREACH(Path& path, src) {
    Path singleBranch = make_from(path);
    Path simplified = make_from(path);
    Point last = path.front();
    Point current;
    BOOST_FOREACH(const Segment& seg, segmentConstView(path)) {
      concat(singleBranch,seg);
      if(last == seg.first)
        current = seg.second;
      else
        assert(false);

      last = current;
      if(boost::degree(index[current],g) > 2) {
        boost::geometry::simplify(singleBranch, simplified, maxDistance);
        if(!is_collapsed(singleBranch, simplified))
          *addSink++ = simplified;
        singleBranch.clear();
        simplified.clear();
      }
    }

    if (!singleBranch.empty()) {
      boost::geometry::simplify(singleBranch, simplified, maxDistance);
      if (!is_collapsed(singleBranch, simplified))
        *addSink++ = simplified;
    }

    singleBranch.clear();
    simplified.clear();
  }
  LOG_DEBUG(sink.size());
}
