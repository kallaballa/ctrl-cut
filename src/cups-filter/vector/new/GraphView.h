#ifndef GRAPHVIEW_H_
#define GRAPHVIEW_H_

#include <list>
#include "CutModel.h"
#include "Indices.h"

using std::list;

template<typename Graph>
struct is_free_edge {
  typedef typename Indices<Graph>::Edge Edge;

  const Graph* graph;

  is_free_edge() {}
  is_free_edge(const Graph& graph) : graph(&graph) {}

  bool operator()(const Edge& e) const {
    return get(edge_geom, *graph)[e] == NULL;
  }
};

template<typename Graph>
struct is_component_vertex {
  typedef typename Indices<Graph>::VertexComponentMap vc_map_t;
  typedef typename Indices<Graph>::Vertex Vertex;

  vc_map_t *vertexCompMap;
  int componentIndex;

  is_component_vertex() {}
  is_component_vertex(vc_map_t& vertexCompMap, int componentIndex) : vertexCompMap(&vertexCompMap),  componentIndex(componentIndex) {
  }
  bool operator()(const Vertex& v) const {
    typename vc_map_t::iterator it = vertexCompMap->find(v);
    return it != vertexCompMap->end() &&  (*it).second == componentIndex;
  }
};

template<typename Graph>
struct is_component_edge {
  typedef typename Indices<Graph>::EdgeComponentMap ec_map_t;
  typedef typename Indices<Graph>::Edge Edge;

  const Graph* graph;
  ec_map_t* edgeCompMap;
  int componentIndex;
  is_free_edge<Graph> free_predicate;

  is_component_edge() {}
  is_component_edge(const Graph& graph, ec_map_t& edgeCompMap, int componentIndex) : graph(&graph), edgeCompMap(&edgeCompMap),  componentIndex(componentIndex), free_predicate(graph) {
  }

  bool operator()(const Edge& e) const {
    if(free_predicate(e)) {
      typename ec_map_t::const_iterator it = edgeCompMap->find(e);
      return  it != edgeCompMap->end() && (*it).second == componentIndex;
    } else
      return false;
  }
};

template<typename Graph>
class EdgeComponentView : public filtered_graph<Graph, is_component_edge<Graph>, boost::keep_all> {
public:
  EdgeComponentView(const Graph& g, typename Indices<Graph>::EdgeComponentMap& ec_map, int componentIndex) :
    filtered_graph<Graph, is_component_edge<Graph>, boost::keep_all>(g, (* new is_component_edge<Graph>(g, ec_map, componentIndex)), (* new boost::keep_all())) {}
};

template<typename Graph>
class VertexComponentView : public filtered_graph<Graph, is_free_edge<Graph>, is_component_vertex<Graph> > {
public:
  VertexComponentView(const Graph& g, typename Indices<Graph>::VertexComponentMap& vc_map, int componentIndex) :
    filtered_graph<Graph, is_free_edge<Graph>, is_component_vertex<Graph> >(g, is_free_edge<Graph>(g), (*new is_component_vertex<Graph>(vc_map, componentIndex))) {}
};


template<typename Graph>
inline list<EdgeComponentView<Graph> >& all_edge_component_views(Graph& graph) {
  list<EdgeComponentView<Graph> > *cgList =  new list<EdgeComponentView<Graph> >();
  typename Indices<Graph>::EdgeComponentLookup & clup = get_edge_component_lookup(graph);

  for (int i = 0; i < clup.first; ++i)
    cgList->push_back((* new EdgeComponentView<Graph>(graph, clup.second, i)));

  return *cgList;
}

template<typename Graph>
inline list<VertexComponentView<Graph> >& all_vertex_component_views(Graph& graph) {
  list<VertexComponentView<Graph> > *cgList =  new list<VertexComponentView<Graph> >();
  typename Indices<Graph>::VertexComponentLookup & clup = get_vertex_component_lookup(graph);

  for (int i = 0; i < clup.first; ++i)
    cgList->push_back((* new VertexComponentView<Graph>(graph, clup.second, i)));

  return *cgList;
}


#endif /* GRAPHVIEW_H_ */
