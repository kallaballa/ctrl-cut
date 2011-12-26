#ifndef CUTGRAPH_H_
#define CUTGRAPH_H_

#include "StringGraph.h"
#include "SegmentGraph.h"
#include <boost/graph/filtered_graph.hpp>

using boost::filtered_graph;

template<typename Graph>
struct is_free_edge_predicate {
  typedef typename Graph::Edge Edge;
  const Graph* graph;

  is_free_edge_predicate() {}
  is_free_edge_predicate(const Graph& graph) : graph(&graph) {}

  bool operator()(const Edge& e) const {
    return !(*graph)[e].owned;
  }
};

template<typename Graph>
struct is_free_vertex_predicate {
  typedef typename Graph::Vertex Vertex;
  typedef typename boost::graph_traits<Graph>::out_edge_iterator EdgeIterator;
  const Graph* graph;

  is_free_vertex_predicate() {}
  is_free_vertex_predicate(const Graph& graph) : graph(&graph) {}

  bool operator()(const Vertex& v) const {
    EdgeIterator eit, eend;

    for(boost::tie(eit,eend) = boost::out_edges(v, *graph); eit != eend; ++eit) {
      if(!(*graph)[*eit].owned)
        return true;
    }
    return false;
  }
};


template<typename Graph>
class FreeGeometryView : public filtered_graph<Graph, is_free_edge_predicate<Graph>, is_free_vertex_predicate<Graph> > {
public:
  FreeGeometryView(const Graph& g) :
    filtered_graph<Graph, is_free_edge_predicate<Graph>, is_free_vertex_predicate<Graph> >(g, is_free_edge_predicate<Graph>(g), is_free_vertex_predicate<Graph>(g)) {}
};


template<typename Graph, typename Edge, typename Vertex>
const Vertex get_opposite(const Graph& graph, const Edge edge, const Vertex one) {
  Vertex source = boost::source(edge, graph);
  Vertex target = boost::target(edge, graph);
  if (one == source)
    return target;
  else if (one == target)
    return source;
  else
    assert(false);
}

template<typename Graph, typename _EdgeIterator>
void find_steapest(typename boost::graph_traits<Graph>::edge_descriptor& result, const Graph& graph, _EdgeIterator first, _EdgeIterator last) {
  // Find next clockwise segment
  typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
  float steapest = 2 * CC_PI;
  bool found = false;

  for (_EdgeIterator it = first; it != last; ++it){
    const Edge candidate = *it;
    Segment seg = graph[candidate];

    float steapness = CC_PI - seg.getSlope();

    if(steapness < 0)
      steapness += (2 * CC_PI);

    if (steapness < steapest) {
      steapest = steapness;
      result = candidate;
      found = true;
    }
  }
  assert(found);
}

#endif
