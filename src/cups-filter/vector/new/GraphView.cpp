#include "GraphView.h"

GraphView& component_view(CutModel& cutModel, ComponentLookup& compLookup, int componentIndex) {
  return *(new GraphView(cutModel.getGraph(), all_edges_predicate(),  is_component_vertex(compLookup, componentIndex)));
}

list<GraphView>& all_component_views(string name, CutModel& cutModel) {
  list<GraphView> *cgList =  new list<GraphView>();
  ComponentLookup& clup = Indices::getIndices(cutModel).getComponentLookup(name);
  for (int i = 0; i < clup.first; ++i) {
    cgList->push_back(component_view(cutModel, clup, i));
  }

  return *cgList;
}
