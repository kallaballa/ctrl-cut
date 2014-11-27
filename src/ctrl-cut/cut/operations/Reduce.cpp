/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "Reduce.hpp"
#include "util/Logger.hpp"
#include "cut/geom/Geometry.hpp"
#include "cut/graph/SegmentGraph.hpp"
#include "cut/geom/algorithms/Algorithms.hpp"
#include <boost/foreach.hpp>

inline bool is_collapsed(const Path& path, const Path& simplified) {
  return is_self_intersecting(simplified)
      ||(is_closed(path) && (!is_closed(simplified) || simplified.size() <= 2));
}


void reduce(Route& src, Route& sink, double maxDistance) {
  if(maxDistance == 0)
    return;
  LOG_DEBUG(src.size());

  typedef SegmentGraphImpl<boost::setS, boost::setS> UniqueSegmentGraph;
  UniqueSegmentGraph g;

  for(const SegmentPtr seg : segments(src)) {
    g.addSegment(*seg.get());
  }

  std::map<Point, UniqueSegmentGraph::Vertex> index;
  BOOST_FOREACH(UniqueSegmentGraph::Vertex v, vertices(g)) {
    index[g[v]] = v;
  }

  for(Path& path : src) {
    Path singleBranch;
    Path simplified;
    Point last = path.front();
    Point current;
    for(const SegmentPtr segPtr :  segments(path)) {
      const Segment& seg = *segPtr.get();
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
