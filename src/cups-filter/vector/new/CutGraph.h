#ifndef CUTGRAPH_H_
#define CUTGRAPH_H_

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <map>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/multi/multi.hpp>
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
using boost::tuple;
using std::vector;
using std::map;
using std::pair;

typedef adjacency_list<setS, vecS, directedS> Graph;

typedef typename boost::graph_traits<Graph>
  ::vertex_descriptor Vertex;

typedef typename boost::graph_traits<Graph>
  ::edge_descriptor Edge;

typedef Graph::vertex_iterator VertexIterator;

struct ConnectedIndex {
  map<Vertex, int> component;
  vector<Vertex> root;
};

struct GeomIndex {
  map<CutPoint2D, Vertex, boost::geometry::less<CutPoint2D> > points;
};

class CutGraph {
private:
  Vertex find_or_insert(CutPoint2D const&point);

  Graph graph;
  ConnectedIndex connIndex;
  GeomIndex geomIndex;
public:
  CutGraph();
  virtual ~CutGraph();


  void createEdge(CutPoint2D& in, CutPoint2D& out, LaserSettings& settings);
  void createEdge(uint32_t inX, uint32_t inY, uint32_t outX, uint32_t outY, int power, int speed, int frequency);
  void buildConnectedIndex();

  uint32_t getNumVertices() { return num_vertices(this->graph); }
  pair<VertexIterator, VertexIterator> getVertexIteratorPair() { return vertices(this->graph); }
  const ConnectedIndex getConnectedIndex() { return connIndex; }
  const GeomIndex getGeomIndex() { return geomIndex; }
};

#endif
