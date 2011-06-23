#include "CutGraph.h"

#include <iostream>
#include <vector>

void create_segment_graph(CutGraph& graph, SegmentList::const_iterator start, SegmentList::const_iterator end) {
    for (SegmentList::const_iterator it = start; it != end; ++it)
     graph.createEdge(*(*it));
}

CutGraph::Vertex create_complete_graph_from_point(CutGraph& graph, const Point& origin, StringList::const_iterator start, StringList::const_iterator end) {
  const SegmentString* s_i = NULL;
  const SegmentString* s_j = NULL;
  CutGraph::Vertex v_origin = graph.addVertex(&origin);

  for(StringList::const_iterator it_si = start; it_si != end; ++it_si) {
    s_i = *it_si;
    for (StringList::const_iterator it_sj = start; it_sj != end; ++it_sj) {
      s_j = *it_sj;
      vector<CutGraph::Vertex> siVertices;
      vector<CutGraph::Vertex> sjVertices;

      graph.permutateEdges(*s_i, v_origin, siVertices);
      if (s_i != s_j) {
        graph.permutateEdges(*s_j, v_origin, sjVertices);

        for (vector<CutGraph::Vertex>::iterator it_vi = siVertices.begin(); it_vi
            != siVertices.end(); ++it_vi) {
          for (vector<CutGraph::Vertex>::iterator it_vj = sjVertices.begin(); it_vj
              != sjVertices.end(); ++it_vj) {
            const CutGraph::Vertex vi = *it_vi;
            const CutGraph::Vertex vj = *it_vj;
            graph.createMoveEdge(vi, vj);
          }
        }
      }
    }
  }

  return v_origin;
}

void CutGraph::createWorkEdge(const Vertex& in, const Vertex& out, const SegmentString* owner) {
  if(in == out || hasEdge(in, out))
    return;

  std::cerr << in << " " << out << std::endl;
  add_edge(in, out, EdgeGeometry(0, owner), *this);
}

void CutGraph::createMoveEdge(const Vertex& in, const Vertex& out) {
  if(in == out || hasEdge(in, out))
    return;
  const Point* p_in = this->getPoint(in);
  const Point* p_out = this->getPoint(out);
  EdgeGeometry map(0,0, p_in->distance(*p_out));
  std::cerr << in << " " << out << std::endl;
  add_edge(in, out, map, *this);
}

bool CutGraph::hasEdge(const Vertex& in, const Vertex& out) {
  return boost::edge(in, out, *this).second;
}

void CutGraph::permutateEdges(const SegmentString& string, Vertex v_origin,
    vector<Vertex>& outVertices) {
  std::cerr << "permutate" << std::endl;

  CutGraph::Vertex frontV = addVertex(string.frontPoints(), &string);
  CutGraph::Vertex backV = addVertex(string.backPoints(), &string);
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

void CutGraph::createEdge(const Segment& seg) {

  CutGraph::Vertex inV = addVertex(&seg.first);
  CutGraph::Vertex outV = addVertex(&seg.second);

  if(hasEdge(inV,outV))
    return;

  double weight = seg.first.distance(seg.second);

  add_edge(inV, outV, EdgeGeometry(&seg, 0, weight), *this);
}


CutGraph::Vertex* CutGraph::findVertex(const VertexGeometry &map) {
  GeomVertexMap::const_iterator it = geometries.find(map);
  if (it == geometries.end())
    return NULL;
  else
    return (CutGraph::Vertex*)&(*it).second;
}

CutGraph::Vertex CutGraph::addVertex(const Point* p, const SegmentString* owner) {
  VertexGeometry map(p,owner);
  Vertex* v = findVertex(map);
  if (v == NULL) {
    Vertex new_vertex = add_vertex(map, *this);
    geometries[map] = new_vertex;
    return new_vertex;
  }

  return *v;
}
