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


#ifndef TRAVELLER_HPP_
#define TRAVELLER_HPP_

template<typename TsegmentInputIterator, typename TsegmentOutputIterator>
class Traveler {
  class VertexAppender {
  public:
    Tgraph* graph;
    TsegmentOutputIterator* sink;

    VertexAppender(Tgraph& graph, TsegmentOutputIterator& sink) :
      graph(&graph), sink(&sink), first(true) {
      BOOST_CONCEPT_ASSERT((SegmentOutputIterator<TsegmentOutputIterator>));
    }
    VertexAppender(const VertexAppender& rb) :
      graph(rb.graph), sink(rb.sink), first(rb.first) {
      BOOST_CONCEPT_ASSERT((SegmentOutputIterator<TsegmentOutputIterator>));
    }

    void operator()(typename Tgraph::Vertex v) {
      const Point& current = (*graph)[v];
      if (!first) {
        sink++ = Segment(this->last, current);
      }
      this->last = current;
    }
  private:
    bool first;
    Point last;
  };

  void operator()() {
    using namespace boost;
    LOG_INFO_STR("travel linestrings");

    DistanceGraph graph;
    DistanceGraph::Vertex v_origin = create_complete_graph_from_point(graph,
        Point(0, 0), begin, end);

    typedef boost::property_map<boost::adjacency_list<boost::vecS, boost::vecS,
        boost::undirectedS, Point, WeightProperty>, double WeightProperty::*>::type
        WeightMap;
    WeightMap weight_map(get(&WeightProperty::weight, graph));

    vector<DistanceGraph::Vertex> tour;
    double len = 0.0;
    VertexAppender<DistanceGraph, TsegmentOutputIterator> rb(graph, sink);

    boost::metric_tsp_approx_from_vertex(
        graph,
        v_origin,
        weight_map,
        boost::make_tsp_tour_visitor(
            make_function_output_iterator(boost::bind<void>(rb, _1))));

    LOG_INFO_MSG("Tour length", len);

#ifdef DEBUG
    check(begin, end);
#endif
  }
};

#endif /* TRAVELLER_HPP_ */
