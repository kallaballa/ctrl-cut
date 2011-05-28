#ifndef CUTGRAPH_H_
#define CUTGRAPH_H_

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <map>
#include <list>
#include <string>

#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/strong_components.hpp>

#include <boost/geometry/geometry.hpp>
#include <boost/geometry/multi/multi.hpp>
#include <boost/geometry/strategies/cartesian/distance_pythagoras.hpp>

#include <boost/property_map/vector_property_map.hpp>

#include "CutEdge.h"
#include "CutVertex.h"

using boost::adjacency_list;
using boost::directedS;
using boost::setS;
using boost::vecS;
using boost::add_vertex;
using boost::add_edge;
using boost::num_vertices;
using boost::default_color_type;
using boost::filtered_graph;
using boost::no_property;
using boost::property_map;
using std::string;
using std::vector;
using std::map;
using std::pair;
using std::list;

typedef adjacency_list<vecS, vecS, directedS, PointProperty, CutProperty> Graph;

typedef typename boost::graph_traits<Graph>
  ::vertex_descriptor Vertex;

typedef typename boost::graph_traits<Graph>
  ::edge_descriptor Edge;

typedef map<CutPoint2D, Vertex, boost::geometry::less<CutPoint2D> > PointLookup;

class CutModel {
private:
  Graph graph;
  PointLookup pointLookup;
public:
  CutModel() {}
  virtual ~CutModel() {}

  double measure(CutSegment2D &segm);
  void createEdge(CutSegment2D &segm, LaserSettings& settings);
  void createEdge(CutPoint2D &in, CutPoint2D &out, LaserSettings& settings);
  void createEdge(uint32_t inX, uint32_t inY, uint32_t outX, uint32_t outY, LaserSettings& settings);
  Vertex findOrInsertVertex(const CutPoint2D &point);
  uint32_t getNumVertices() { return num_vertices(this->graph); }
  Graph& getGraph() { return this->graph; }
  PointLookup& getPointLookup(){ return this->pointLookup; };
};


inline std::ostream& operator<<(std::ostream &os, const CutPoint2D &p) {
  os << "{" << p.get<0>() << "," << p.get<1>() << "}";
  return os;
}

inline std::ostream& operator<<(std::ostream &os, const CutSegment2D &segm) {
  os << "{" << segm.first << "," << segm.second << "}";
  return os;
}

#endif
