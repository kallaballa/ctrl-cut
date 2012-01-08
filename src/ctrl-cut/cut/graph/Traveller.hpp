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

#include "cut/graph/DistanceGraph.hpp"
#include "cut/geom/sink/AddSink.hpp"
#include "cut/geom/sink/CompleteTipGraphSink.hpp"
#include <boost/property_map/property_map.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/metric_tsp_approx.hpp>

#ifndef TRAVELLER_HPP_
#define TRAVELLER_HPP_

template<typename TmultiPointRange>
class Traveler {
public:
  typedef DistanceGraph<typename TmultiPointRange::iterator::value_type> _DistanceGraph;
  const TmultiPointRange* src;
  TmultiPointRange* sink;
  _DistanceGraph graph;

  Traveler(const TmultiPointRange& src, TmultiPointRange& sink) : src(&src), sink(&sink) {}

  class VertexAppender {
  public:
    VertexAppender(_DistanceGraph& graph, TmultiPointRange* sink) :
      graph(&graph),sink(sink), first(true) {}
    VertexAppender(const VertexAppender& rb) :
      graph(rb.graph),sink(rb.sink),first(rb.first) {}

    void operator()(typename _DistanceGraph::Vertex v) {
      append(*sink,(*graph)[v]);
    }
  private:
    _DistanceGraph* graph;
    TmultiPointRange* sink;
    bool first;
  };

  void operator()() {
    using namespace boost;
    LOG_INFO_STR("travel linestrings");
    buildComleteTipGraph(*this->src, this->graph);
    VertexAppender rb(this->graph,this->sink);

    boost::metric_tsp_approx_from_vertex(
        this->graph,
        0,
        get(&WeightProperty::weight, this->graph),
        boost::make_tsp_tour_visitor(
            make_function_output_iterator(boost::bind<void>(rb, _1))));
  }
};

template<
  typename TmultiPointRange
>
void travel(const TmultiPointRange& src, TmultiPointRange& sink) {
  Traveler<TmultiPointRange> traveler(src, sink);
  traveler();
}

#endif /* TRAVELLER_HPP_ */
