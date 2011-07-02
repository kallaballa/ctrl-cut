#include "StringGraph.h"

#include <iostream>
#include <vector>

StringGraph::Vertex create_complete_graph_from_point(StringGraph& graph, const Point& origin, StringList::const_iterator start, StringList::const_iterator end) {
  const SegmentString* s_i = NULL;
  const SegmentString* s_j = NULL;
  StringGraph::Vertex v_origin = graph.addVertex(&origin);

  for(StringList::const_iterator it_si = start; it_si != end; ++it_si) {
    s_i = *it_si;
    for (StringList::const_iterator it_sj = start; it_sj != end; ++it_sj) {
      s_j = *it_sj;
      vector<StringGraph::Vertex> siVertices;
      vector<StringGraph::Vertex> sjVertices;

      graph.permutateEdges(*s_i, v_origin, siVertices);
      if (s_i != s_j) {
        graph.permutateEdges(*s_j, v_origin, sjVertices);

        for (vector<StringGraph::Vertex>::iterator it_vi = siVertices.begin(); it_vi
            != siVertices.end(); ++it_vi) {
          for (vector<StringGraph::Vertex>::iterator it_vj = sjVertices.begin(); it_vj
              != sjVertices.end(); ++it_vj) {
            const StringGraph::Vertex vi = *it_vi;
            const StringGraph::Vertex vj = *it_vj;
            graph.createMoveEdge(vi, vj);
          }
        }
      }
    }
  }

  return v_origin;
}

void StringGraph::createWorkEdge(const Vertex& in, const Vertex& out, const SegmentString* owner) {
  if(in == out || hasEdge(in, out))
    return;

  std::cerr << in << " " << out << std::endl;
  add_edge(in, out, StringProperty(owner), *this);
}

void StringGraph::createMoveEdge(const Vertex& in, const Vertex& out) {
  if(in == out || hasEdge(in, out))
    return;
  const Point* p_in = (*this)[in].point;
  const Point* p_out = (*this)[out].point;
  StringProperty map(0, p_in->distance(*p_out));
  std::cerr << in << " " << out << std::endl;
  add_edge(in, out, map, *this);
}

bool StringGraph::hasEdge(const Vertex& in, const Vertex& out) {
  return boost::edge(in, out, *this).second;
}

void StringGraph::permutateEdges(const SegmentString& string, Vertex v_origin,
    vector<Vertex>& outVertices) {
  std::cerr << "permutate" << std::endl;

  StringGraph::Vertex frontV = addVertex(string.frontPoints(), &string);
  StringGraph::Vertex backV = addVertex(string.backPoints(), &string);
  outVertices.push_back(frontV);
  outVertices.push_back(backV);

  createWorkEdge(frontV, backV, &string);
  createMoveEdge(frontV, v_origin);
  createMoveEdge(backV, v_origin);

  for (vector<Vertex>::iterator it_i = outVertices.begin(); it_i
      != outVertices.end(); ++it_i) {
    for (vector<Vertex>::iterator it_j = outVertices.begin(); it_j
        != outVertices.end(); ++it_j) {
      createMoveEdge(*it_i, *it_j);
    }
  }
}


StringGraph::Vertex* StringGraph::findVertex(const TieProperty &map) {
  TieMap::const_iterator it = tieMap.find(map);
  if (it == tieMap.end())
    return NULL;
  else
    return (StringGraph::Vertex*)&(*it).second;
}

StringGraph::Vertex StringGraph::addVertex(const Point* p, const SegmentString* owner) {
  TieProperty map(p,owner);
  Vertex* v = findVertex(map);
  if (v == NULL) {
    Vertex new_vertex = add_vertex(map, *this);
    tieMap[map] = new_vertex;
    return new_vertex;
  }

  return *v;
}
