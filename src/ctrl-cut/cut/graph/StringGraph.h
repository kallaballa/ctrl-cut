#ifndef STRINGGRAPH_H_
#define STRINGGRAPH_H_

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

struct StringProperty {
  const SegmentString* string;
  double weight;

  StringProperty() : string(0), weight(0) {}
  StringProperty(const SegmentString* string, double weight=0) : string(string), weight(weight) {}

  bool operator<(const StringProperty& other) const {
    return this->string < other.string;
  }
};

struct TieProperty {
  const Point* point;
  const SegmentString* owner;

  TieProperty() : point(0), owner(0) {}
  TieProperty(const Point* p, const SegmentString* string) : point(p), owner(string) {}

  bool operator<(const TieProperty& other) const {
    const Point* p1 = this->point;
    const Point* p2 = other.point;
    const SegmentString* string1 = this->owner;
    const SegmentString* string2 = other.owner;

    bool pLess;

    if(p1 != NULL && p2 != NULL) {
      if(string1 != NULL && string1 == string2 && string1->isClosed() && *p1 == *p2) {
        pLess = p1 < p2;
      } else {
        pLess = *p1 < *p2;
      }
    } else {
      pLess = p1 < p2;
    }

    return string1 < string2 || (string1 == string2 && pLess);
  }
};

class StringGraph : public adjacency_list<vecS, vecS, undirectedS, TieProperty, StringProperty> {
public:

  boost::graph_traits<StringGraph>::edges_size_type edge_count;

  typedef boost::graph_traits<StringGraph>
    ::vertex_descriptor Vertex;
  typedef boost::graph_traits<StringGraph>
    ::edge_descriptor Edge;
  typedef boost::graph_traits<StringGraph>
      ::vertices_size_type v_size;
    typedef std::vector<std::vector< StringGraph::Edge > > Embedding;

  typedef map<const TieProperty, Vertex> TieMap;

  StringGraph() : adjacency_list<vecS, vecS, undirectedS, TieProperty, StringProperty>() , edge_count(0){}
  StringGraph(const StringGraph& graph) : adjacency_list<vecS, vecS, undirectedS, TieProperty, StringProperty>(graph) , edge_count(0) {}
  StringGraph(v_size size) : adjacency_list<vecS, vecS, undirectedS, TieProperty, StringProperty>(size) , edge_count(0){}

  StringGraph::Vertex* findVertex(const TieProperty &map);
  StringGraph::Vertex addVertex(const Point* p, const SegmentString* owner = 0);
  void createWorkEdge(const Vertex& in, const Vertex& out, const SegmentString* owner);
  void createMoveEdge(const Vertex& in, const Vertex& out);
  bool hasEdge(const Vertex& in, const Vertex& out);
  void permutateEdges(const SegmentString& string, Vertex v_origin, vector<Vertex>& outVertices);
private:
  TieMap tieMap;
};

StringGraph::Vertex create_complete_graph_from_point(StringGraph& graph, const Point& origin, StringList::const_iterator start, StringList::const_iterator end);

#endif
