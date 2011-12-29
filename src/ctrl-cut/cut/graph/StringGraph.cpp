#include "StringGraph.h"

#include <iostream>
#include <vector>

DistanceGraph::Vertex create_complete_graph_from_point(DistanceGraph& graph, const Point& origin, Route::iterator start, Route::iterator end) {
  DistanceGraph::Vertex v_origin = graph.addVertex(origin);

  for(Route::iterator it_si = start; it_si != end; ++it_si) {
    LineString& s_i = *it_si;
    for (Route::iterator it_sj = start; it_sj != end; ++it_sj) {
      LineString& s_j = *it_sj;
      vector<DistanceGraph::Vertex> siVertices;
      vector<DistanceGraph::Vertex> sjVertices;

      graph.permutateEdges(s_i.front(),s_i.back(), v_origin, siVertices);
      if (it_si != it_sj) {
        graph.permutateEdges(s_j.front(),s_j.back(), v_origin, sjVertices);

        for (vector<DistanceGraph::Vertex>::iterator it_vi = siVertices.begin(); it_vi
            != siVertices.end(); ++it_vi) {
          for (vector<DistanceGraph::Vertex>::iterator it_vj = sjVertices.begin(); it_vj
              != sjVertices.end(); ++it_vj) {
            const DistanceGraph::Vertex vi = *it_vi;
            const DistanceGraph::Vertex vj = *it_vj;
            graph.createMoveEdge(vi, vj);
          }
        }
      }
    }
  }

  return v_origin;
}

void DistanceGraph::createWorkEdge(const Vertex& in, const Vertex& out) {
  if(in == out || hasEdge(in, out))
    return;

//  std::cerr << in << " " << out << std::endl;
  add_edge(in, out, WeightProperty(), *this);
}

void DistanceGraph::createMoveEdge(const Vertex& in, const Vertex& out) {
  if(in == out || hasEdge(in, out))
    return;
  const Point& p_in = (*this)[in];
  const Point& p_out = (*this)[out];
//  std::cerr << in << " " << out << std::endl;
  add_edge(in, out, WeightProperty(p_in.distance(p_out)), *this);
}

bool DistanceGraph::hasEdge(const Vertex& in, const Vertex& out) {
  return boost::edge(in, out, *this).second;
}

void DistanceGraph::permutateEdges(const Point& front, const Point& back, Vertex v_origin,
    vector<Vertex>& outVertices) {
//  std::cerr << "permutate" << std::endl;

  DistanceGraph::Vertex frontV = addVertex(front);
  DistanceGraph::Vertex backV = addVertex(back);
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


bool DistanceGraph::findVertex(Vertex& v, const Point& point) {
  PointMap::const_iterator it = points.find(point);
  if (it == points.end())
    return false;

  v = (*it).second;
  return true;
}

DistanceGraph::Vertex DistanceGraph::addVertex(const Point& p) {
  Vertex v;
  if (findVertex(v, p)) {
    Vertex new_vertex = add_vertex(p, *this);
    points[p] = new_vertex;
    return new_vertex;
  }

  return v;
}
