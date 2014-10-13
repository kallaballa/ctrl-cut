/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
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
