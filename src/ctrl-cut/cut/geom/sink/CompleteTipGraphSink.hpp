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

template<typename Tgraph, typename TpointRange>
struct BuildCompleteTipGraphFunc {
  Tgraph* graph;
  const Point origin;
  std::vector<Segment> tips;

  BuildCompleteTipGraphFunc(Tgraph& graph, const Point origin = Point(0,0)) : graph(&graph), origin(origin) {
//FIXME concept check
  }

  void operator()(const TpointRange& geom) {
      const Point& front = geom.front();
      const Point& back = geom.back();
      const Segment seg(front,back);

      graph.add(seg,0);
      graph.create(seg.first, origin);
      graph.create(seg.second, origin);

      for (std::vector<Segment>::iterator it = tips.begin(); it != tips.end(); ++it) {
        Segment t = *it;
        if(seg != t) {
          if(seg[0] != t[0])
            graph.create(seg[0], t[0]);
          if(seg[0] != t[1])
            graph.create(seg[0], t[1]);
          if(seg[1] != t[0])
            graph.create(seg[1], t[0]);
          if(seg[1] != t[1])
            graph.create(seg[1], t[1]);
        }
      }

      tips.push_back(seg);
  }
};

template <class Tgraph,class TpointRange>
class CompleteTipGraphSink : public boost::function_output_iterator<BuildCompleteTipGraphFunc<Tgraph,TpointRange> > {
public:
  typedef boost::function_output_iterator<BuildCompleteTipGraphFunc<Tgraph, TpointRange> > _Base;
  typedef std::output_iterator_tag iterator_category;
  typedef void value_type;
  typedef void difference_type;
  typedef void pointer;
  typedef void reference;

  explicit CompleteTipGraphSink() :
    _Base() {};

  CompleteTipGraphSink(Tgraph& graph) :
    _Base(ConcatFunc<TpointRange>(graph))
      {};

  CompleteTipGraphSink(const CompleteTipGraphSink& other) :
    _Base(other)
      {};
};

template<
  typename TmultiPointRange,
  typename Tgraph
>
void buildComleteTipGraph(const TmultiPointRange& multiPointRange, Tgraph& graph) {
  CompleteTipGraphSink<Tgraph,typename TmultiPointRange::value_type> sink(graph);
  std::copy(multiPointRange.begin(), multiPointRange.end(), sink);
}

#endif /* COMPLETETIPGRAPH_HPP_ */
