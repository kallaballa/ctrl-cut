#ifndef GRAPHVIEW_H_
#define GRAPHVIEW_H_

#include "CutModel.h"
#include "Indices.h"

struct all_vertices_predicate {
  all_vertices_predicate() {};
  bool operator()(const Vertex& v) const {
    return true;
  }
};

struct all_edges_predicate {
  all_edges_predicate() {};
  bool operator()(const Edge& e) const {
    return true;
  }
};

struct is_component_vertex {
  is_component_vertex() : compLookup(*new ComponentLookup()) {}
  is_component_vertex(ComponentLookup& compLookup, int componentIndex) : compLookup(compLookup),  componentIndex(componentIndex) {
  }
  bool operator()(const Vertex& v) const {
    return compLookup.second.at(v) == componentIndex;
  }
  ComponentLookup compLookup;
  int componentIndex;
};

typedef filtered_graph<Graph, all_edges_predicate, is_component_vertex> GraphView;

GraphView& component_view(CutModel& cutModel, ComponentLookup& compLookup, int componentIndex);
list<GraphView>& all_component_views(string name, CutModel& cutModel);
#endif /* GRAPHVIEW_H_ */
