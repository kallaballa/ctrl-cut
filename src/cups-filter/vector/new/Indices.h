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
  inline bool operator()(Edge const& left, Edge const& right) const {
    return left.m_source < right.m_source ||
        ((left.m_source == right.m_source) &&  (left.m_target < right.m_target));
  }
};

typedef map<Vertex, int> VertexComponentMap;
typedef map<Edge, int, compare_edge_vertices> EdgeComponentMap;

// numComponents, <vertex, componentIndex>
typedef pair<int,  VertexComponentMap > VertexComponentLookup;
// numComponents, <edge, componentIndex>
typedef pair<int,  EdgeComponentMap > EdgeComponentLookup;

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
  map<string, VertexComponentLookup*> vertexComponentLookUpMap;
  map<string, EdgeComponentLookup*> edgeComponentLookUpMap;

  static map<CutModel*, Indices*> singletons;

  Indices(CutModel& model) : cutModel(model){}
public:
  static Indices& getIndices(CutModel& model);
  virtual ~Indices() {}

  VertexComponentLookup& createVertexComponentLookup(string name, VertexComponentLookup& (*build_comp_lookup)(CutModel& cutModel), IndexLifetime& lifetime = (* new IndexLifetime())) {
    VertexComponentLookup& compLookup = build_comp_lookup(this->cutModel);
    this->vertexComponentLookUpMap[name] = &compLookup;
    return compLookup;
  }

  VertexComponentLookup& getVertexComponentLookup(string name) {
    return *vertexComponentLookUpMap[name];
  }

  EdgeComponentLookup& createEdgeComponentLookup(string name, EdgeComponentLookup& (*build_comp_lookup)(CutModel& cutModel), IndexLifetime& lifetime = (* new IndexLifetime())) {
    EdgeComponentLookup& compLookup = build_comp_lookup(this->cutModel);
    this->edgeComponentLookUpMap[name] = &compLookup;
    return compLookup;
  }

  EdgeComponentLookup& getEdgeComponentLookup(string name) {
    return *edgeComponentLookUpMap[name];
  }
};

//ComponentLookup& strong_componentlookup(CutModel& cutModel);
VertexComponentLookup& connected_componentlookup(CutModel& cutModel);
EdgeComponentLookup& biconnected_componentlookup(CutModel& cutModel);

#endif /* INDICES_H_ */
