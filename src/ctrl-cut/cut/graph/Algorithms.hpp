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

#include "cut/graph/SegmentGraph.hpp"
#include <boost/graph/filtered_graph.hpp>
#include <boost/foreach.hpp>

using boost::filtered_graph;
using boost::graph_traits;
/*
template<typename TmultiPointRange, typename Tgraph>
void load(TmultiPointRange src, Tgraph& graph) {
  BOOST_FOREACH(const typename TmultiPointRange::value_type pointRange, src) {
    graph.add(pointRange);
  }
}*/

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

template<typename Graph>
inline bool is_complete_graph(Graph& graph) {
  typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
  typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;

  BOOST_FOREACH(Vertex i, boost::vertices(graph)) {
    BOOST_FOREACH(Vertex j, boost::vertices(graph)) {
      if(i != j && !boost::edge(i, j,graph).second)
        return false;
    }
  }

  return true;
}


template<typename Tgraph>
inline void make_vertex_index(Tgraph& graph,
    std::map<typename graph_traits<Tgraph>::vertex_descriptor, typename graph_traits<Tgraph>::vertices_size_type>& index) {
  typename graph_traits<Tgraph>::vertices_size_type vertex_count = 0;

  BOOST_FOREACH(typename graph_traits<Tgraph>::vertex_descriptor v, vertices(graph)) {
    index[v] = vertex_count;
  }
}

template<typename Tgraph>
inline void make_edge_index(Tgraph& graph,
    std::map<typename graph_traits<Tgraph>::edge_descriptor, typename graph_traits<Tgraph>::edges_size_type>& index) {
  typename graph_traits<Tgraph>::edges_size_type edge_count = 0;

  BOOST_FOREACH(typename graph_traits<Tgraph>::edge_descriptor e, edges(graph)) {
    index[e] = edge_count;
  }
}

template<typename Tgraph>
inline void make_point_index(Tgraph& graph,
    std::map<Point, typename graph_traits<Tgraph>::vertex_descriptor>& index) {

  BOOST_FOREACH(typename graph_traits<Tgraph>::vertex_descriptor v, vertices(graph)) {
    index[graph[v]] = v;
  }
}

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

#endif
