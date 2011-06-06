#ifndef CUTGRAPH_H_
#define CUTGRAPH_H_

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <map>

#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "Index.h"
#include "Traverse.h"
#include "CutEdge.h"
#include "CutVertex.h"

using boost::adjacency_list;
using boost::undirectedS;
using boost::vecS;
using boost::add_vertex;
using boost::add_edge;
using std::vector;
using std::map;
using std::pair;

typedef adjacency_list<vecS, vecS, undirectedS, PointProperty, CutProperty> CutGraph;

typedef typename boost::graph_traits<CutGraph>
  ::vertex_descriptor Vertex;

typedef typename boost::graph_traits<CutGraph>
  ::edge_descriptor Edge;

template<typename Graph>
Vertex findOrAddVertex(Graph& graph, Point &point) {
  typename Index<Graph>::Points::const_iterator it = Index<Graph>::points.find(point);
    if (it == Index<Graph>::points.end()) {
        Vertex new_vertex = add_vertex(PointProperty(point),graph);
        Index<Graph>::points[point] = new_vertex;
        return new_vertex;
    }

    return (*it).second;
}

void createEdge(CutGraph& graph, Point &in, Point &out, LaserSettings& settings);
void createEdge(CutGraph& graph, uint32_t inX, uint32_t inY, uint32_t outX, uint32_t outY, LaserSettings& settings);

template<typename Graph>
typename Index<Graph>::Linestrings& build_linestrings(Graph& graph) {
  typename Index<Graph>::Linestrings& linestrings = (* new (typename Index<Graph>::Linestrings)());
  traverse_linestrings(graph, linestrings);
  return linestrings;
}

template<typename Graph>
typename Index<Graph>::Rtree& build_rtree(typename Index<Graph>::Linestrings& linestrings) {
  typename Index<Graph>::Rtree& rtree = (* new (typename Index<Graph>::Rtree)(9,3));
  typename Index<Graph>::Linestrings::iterator it;

  for(it = linestrings.begin(); it != linestrings.end(); ++it)
    rtree.insert(*(*it).second, (*it).first);

  return rtree;
}

template<typename Vertex, typename Graph>
inline const Point& get_point(const Vertex& v, const Graph& graph) {
  return get(vertex_point, graph)[v];
}

template<typename Edge, typename Graph>
inline const Point& source_point(const Edge& e, const Graph& graph) {
  return get_point(source(e, graph), graph);
}

template<typename Edge, typename Graph>
inline const Point& target_point(const Edge& e, const Graph& graph) {
  return get_point(target(e, graph), graph);
}

template<typename Edge, typename Graph>
inline const Segment& segment(const Edge& e, const Graph& graph) {
  return (*new Segment(source_point(e, graph), target_point(e, graph)));
}

template<typename Edge, typename Graph>
inline double get_length(const Edge& e, const Graph& graph) {
  return get(edge_length, graph)[e];
}

#endif
