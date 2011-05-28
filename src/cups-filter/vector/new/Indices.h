#ifndef INDICES_H_
#define INDICES_H_

#include <string>
#include <map>
#include "CutModel.h"

using std::map;
using std::pair;
using std::string;

// numComponents, <vertex, componentIndex>
typedef pair<int,  map<Vertex, int> > ComponentLookup;

struct IndexLifetime {
  // delete on create request on an existing index
  bool recreate_index;
  // delete when the CutModel graph changes
  bool modify_graph;
  // delete when the CutModel geometry changes
  bool modify_geometry;

  IndexLifetime(bool recreate_index = true, bool modify_graph = true, bool modify_geometry = true) :
    recreate_index(recreate_index), modify_graph(modify_graph), modify_geometry(modify_geometry)
  {}
};

class Indices {
private:
  CutModel cutModel;
  map<string, ComponentLookup*> compLookUpMap;
  static map<CutModel*, Indices*> singletons;

  Indices(CutModel& model) : cutModel(model){}
public:
  static Indices& getIndices(CutModel& model);
  virtual ~Indices() {}

  ComponentLookup& createComponentLookup(string name, ComponentLookup& (*build_comp_lookup)(CutModel& cutModel), IndexLifetime& lifetime = (* new IndexLifetime())) {
    ComponentLookup& compLookup = build_comp_lookup(this->cutModel);
    this->compLookUpMap[name] = &compLookup;
    return compLookup;
  }

  ComponentLookup& getComponentLookup(string name) {
    return *compLookUpMap[name];
  }
};


ComponentLookup& strong_componentlookup(CutModel& cutModel);
ComponentLookup& connected_componentlookup(CutModel& cutModel);

#endif /* INDICES_H_ */
