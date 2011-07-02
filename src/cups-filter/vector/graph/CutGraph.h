#ifndef CUTGRAPH_H_
#define CUTGRAPH_H_

#include "StringGraph.h"
#include "SegmentGraph.h"

template<typename Graph>
bool filter_accept_all(const Graph& graph, typename Graph::Edge e) {
  return true;
}

template<typename Graph>
const typename Graph::Vertex get_opposite(const Graph& graph, const typename Graph::Edge edge, const typename Graph::Vertex one) {
  const typename Graph::Vertex source = boost::source(edge, graph);
  const typename Graph::Vertex target = boost::target(edge, graph);
  if (one == source)
    return target;
  else if (one == target)
    return source;
  else
    assert(false);
}

//FIXME doesn't work with directed graphs
//FIXME explicitly require a binary function object
template<typename Graph,typename _OutputIterator, typename FilterObject>
void find_edges(const Graph& graph, const typename Graph::Vertex& v, _OutputIterator out, FilterObject filter = std::ptr_fun<const Graph&, typename Graph::Edge, bool>(filter_accept_all)) {
  typename boost::graph_traits<Graph>::out_edge_iterator oe_it, oe_end;
  typename Graph::Edge edge;

  for (boost::tie(oe_it, oe_end) = boost::out_edges(v, graph); oe_it != oe_end; ++oe_it){
    edge = *oe_it;
    if(filter(graph,edge))
      *out++ = edge;
  }
}

template<typename Graph, typename _Iterator>
const typename Graph::Edge find_steapest(const Graph& graph, _Iterator first, _Iterator last) {
  // Find next clockwise segment
  typedef typename Graph::Edge Edge;
  float steapest = 2 * M_PI;
  const Edge* found = NULL;

  for (_Iterator it = first; it != last; ++it){
    const Edge candidate = *it;
    const Segment* seg = graph.getSegment(candidate);

    float slope = seg->getSlope();
    if (slope < steapest) {
      steapest = slope;
      found = &candidate;
    }
  }
  assert(found != NULL);
  return *found;
}

#endif
