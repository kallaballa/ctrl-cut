#include "Indices.h"


map<CutModel*, Indices*> Indices::singletons = (* new map<CutModel*, Indices*>());

Indices& Indices::getIndices(CutModel& model) {
  CutModel *key = &model;
  map<CutModel*, Indices*>::iterator it = Indices::singletons.find(key);

  if(it == Indices::singletons.end()) {
    Indices *indices = new Indices(model);
    Indices::singletons[key] = indices;
    return *indices;
  }

  return *(*it).second;
}

/*ComponentLookup& strong_componentlookup(CutModel& cutModel) {
  ComponentLookup *clup = new ComponentLookup();
  clup->first = boost::strong_components(cutModel.getUndirectedGraph(), boost::make_assoc_property_map(clup->second));
  return *clup;
}*/

VertexComponentLookup& connected_componentlookup(CutModel& cutModel) {
  VertexComponentLookup *clup = new VertexComponentLookup();
  clup->first = boost::connected_components(cutModel.getUndirectedGraph(), boost::make_assoc_property_map(clup->second));
  return *clup;
}

EdgeComponentLookup& biconnected_componentlookup(CutModel& cutModel) {
  EdgeComponentLookup *clup = new EdgeComponentLookup();
  clup->first = boost::biconnected_components(cutModel.getUndirectedGraph(), boost::make_assoc_property_map(clup->second));
  return *clup;
}
