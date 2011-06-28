#ifndef CUTGRAPH_H_
#define CUTGRAPH_H_

#include "StringGraph.h"
#include "SegmentGraph.h"

template<typename Graph>
const typename Graph::Vertex getOther(const Graph& graph, const typename Graph::Edge edge, const typename Graph::Vertex one) {
  const typename Graph::Vertex source = boost::source(edge, graph);
  const typename Graph::Vertex target = boost::target(edge, graph);
  if (one == source)
    return target;
  else if (one == target)
    return source;
  else
    assert(false);
}

#endif
