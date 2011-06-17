#include "CutGraph.h"

#include <iostream>
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/graph/graphml.hpp>
#include <boost/property_map/dynamic_property_map.hpp>

using std::vector;

void dump_graph(const CutGraph& graph, const std::string& filename) {
  boost::dynamic_properties dp;
//  dp.property("vertex_geom", get(vertex_geom_t(), graph));
//  dp.property("edge_geom", get(edge_geom_t(), graph));
//  dp.property("weight", get(boost::edge_weight_t(), graph));

  std::ofstream os(filename.c_str(), std::ios_base::out);
  boost::write_graphml(os, graph, dp, true);
  os.close();
}

void create_segment_graph(CutGraph& graph, SegmentList::const_iterator start, SegmentList::const_iterator end) {
    for (SegmentList::const_iterator it = start; it != end; ++it)
     graph.createEdge(*(*it));
}

void create_complete_graph(CutGraph& graph, StringList::const_iterator start, StringList::const_iterator end) {
  const SegmentString* s_i = NULL;
  const SegmentString* s_j = NULL;

  for(StringList::const_iterator it_i = start; it_i != end; ++it_i) {
    s_i = *it_i;
    for (StringList::const_iterator it_j = start; it_j != end; ++it_j) {
      s_j = *it_j;

      graph.createEdges(*s_i, *s_j);
    }
  }
}

CutGraph::Vertex create_complete_graph_from_point(CutGraph& graph, const Point& origin, StringList::const_iterator start, StringList::const_iterator end) {
  const SegmentString* s_i = NULL;
  const SegmentString* s_j = NULL;
  CutGraph::Vertex v_origin = graph.addVertex(GeometryMapping(&origin, 0, 0));

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
            CutGraph::Vertex vi = *it_vi;
            CutGraph::Vertex vj = *it_vj;
            graph.createMetricEdge(vi, vj, GeometryMapping(0, 0, 0));
          }
        }
      }
    }
  }

  return v_origin;
}

void CutGraph::createMetricEdge(const Vertex& in, const Vertex& out, const GeometryMapping& map) {
  if(in == out || hasEdge(in, out))
    return;
  const Point* p_in = this->getPoint(in);
  const Point* p_out = this->getPoint(out);

  double distance = p_in->distance(*p_out);
  std::cerr << in << " " << out << std::endl;
  add_edge(in, out, EdgeGeomProperty(map, IndexProperty(edge_count++, WeightProperty(distance))), *this);
}

bool CutGraph::hasEdge(const Vertex& in, const Vertex& out) {
  return boost::edge(in, out, *this).second;
}

void CutGraph::permutateEdges(const SegmentString& string, Vertex v_origin, vector<Vertex>& outVertices) {
  std::cerr << "permutate" << std::endl;

  /*if(string.isClosed()) {
    for(SegmentString::PointConstIter it = string.beginPoints(); it != string.endPoints(); ++it) {
      CutGraph::Vertex nextV = addVertex(GeometryMapping(*it ,0 ,&string));

      if(!outVertices.empty()) {
        CutGraph::Vertex lastV = outVertices.back();

        if(!hasEdge(lastV, nextV)) {
          std::cerr << lastV << " " << nextV << std::endl;
          add_edge(lastV, nextV, EdgeGeomProperty(GeometryMapping(0, 0, &string), IndexProperty(edge_count++, WeightProperty(0))), *this);
        }
      }

      outVertices.push_back(nextV);
      createMetricEdge(nextV, v_origin, GeometryMapping(0, 0, 0));
    }
    CutGraph::Vertex frontV = outVertices.front();
    CutGraph::Vertex backV = outVertices.back();
    if(!hasEdge(frontV, backV))
      add_edge(frontV, backV, EdgeGeomProperty(GeometryMapping(0, 0, &string), IndexProperty(edge_count++, WeightProperty(0))), *this);
    std::cerr << frontV << " " << backV << std::endl;
  } else {*/
    CutGraph::Vertex frontV = addVertex(GeometryMapping(string.frontPoints(),0 ,&string));
    CutGraph::Vertex backV = addVertex(GeometryMapping(string.backPoints(),0 ,&string));
    outVertices.push_back(frontV);
    outVertices.push_back(backV);

    if(!hasEdge(frontV, backV)) {
      add_edge(frontV, backV, EdgeGeomProperty(GeometryMapping(0, 0, &string), IndexProperty(edge_count++, WeightProperty(0))), *this);
      std::cerr << frontV << " " << backV << std::endl;
    }
    createMetricEdge(frontV, v_origin, GeometryMapping(0, 0, 0));
    createMetricEdge(backV, v_origin, GeometryMapping(0, 0, 0));
  //}

  for(vector<Vertex>::iterator it_i = outVertices.begin(); it_i != outVertices.end(); ++it_i) {
    for(vector<Vertex>::iterator it_j = outVertices.begin(); it_j != outVertices.end(); ++it_j) {
      createMetricEdge(*it_i, *it_j, GeometryMapping(0, 0, 0));
    }
  }
}

