/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "Traveller.hpp"
#include "cut/geom/algorithms/Append.hpp"
#include <boost/property_map/property_map.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/metric_tsp_approx.hpp>
#include <boost/function_output_iterator.hpp>
#include <cut/graph/CompleteDistanceGraph.hpp>
#include <functional>

class Traveler {
public:
  typedef CompleteDistanceGraph<Path> _WeightedGraph;
  const Route* src;
  Route* sink;
  _WeightedGraph graph;

  Traveler(const Route& src, Route& sink) : src(&src), sink(&sink) {}

  class VertexAppender {
  public:
    VertexAppender(_WeightedGraph& graph, Route* sink) :
      graph(&graph),sink(sink), first(true) {}
    VertexAppender(const VertexAppender& rb) :
      graph(rb.graph),sink(rb.sink),first(rb.first) {}

    void operator()(typename _WeightedGraph::Vertex v) {
      append(*sink,(*graph)[v]);
    }
  private:
    _WeightedGraph* graph;
    Route* sink;
    bool first;
  };

  void operator()() {
    using namespace boost;
    LOG_INFO_STR("travel linestrings");

    for(const Path& path : *this->src) {
      this->graph.addVertex(path);
    }
    this->graph.makeComplete();

    VertexAppender rb(this->graph,this->sink);

    auto propMap = get(&DistanceProperty::distance, this->graph);

    typedef _WeightedGraph::edge_iterator EdgeIterator;
    std::pair<EdgeIterator, EdgeIterator> edges = boost::edges(graph);
    std:: cerr << this->graph.m_edges.size() << std::endl;
    EdgeIterator edge;
    for (edge = edges.first; edge != edges.second; ++edge) {
      std::cerr << boost::get(propMap, *edge) << std::endl;
    }

    boost::metric_tsp_approx_from_vertex(
        this->graph,
        *boost::vertices(graph).first,
        get(&DistanceProperty::distance, this->graph),
        boost::make_tsp_tour_visitor(
            make_function_output_iterator(rb)));
    this->sink->erase(sink->end() - 1);
  }
};

void streamline(Route& r) {
  Path lastPath;
  for(Path& p : r) {
    if(!lastPath.empty()) {
      if(lastPath.back().distance(p.back()) < lastPath.back().distance(p.front()))
        boost::geometry::reverse(p);
    }

    lastPath = p;
  }
}

void travel(const Route& src, Route& sink) {
  LOG_DEBUG(src.size());

  Traveler traveler(src, sink);
  traveler();
  streamline(sink);

  LOG_DEBUG(sink.size());
}
