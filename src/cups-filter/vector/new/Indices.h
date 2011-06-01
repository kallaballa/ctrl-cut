#ifndef INDICES_H_
#define INDICES_H_
#include <boost/graph/biconnected_components.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/geometry/extensions/index/rtree/rtree.hpp>
#include <string>
#include <map>
#include <set>

using std::set;
using std::map;
using std::pair;
using std::string;
using boost::geometry::index::rtree;
using boost::geometry::envelope;

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

  Indices(): spatialIndex(12,4) {}
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

  typedef set<Point, boost::geometry::less<Point> > PointIndex;
  typedef map<Box*, Linestring> BoxLinestringLookup;
  typedef rtree<Box, Linestring> SpatialIndex;


  VertexComponentLookup vc_lookup;
  EdgeComponentLookup ec_lookup;
  PointIndex pointIndex;
  BoxLinestringLookup boxLsLookup;
  SpatialIndex spatialIndex;

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
void put_point_index(Graph& graph, typename Indices<Graph>::PointIndex& pointIndex) {
  Indices<Graph>::getIndices(graph).pointIndex = pointIndex;
}

template<typename Graph>
void put_vertex_component_lookup(Graph& graph, typename Indices<Graph>::VertexComponentLookup& vc_lookup) {
  Indices<Graph>::getIndices(graph).vc_lookup = vc_lookup;
}

template<typename Graph>
void put_edge_component_lookup(Graph& graph, typename Indices<Graph>::EdgeComponentLookup& ec_lookup) {
  Indices<Graph>::getIndices(graph).ec_lookup = ec_lookup;
}

template<typename Graph>
typename Indices<Graph>::PointLookup& get_point_index(Graph& graph) {
  typename Indices<Graph>::PointIndex& pointIndex = Indices<Graph>::getIndices(graph).pointIndex;
  assert(pointIndex != NULL);
  return pointIndex;
}

template<typename Graph>
typename Indices<Graph>::BoxLinestringLookup& get_box_linestring_lookup(Graph& graph) {
  return Indices<Graph>::getIndices(graph).boxLsLookup;
}

template<typename Graph>
typename Indices<Graph>::SpatialIndex& get_spatial_index(Graph& graph) {
  return Indices<Graph>::getIndices(graph).spatialIndex;
}

template<typename Graph>
typename Indices<Graph>::VertexComponentLookup& get_vertex_component_lookup(Graph& graph) {
  return Indices<Graph>::getIndices(graph).vc_lookup;
}

template<typename Graph>
typename Indices<Graph>::EdgeComponentLookup& get_edge_component_lookup(Graph& graph) {
  return Indices<Graph>::getIndices(graph).ec_lookup;
}

template<typename Graph>
void create_connected_component_lookup(Graph& graph) {
  typename Indices<Graph>::VertexComponentLookup& vc_lookup = get_vertex_component_lookup(graph);
  vc_lookup.first = boost::connected_components(graph, boost::make_assoc_property_map(vc_lookup.second));
  put_vertex_component_lookup(graph, vc_lookup);
}

template<typename Graph>
void create_biconnected_component_lookup(Graph& graph) {
  typename Indices<Graph>::EdgeComponentLookup& ec_lookup = get_edge_component_lookup(graph);
  ec_lookup.first = boost::biconnected_components(graph, boost::make_assoc_property_map(ec_lookup.second));
  put_edge_component_lookup(graph, ec_lookup);
}

template<typename Graph>
void index_linestring(Graph& graph, Linestring& ls) {
  typename Indices<Graph>::BoxLinestringLookup& bsLsLookup = get_box_linestring_lookup(graph);
  typename Indices<Graph>::SpatialIndex& spatialIndex = get_spatial_index(graph);
  Box& box = (* new Box());
  envelope(ls, box);
  bsLsLookup[&box] = ls;
  spatialIndex.insert(box, ls);
}


#endif /* INDICES_H_ */
