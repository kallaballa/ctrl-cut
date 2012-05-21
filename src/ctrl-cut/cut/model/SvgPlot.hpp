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
 * GNU General Public License for more de0tails.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef PLOT_CUT_H_
#define PLOT_CUT_H_

#include "Cut.hpp"
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "svg/SvgWriter.hpp"

void find_shared_points(const Cut& cut, std::vector<Point>& sharedPoints) {
  SegmentGraph g;
  build(cut, g);

  BOOST_FOREACH(SegmentGraph::Vertex v, vertices(g)) {
    if(boost::degree(v,g) > 2) {
      sharedPoints.push_back(g[v]);
    }
  }
}

void plot_shared_segments(const Cut& cut, const char* filename) {
  std::set<Segment> segidx;
  uint32_t width = cut.get(DocumentSettings::WIDTH).in(PX);
  uint32_t height = cut.get(DocumentSettings::HEIGHT).in(PX);

  SvgWriter svg(width, height, filename);

  BOOST_FOREACH(const Path& path, cut) {
    svg.write(path, "stroke:rgb(0,0,0);stroke-width:1");

    BOOST_FOREACH(const Segment& seg, segmentConstView(path)) {
      if(segidx.find(seg) != segidx.end()) {
        Path pseg = make_from(path);
        append(pseg, seg);
        svg.write(pseg, "stroke:rgb(0,255,0);stroke-width:1");
        break;
      }
    }
  }
}

void plot_shared_points(const Cut& cut, const char* filename) {
  uint32_t width = cut.get(DocumentSettings::WIDTH).in(PX);
  uint32_t height = cut.get(DocumentSettings::HEIGHT).in(PX);

  SvgWriter svg(width, height, filename);
  std::vector<Point> sharedPoints;

  find_shared_points(cut, sharedPoints);

  BOOST_FOREACH(const Path& path, cut) {
    svg.write(path, "stroke:rgb(0,0,0);stroke-width:1");
  }

  BOOST_FOREACH(const Point& p, sharedPoints) {
     svg.write(p, "stroke:rgb(255,0,0);stroke-width:1;fill:none");
  }
}
#endif