void CutGraph::createEdges(const SegmentString& string1, const SegmentString& string2) {
  const Point& s1_inP = *string1.frontPoints();
  const Point& s1_outP = *string1.backPoints();
  const Point& s2_inP = *string2.frontPoints();
  const Point& s2_outP = *string2.backPoints();

  bool sameString = &string1 == &string2;

  CutGraph::Vertex s1_inV = addVertex(GeometryMapping(&s1_inP  ,0 ,&string1));
  CutGraph::Vertex s1_outV = addVertex(GeometryMapping(&s1_outP ,0 ,&string1));

  if(!boost::edge(s1_inV, s1_outV, *this).second) {
    add_edge(s1_inV, s1_outV, EdgeGeomProperty(GeometryMapping(0, 0, &string1), IndexProperty(edge_count++, WeightProperty(0))), *this);
  }

  if(!sameString) {
    GeometryMapping s2_inMap =  * new GeometryMapping(&s2_inP  ,0 ,&string2);
    GeometryMapping s2_outMap = * new GeometryMapping(&s2_outP ,0 ,&string2);
    CutGraph::Vertex s2_inV = addVertex(s2_inMap);
    CutGraph::Vertex s2_outV = addVertex(s2_outMap);

    if(!boost::edge(s2_inV, s2_outV, *this).second) {
      add_edge(s2_inV, s2_outV, EdgeGeomProperty(GeometryMapping(0, 0, &string2), IndexProperty(edge_count++, WeightProperty(0))), *this);
    }

    if(!boost::edge(s1_inV,s2_inV, *this).second) {
      double w0 = s1_inP.distance(s2_inP);
      double w1 = s1_inP.distance(s2_outP);
      double w2 = s1_outP.distance(s2_inP);
      double w3 = s1_outP.distance(s2_outP);

      add_edge(s1_inV, s2_inV, EdgeGeomProperty(GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(w0))), *this);
      add_edge(s1_inV, s2_outV, EdgeGeomProperty(GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(w1))), *this);
      add_edge(s1_outV, s2_inV, EdgeGeomProperty(GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(w2))), *this);
      add_edge(s1_outV, s2_outV, EdgeGeomProperty(GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(w3))), *this);
    }
  }
}

void CutGraph::createEdge(const Segment& seg) {
  const GeometryMapping& mapIn = *new GeometryMapping(&seg.first, 0, 0);
  const GeometryMapping& mapOut = *new GeometryMapping(&seg.second, 0, 0);
  CutGraph::Vertex inV = addVertex(mapIn);
  CutGraph::Vertex outV = addVertex(mapOut);

  double weight = seg.first.distance(seg.second);

  add_edge(inV, outV, EdgeGeomProperty(*new GeometryMapping(0, &seg, 0), IndexProperty(edge_count++, WeightProperty(weight))), *this);
}


