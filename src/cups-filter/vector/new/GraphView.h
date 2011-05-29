#ifndef GRAPHVIEW_H_
#define GRAPHVIEW_H_

#include "CutModel.h"
#include "Indices.h"

struct is_component_vertex {
  is_component_vertex() : compLookup(*new VertexComponentLookup()) {}
  is_component_vertex(VertexComponentLookup& compLookup, int componentIndex) : compLookup(compLookup),  componentIndex(componentIndex) {
  }
  bool operator()(const Vertex& v) const {
    return compLookup.second.at(v) == componentIndex;
  }
  VertexComponentLookup compLookup;
  int componentIndex;
};

struct is_component_edge {
  is_component_edge() : compLookup(*new EdgeComponentLookup()) {}
  is_component_edge(EdgeComponentLookup& compLookup, int componentIndex) : compLookup(compLookup),  componentIndex(componentIndex) {
  }
  bool operator()(const UndirectedEdge& e) const {
    map<UndirectedEdge, int>::const_iterator it = compLookup.second.find(e);

    if(it != compLookup.second.end()) {
      std::cerr << ((*it).second == componentIndex) << std::endl;
      return (*it).second == componentIndex;
    }
    else
      return false;
  }
  EdgeComponentLookup compLookup;
  int componentIndex;
};

template<typename EdgePredicate, typename VertexPredicate>
class UDGraphView : public filtered_graph<UndirectedGraph, EdgePredicate, VertexPredicate> {
public:
  UDGraphView(const UndirectedGraph& ug, EdgePredicate epredicate, VertexPredicate vpredicate) : filtered_graph<UndirectedGraph, EdgePredicate, VertexPredicate>(ug, epredicate, vpredicate) {}
};

typedef UDGraphView<is_component_edge,boost::keep_all> EdgeComponentGraphView;
typedef UDGraphView<boost::keep_all, is_component_vertex> VertexComponentGraphView;

inline list<EdgeComponentGraphView>& all_edge_component_views(string name, CutModel& cutModel) {
  list<EdgeComponentGraphView> *cgList =  new list<EdgeComponentGraphView>();
  EdgeComponentLookup& clup = Indices::getIndices(cutModel).getEdgeComponentLookup(name);
  for (int i = 0; i < clup.first; ++i) {
    is_component_edge& efilter = (*new is_component_edge(clup, i));
    boost::keep_all &vfilter = (*new boost::keep_all());
    cgList->push_back((* new EdgeComponentGraphView(cutModel.getUndirectedGraph(), efilter, vfilter)));
  }

  return *cgList;
}

inline list<VertexComponentGraphView>& all_vertex_component_views(string name, CutModel& cutModel) {
  list<VertexComponentGraphView> *cgList =  new list<VertexComponentGraphView>();
  VertexComponentLookup& clup = Indices::getIndices(cutModel).getVertexComponentLookup(name);
  for (int i = 0; i < clup.first; ++i) {
    boost::keep_all efilter = (*new boost::keep_all());
    is_component_vertex vfilter = (*new is_component_vertex(clup, i));
    cgList->push_back((* new VertexComponentGraphView(cutModel.getUndirectedGraph(), efilter, vfilter)));
  }

  return *cgList;
}
#endif /* GRAPHVIEW_H_ */
