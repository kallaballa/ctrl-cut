
#ifndef LOOKUP_H_
#define LOOKUP_H_


template<typename Graph>
void put_vertex_component_lookup(Graph& graph, typename Index<Graph>::VertexComponentLookup& vc_lookup) {
  Index<Graph>::getIndices(graph).vc_lookup = vc_lookup;
}

template<typename Graph>
void put_edge_component_lookup(Graph& graph, typename Index<Graph>::EdgeComponentLookup& ec_lookup) {
  Index<Graph>::getIndices(graph).ec_lookup = ec_lookup;
}

template<typename Graph>
typename Index<Graph>::VertexComponentLookup& get_vertex_component_lookup(Graph& graph) {
  return Index<Graph>::getIndices(graph).vc_lookup;
}

template<typename Graph>
typename Index<Graph>::EdgeComponentLookup& get_edge_component_lookup(Graph& graph) {
  return Index<Graph>::getIndices(graph).ec_lookup;
}

template<typename Graph>
typename Index<Graph>::VertexComponentLookup& create_connected_component_lookup(Graph& graph) {
  typename Index<Graph>::VertexComponentLookup& vc_lookup = get_vertex_component_lookup(graph);
  vc_lookup.first = boost::connected_components(graph, boost::make_assoc_property_map(vc_lookup.second));
  return vc_lookup;
}

template<typename Graph>
void create_biconnected_component_lookup(Graph& graph) {
  typename Index<Graph>::EdgeComponentLookup& ec_lookup = get_edge_component_lookup(graph);
  ec_lookup.first = boost::biconnected_components(graph, boost::make_assoc_property_map(ec_lookup.second));
  return vc_lookup;
}


#endif /* LOOKUP_H_ */
