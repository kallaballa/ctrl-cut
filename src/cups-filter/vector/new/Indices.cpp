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

ComponentLookup& strong_componentlookup(CutModel& cutModel) {
  ComponentLookup *clup = new ComponentLookup();
  clup->first = boost::strong_components(cutModel.getGraph(), boost::make_assoc_property_map(clup->second));
  return *clup;
}

ComponentLookup& connected_componentlookup(CutModel& cutModel) {
  ComponentLookup *clup = new ComponentLookup();
  clup->first = boost::connected_components(cutModel.getGraph(), boost::make_assoc_property_map(clup->second));
  return *clup;
}

