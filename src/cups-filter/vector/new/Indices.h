#ifndef INDICES_H_
#define INDICES_H_
#include <boost/graph/biconnected_components.hpp>
#include <boost/graph/connected_components.hpp>
#include <string>
#include <map>
#include "CutModel.h"

using std::map;
using std::pair;
using std::string;

struct compare_edge_vertices
{
  template<typename Edge>
  inline bool operator()(Edge const& left, Edge const& right) const {
    return left.m_source < right.m_source ||
        ((left.m_source == right.m_source) &&  (left.m_target < right.m_target));
  }
};

template<typename Graph>
class Indices {
private:
  static map<Graph*, Indices*> singletons;

  Indices() {}
public:
  typedef typename boost::graph_traits<Graph>
    ::vertex_descriptor Vertex;

  typedef typename boost::graph_traits<Graph>
    ::edge_descriptor Edge;

  typedef map<Vertex, int> VertexComponentMap;
  typedef map<Edge, int, compare_edge_vertices> EdgeComponentMap;

  // numComponents, <vertex, componentIndex>
  typedef pair<int,  VertexComponentMap > VertexComponentLookup;
  // numComponents, <edge, componentIndex>
  typedef pair<int,  EdgeComponentMap > EdgeComponentLookup;

  VertexComponentLookup* vc_lookup;
  EdgeComponentLookup* ec_lookup;

  static Indices<Graph>& getIndices(Graph& graph);
  virtual ~Indices() {}
};

template<typename Graph>
map<Graph*, Indices<Graph>*> Indices<Graph>::singletons = (* new map<Graph*, Indices<Graph>*>());

template<typename Graph>
Indices<Graph>& Indices<Graph>::getIndices(Graph& graph) {
  typename map<Graph*, Indices*>::iterator it = Indices::singletons.find(&graph);

  if(it == Indices<Graph>::singletons.end()) {
    Indices<Graph> *indices = new Indices<Graph>();
    Indices<Graph>::singletons[&graph] = indices;
    return *indices;
  }

  return *(*it).second;
}

template<typename Graph>
void put_vertex_component_lookup(Graph& graph, typename Indices<Graph>::VertexComponentLookup& vc_lookup) {
  Indices<Graph>::getIndices(graph).vc_lookup = &vc_lookup;
}

template<typename Graph>
void put_edge_component_lookup(Graph& graph, typename Indices<Graph>::EdgeComponentLookup& ec_lookup) {
  Indices<Graph>::getIndices(graph).ec_lookup = &ec_lookup;
}

template<typename Graph>
typename Indices<Graph>::VertexComponentLookup& get_vertex_component_lookup(Graph& graph) {
  typename Indices<Graph>::VertexComponentLookup *vc_lookup = Indices<Graph>::getIndices(graph).vc_lookup;
  assert(vc_lookup != NULL);
  return *vc_lookup;
}

template<typename Graph>
typename Indices<Graph>::EdgeComponentLookup& get_edge_component_lookup(Graph& graph) {
  typename Indices<Graph>::EdgeComponentLookup *ec_lookup = Indices<Graph>::getIndices(graph).ec_lookup;
  assert(ec_lookup != NULL);
  return *ec_lookup;
}

template<typename Graph>
void create_connected_component_lookup(Graph& graph) {
  typedef typename Indices<Graph>::VertexComponentLookup vc_lookup_t;
  vc_lookup_t *clup = new vc_lookup_t();
  clup->first = boost::connected_components(graph, boost::make_assoc_property_map(clup->second));
  put_vertex_component_lookup(graph, *clup);
}

template<typename Graph>
void create_biconnected_component_lookup(Graph& graph) {
  typedef typename Indices<Graph>::EdgeComponentLookup ec_lookup_t;
  ec_lookup_t *clup = new ec_lookup_t();
  clup->first = boost::biconnected_components(graph, boost::make_assoc_property_map(clup->second));
  put_edge_component_lookup(graph, *clup);
}

#endif /* INDICES_H_ */