void CutGraph::createEdges(const SegmentString& string1, const SegmentString& string2, const CutGraph::Vertex& v_origin) {
  const Point& s1_inP = *string1.frontPoints();
  const Point& s1_outP = *string1.backPoints();
  const Point& s2_inP = *string2.frontPoints();
  const Point& s2_outP = *string2.backPoints();
  const Point& p_origin = *this->getPoint(v_origin);

  bool sameString = &string1 == &string2;

  GeometryMapping s1_inMap  = * new GeometryMapping(&s1_inP  ,0 ,&string1);
  GeometryMapping s1_outMap = * new GeometryMapping(&s1_outP ,0 ,&string1);
  GeometryMapping s2_inMap  = * new GeometryMapping(&s2_inP  ,0 ,&string2);
  GeometryMapping s2_outMap = * new GeometryMapping(&s2_outP ,0 ,&string2);

  CutGraph::Vertex s1_inV = addVertex(s1_inMap);
  CutGraph::Vertex s1_outV = addVertex(s1_outMap);

  if(!boost::edge(s1_inV, v_origin, *this).second) {
    double wo0 = p_origin.distance(s1_inP);
    double wo1 = p_origin.distance(s1_outP);
    add_edge(v_origin, s1_inV, EdgeGeomProperty(*new GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(wo0))), *this);
    add_edge(v_origin, s1_outV, EdgeGeomProperty(*new GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(wo1))), *this);
    add_edge(s1_inV, s1_outV, EdgeGeomProperty(*new GeometryMapping(0, 0, &string1), IndexProperty(edge_count++, WeightProperty(0))), *this);
  }

  if(!sameString) {
    CutGraph::Vertex s2_inV = addVertex(s2_inMap);
    CutGraph::Vertex s2_outV = addVertex(s2_outMap);

    if(!boost::edge(s2_inV, v_origin, *this).second) {
      double wo2 = p_origin.distance(s2_inP);
      double wo3 = p_origin.distance(s2_outP);
      add_edge(v_origin, s2_inV, EdgeGeomProperty(*new GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(wo2))), *this);
      add_edge(v_origin, s2_outV, EdgeGeomProperty(*new GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(wo3))), *this);
      add_edge(s2_inV, s2_outV, EdgeGeomProperty(*new GeometryMapping(0, 0, &string2), IndexProperty(edge_count++, WeightProperty(0))), *this);
    }

    if(!boost::edge(s1_inV,s2_inV, *this).second) {
      double w0 = s1_inP.distance(s2_inP);
      double w1 = s1_inP.distance(s2_outP);
      double w2 = s1_outP.distance(s2_inP);
      double w3 = s1_outP.distance(s2_outP);

      add_edge(s1_inV, s2_inV, EdgeGeomProperty(*new GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(w0))), *this);
      add_edge(s1_inV, s2_outV, EdgeGeomProperty(*new GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(w1))), *this);
      add_edge(s1_outV, s2_inV, EdgeGeomProperty(*new GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(w2))), *this);
      add_edge(s1_outV, s2_outV, EdgeGeomProperty(*new GeometryMapping(0, 0, 0), IndexProperty(edge_count++, WeightProperty(w3))), *this);
    }
  }
}

CutGraph::Vertex* CutGraph::findVertex(const GeometryMapping &map) {
  GeomVertexMap::const_iterator it = geometries.find(map);
  if (it == geometries.end())
    return NULL;
  else
    return (CutGraph::Vertex*)&(*it).second;
}

CutGraph::Vertex CutGraph::addVertex(const GeometryMapping &map) {
  Vertex* v = findVertex(map);
  if (v == NULL) {
    Vertex new_vertex = add_vertex(* new VertexGeomProperty(map), *this);
    geometries[map] = new_vertex;
    return new_vertex;
  }

  return *v;
}
