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

#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

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

template<typename Graph>
struct VertexPair {
  typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
  Vertex lower;
  Vertex higher;
  VertexPair() {

  }
  VertexPair(Vertex v1, Vertex v2) {
    if(v1 == v2)
      assert(!"source and target vertex are equal");

    if(v1 < v2) {
      this->lower = v1;
      this->higher = v2;
    } else {
      this->lower = v2;
      this->higher = v1;
    }
  }

  bool operator<(const VertexPair& other) const {
    return this->lower < other.lower || (this->lower == other.lower && this->higher < other.higher);
  }
};

template<typename Graph>
inline bool is_complete_graph(Graph& graph) {
  typename boost::graph_traits<Graph>::edge_iterator ei, ei_end;
  typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
  typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef VertexPair<Graph> VPair;
  std::set<VPair > uniqVertexPairs;

  for(boost::tie(ei, ei_end) = boost::edges(graph); ei != ei_end; ++ei) {
    Edge edge = *ei;
    VPair vpair(source(edge, graph), target(edge, graph));
    if(!uniqVertexPairs.insert(vpair).second) {
      assert(!"duplicate edge found");
    }
  }

  typename boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
  typename boost::graph_traits<Graph>::vertex_iterator vj, vj_end;
  Vertex i,j;

  for(boost::tie(vi, vi_end) = boost::vertices(graph); vi != vi_end; ++vi) {
    i = *vi;
    for(boost::tie(vj, vj_end) = boost::vertices(graph); vj != vj_end; ++vj) {
      j = *vj;
      if(i == j)
        continue;

      if(!boost::edge(i, j,graph).second)
        return false;
    }
  }

  return true;
}

template<typename Graph>
inline void make_vertex_index(Graph& graph, std::map<typename Graph::vertex_property_type::value_type, typename Graph::vertex_descriptor>& index) {
  BOOST_FOREACH(typename Graph::vertex_descriptor vertex, vertices(graph)) {
    Point p = graph[vertex];
    index[p] = vertex;
  }
}

template<typename Graph>
inline void make_edge_index(Graph& graph, std::map<typename Graph::edge_property_type::value_type, typename Graph::edge_descriptor>& index) {

  BOOST_FOREACH(typename Graph::edge_descriptor edge, edges(graph)) {
    index[graph[edge]] = edge;
  }
}

#endif
