#ifndef GRAPHVIEW_H_
#define GRAPHVIEW_H_

#include <list>
#include "CutModel.h"
#include "Indices.h"

using std::list;

struct is_free_edge {
  UndirectedGraph* udGraph;

  is_free_edge() : udGraph(new UndirectedGraph()) {}
  is_free_edge(UndirectedGraph& udGraph) : udGraph(&udGraph) {}

  bool operator()(const Edge& e) const {
    return get(edge_geom, *udGraph)[e] == NULL;
  }
};

struct is_component_vertex {
  VertexComponentLookup* compLookup;
  int componentIndex;

  is_component_vertex() : compLookup(new VertexComponentLookup()) {}
  is_component_vertex(VertexComponentLookup& compLookup, int componentIndex) : compLookup(&compLookup),  componentIndex(componentIndex) {
  }
  bool operator()(const Vertex& v) const {
    return compLookup->second.at(v) == componentIndex;
  }
};

struct is_component_edge {
  UndirectedGraph* udGraph;
  EdgeComponentLookup* edgeCompMap;
  int componentIndex;
  is_free_edge free_predicate;

  is_component_edge() : udGraph(new UndirectedGraph()), compLookup(new EdgeComponentMap()), free_predicate(*udGraph) {}
  is_component_edge(UndirectedGraph& udGraph, EdgeComponentMap& edgeCompMap, int componentIndex) : udGraph(&udGraph), edgeCompMap(&edgeCompMap),  componentIndex(componentIndex), free_predicate(udGraph) {
  }
  bool operator()(const Edge& e) const {
    if(free_predicate(e)) {
      EdgeComponentMap::const_iterator it = edgeCompMap.find(e);
      return  it != edgeCompMap.end() && (*it).second == componentIndex;
    }
  }
};

template<typename EdgePredicate, typename VertexPredicate>
class UDGraphView : public filtered_graph<UndirectedGraph, EdgePredicate, VertexPredicate> {
public:
  UDGraphView(const UndirectedGraph& ug, EdgePredicate epredicate, VertexPredicate vpredicate) : filtered_graph<UndirectedGraph, EdgePredicate, VertexPredicate>(ug, epredicate, vpredicate) {}
};

typedef UDGraphView<is_component_edge,boost::keep_all> EdgeComponentGraphView;
typedef UDGraphView<is_free_edge, is_component_vertex> VertexComponentGraphView;

inline list<EdgeComponentGraphView>& all_edge_component_views(string name, CutModel& cutModel) {
  list<EdgeComponentGraphView> *cgList =  new list<EdgeComponentGraphView>();
  EdgeComponentLookup& clup = Indices::getIndices(cutModel).getEdgeComponentLookup(name);
  for (int i = 0; i < clup.first; ++i) {
    is_component_edge& efilter = (*new is_component_edge(cutModel.getUndirectedGraph(), clup, i));
    boost::keep_all &vfilter = (*new boost::keep_all());
    cgList->push_back((* new EdgeComponentGraphView(cutModel.getUndirectedGraph(), efilter, vfilter)));
  }

  return *cgList;
}

inline list<VertexComponentGraphView>& all_vertex_component_views(string name, CutModel& cutModel) {
  list<VertexComponentGraphView> *cgList =  new list<VertexComponentGraphView>();
  VertexComponentLookup& clup = Indices::getIndices(cutModel).getVertexComponentLookup(name);
  for (int i = 0; i < clup.first; ++i) {
    is_free_edge efilter = (*new is_free_edge(cutModel.getUndirectedGraph()));
    is_component_vertex vfilter = (*new is_component_vertex(clup, i));
    cgList->push_back((* new VertexComponentGraphView(cutModel.getUndirectedGraph(), efilter, vfilter)));
  }

  return *cgList;
}


#endif /* GRAPHVIEW_H_ */
