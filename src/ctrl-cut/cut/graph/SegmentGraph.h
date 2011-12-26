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

#include "cut/geom/Segment.h"

using boost::adjacency_list;
using boost::undirectedS;
using boost::vecS;
using boost::add_vertex;
using boost::add_edge;
using std::vector;
using std::map;
using std::pair;

class SegmentGraph : public adjacency_list<vecS, vecS, undirectedS, Point, Segment> {
public:
  boost::graph_traits<SegmentGraph>::edges_size_type edge_count;

  typedef boost::graph_traits<SegmentGraph>
    ::vertex_descriptor Vertex;
  typedef boost::graph_traits<SegmentGraph>
    ::edge_descriptor Edge;
  typedef boost::graph_traits<SegmentGraph>
      ::vertices_size_type v_size;
  typedef std::vector<std::vector< SegmentGraph::Edge > > Embedding;

  typedef map<const Point , Vertex> PointMap;

  SegmentGraph() : adjacency_list<vecS, vecS, undirectedS, Point, Segment>() , edge_count(0){}
  SegmentGraph(const SegmentGraph& graph) : adjacency_list<vecS, vecS, undirectedS, Point, Segment>(graph) , edge_count(0) {}
  SegmentGraph(v_size size) : adjacency_list<vecS, vecS, undirectedS, Point, Segment>(size) , edge_count(0){}

  bool findVertex(Vertex& v, const Point&  p);
  SegmentGraph::Vertex addVertex(const Point&  p);
  const PointMap& getPointMap() {
    return points;
  }

  void createEdge(const Segment& seg);
  bool hasEdge(const Vertex& in, const Vertex& out);
private:
  PointMap points;
};

void create_planar_graph(SegmentGraph& graph, SegmentList::const_iterator start, SegmentList::const_iterator end);

#endif
