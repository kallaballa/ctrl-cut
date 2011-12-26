#include "StringGraph.h"

#include <iostream>
#include <vector>

StringGraph::Vertex create_complete_graph_from_point(StringGraph& graph, const Point& origin, Route::StringIter start, Route::StringIter end) {
  StringGraph::Vertex v_origin = graph.addVertex(origin);

  for(Route::StringIter it_si = start; it_si != end; ++it_si) {
    SegmentString& s_i = *it_si;
    for (Route::StringIter it_sj = start; it_sj != end; ++it_sj) {
      SegmentString& s_j = *it_sj;
      vector<StringGraph::Vertex> siVertices;
      vector<StringGraph::Vertex> sjVertices;

      graph.permutateEdges(s_i, v_origin, siVertices);
      if (it_si != it_sj) {
        graph.permutateEdges(s_j, v_origin, sjVertices);

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

void StringGraph::createWorkEdge(const Vertex& in, const Vertex& out) {
  if(in == out || hasEdge(in, out))
    return;

//  std::cerr << in << " " << out << std::endl;
  add_edge(in, out, StringProperty(), *this);
}

void StringGraph::createMoveEdge(const Vertex& in, const Vertex& out) {
  if(in == out || hasEdge(in, out))
    return;
  const Point& p_in = (*this)[in];
  const Point& p_out = (*this)[out];
//  std::cerr << in << " " << out << std::endl;
  add_edge(in, out, StringProperty(p_in.distance(p_out)), *this);
}

bool StringGraph::hasEdge(const Vertex& in, const Vertex& out) {
  return boost::edge(in, out, *this).second;
}

void StringGraph::permutateEdges(SegmentString& string, Vertex v_origin,
    vector<Vertex>& outVertices) {
//  std::cerr << "permutate" << std::endl;

  StringGraph::Vertex frontV = addVertex(string.frontPoints());
  StringGraph::Vertex backV = addVertex(string.backPoints());
  outVertices.push_back(frontV);
  outVertices.push_back(backV);

  createWorkEdge(frontV, backV);
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


bool StringGraph::findVertex(Vertex& v, const Point& point) {
  PointMap::const_iterator it = points.find(point);
  if (it == points.end())
    return false;

  v = (*it).second;
  return true;
}

StringGraph::Vertex StringGraph::addVertex(const Point& p) {
  Vertex v;
  if (findVertex(v, p)) {
    Vertex new_vertex = add_vertex(p, *this);
    points[p] = new_vertex;
    return new_vertex;
  }

  return v;
}
