#include "CutGraph.h"

#include <iostream>
#include <vector>

using std::vector;

SegmentGraph& SegmentGraph::createSegmentGraph(SegmentList::const_iterator start, SegmentList::const_iterator end) {
  SegmentGraph& graph = *new SegmentGraph();
  const Segment& origin = *new Segment(* new Point(), * new Point(), * new CutSettings(-1,-1,-1));
  const Segment* s_i = NULL;
  const Segment* s_j = NULL;


  for (SegmentList::const_iterator it_i = start; it_i != end; ++it_i) {
    s_i = *it_i;
    for (SegmentList::const_iterator it_j = start; it_j != end; ++it_j) {
      s_j = *it_j;

      if(s_i == s_j)
        continue;

      graph.createEdges(origin, *s_i);
      graph.createEdges(*s_i, *s_j);
    }
  }

  return graph;
}

void SegmentGraph::createEdges(const Segment& seg1, const Segment& seg2) {
  SegmentGraph::Vertex inV = addVertex(seg1);
  SegmentGraph::Vertex outV = addVertex(seg2);

  //FIXME do we want to filter duplicated edges?
  if(boost::edge(inV,outV, *this).second)
    return;

  double w0 = seg1.first.distance(seg2.first);
  double w1 = seg1.first.distance(seg2.second);
  double w2 = seg1.second.distance(seg2.first);
  double w3 = seg1.second.distance(seg2.second);

//  add_edge(inV, outV, EdgeGeomProperty(*new GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(w0))), *this);
  add_edge(inV, outV, EdgeGeomProperty(*new GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(w1))), *this);
//  add_edge(outV, inV, EdgeGeomProperty(*new GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(w2))), *this);
//  add_edge(outV, inV, EdgeGeomProperty(*new GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(w3))), *this);
}

SegmentGraph::Vertex* SegmentGraph::findVertex(const Segment &seg) {
  SegmentVertexMap::const_iterator it = segments.find(seg);
  if (it == segments.end())
    return NULL;
  else
    return (SegmentGraph::Vertex*)&(*it).second;
}

SegmentGraph::Vertex SegmentGraph::addVertex(const Segment &seg) {
  Vertex* v = findVertex(seg);
  if (v == NULL) {
    Vertex new_vertex = add_vertex(* new VertexGeomProperty(* new GeometryMapping(0,&seg,0)), *this);
    segments[seg] = new_vertex;
    return new_vertex;
  }

  return *v;
}

