#ifndef SEGMENTGRAPH_H_
#define SEGMENTGRAPH_H_

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <map>
#include <string>

#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "cut/geom/Geometry.h"

using boost::adjacency_list;
using boost::undirectedS;
using boost::vecS;
using boost::add_vertex;
using boost::add_edge;
using std::vector;
using std::map;
using std::pair;

struct SegmentProperty {
  Segment* segment;
  SegmentString* owner;

  SegmentProperty() : segment(0), owner(0) {}
  SegmentProperty(Segment* seg, SegmentString* string) : segment(seg), owner(string) {}

  bool operator<(const SegmentProperty& other) const {
    Segment* const seg1 = this->segment;
    Segment* const seg2 = other.segment;
    const SegmentString* string1 = this->owner;
    const SegmentString* string2 = other.owner;

    return string1 < string2 || (string1 == string2 && seg1 < seg2);
  }
};

struct PointProperty  {
  Point* point;

  PointProperty () : point(0) {}
  PointProperty (Point* const  p) : point(p) {}

  bool operator<(const PointProperty & other) const {
    return *this->point < *other.point;
  }
};
class SegmentGraph : public adjacency_list<vecS, vecS, undirectedS, PointProperty , SegmentProperty> {
public:


  boost::graph_traits<SegmentGraph>::edges_size_type edge_count;

  typedef boost::graph_traits<SegmentGraph>
    ::vertex_descriptor Vertex;
  typedef boost::graph_traits<SegmentGraph>
    ::edge_descriptor Edge;
  typedef boost::graph_traits<SegmentGraph>
      ::vertices_size_type v_size;
  typedef std::vector<std::vector< SegmentGraph::Edge > > Embedding;

  typedef map<const PointProperty , Vertex> PointMap;

  SegmentGraph() : adjacency_list<vecS, vecS, undirectedS, PointProperty , SegmentProperty>() , edge_count(0){}
  SegmentGraph(const SegmentGraph& graph) : adjacency_list<vecS, vecS, undirectedS, PointProperty , SegmentProperty>(graph) , edge_count(0) {}
  SegmentGraph(v_size size) : adjacency_list<vecS, vecS, undirectedS, PointProperty , SegmentProperty>(size) , edge_count(0){}

  SegmentGraph::Vertex* findVertex(Point&  p);
  SegmentGraph::Vertex addVertex(Point&  p);
  const PointMap& getPointMap() {
    return points;
  }

  void createEdge(Segment& seg);
  bool hasEdge(const Vertex& in, const Vertex& out);
private:
  PointMap points;
};

void create_planar_graph(SegmentGraph& graph, SegmentList::const_iterator start, SegmentList::const_iterator end);

#endif
