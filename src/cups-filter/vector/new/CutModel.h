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

#include <boost/geometry/geometry.hpp>
#include <boost/geometry/multi/multi.hpp>
#include <boost/geometry/strategies/cartesian/distance_pythagoras.hpp>

#include <boost/property_map/vector_property_map.hpp>

#include "CutEdge.h"
#include "CutVertex.h"

using boost::adjacency_list;
using boost::undirectedS;
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

typedef adjacency_list<vecS, vecS, undirectedS, PointProperty, CutProperty> UndirectedGraph;

typedef typename boost::graph_traits<UndirectedGraph>
  ::vertex_descriptor Vertex;

typedef typename boost::graph_traits<UndirectedGraph>
  ::edge_descriptor UndirectedEdge;

typedef map<Point, Vertex, boost::geometry::less<Point> > PointLookup;

class CutModel {
private:
  UndirectedGraph graph;
  PointLookup pointLookup;
public:
  CutModel() {}
  virtual ~CutModel() {}

  void createEdge(Point &in, Point &out, LaserSettings& settings);
  void createEdge(uint32_t inX, uint32_t inY, uint32_t outX, uint32_t outY, LaserSettings& settings);
  Vertex findOrInsertVertex(const Point &point);
  uint32_t getNumVertices() { return num_vertices(this->graph); }
  UndirectedGraph& getUndirectedGraph() { return this->graph; }
  PointLookup& getPointLookup(){ return this->pointLookup; };
};

inline std::ostream& operator<<(std::ostream &os, const Point &p) {
  os << "{" << p.get<0>() << "," << p.get<1>() << "}";
  return os;
}

#endif
