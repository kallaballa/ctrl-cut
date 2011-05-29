#include "CutModel.h"
#include "GraphView.h"
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/linear_ring.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <iostream>
#include <vector>

using boost::geometry::linear_ring;
using boost::geometry::linestring;
using boost::geometry::polygon;
using std::vector;

void CutModel::createEdge(Point &in, Point &out, LaserSettings& settings) {
  Vertex inV = this->findOrInsertVertex(in);
  Vertex outV = this->findOrInsertVertex(out);

  GeomProperty geomProp(0, LengthProperty(boost::geometry::distance(in, out), IndexProperty(0)));
  CutProperty cutProp(settings, geomProp);

  add_edge(inV, outV, cutProp, this->graph);
}


void CutModel::createEdge(uint32_t inX, uint32_t inY, uint32_t outX, uint32_t outY, LaserSettings& settings) {
  this->createEdge(*(new Point(inX, inY)), *(new Point(outX, outY)), settings);
}

Vertex CutModel::findOrInsertVertex(const Point &point) {
  PointLookup::const_iterator it = pointLookup.find(point);
    if (it == pointLookup.end()) {
        Vertex new_vertex = add_vertex(PointProperty(point),graph);
        pointLookup[point] = new_vertex;
        return new_vertex;
    }

    return it->second;
}


int main() {
  CutModel cm;
  LaserSettings laserSettings(0,0,0);

  cm.createEdge(0, 0, 1, 0, laserSettings);
  cm.createEdge(1, 0, 1, 1, laserSettings);
  cm.createEdge(1, 1, 0, 1, laserSettings);
  cm.createEdge(0, 1, 0, 0, laserSettings);
  cm.createEdge(1, 1, 0, 0, laserSettings);

  cm.createEdge(1, 1, 1, 2, laserSettings);
  cm.createEdge(1, 2, 2, 2, laserSettings);
  cm.createEdge(2, 2, 2, 1, laserSettings);
  cm.createEdge(2, 1, 3, 3, laserSettings);

  UndirectedGraph& udGraph = cm.getUndirectedGraph();
  UndirectedGraph::vertex_iterator v, vend;

  property_map<UndirectedGraph, vertex_point_t>::type vPoint = get(vertex_point, udGraph);
  property_map<UndirectedGraph, edge_length_t>::type eLength = get(edge_length, udGraph);

  Indices& indices = Indices::getIndices(cm);
  indices.createEdgeComponentLookup("biconnected", biconnected_componentlookup);
  indices.createVertexComponentLookup("connected", connected_componentlookup);


  list<EdgeComponentGraphView> &biconnected_views = all_edge_component_views("biconnected", cm);
  list<VertexComponentGraphView> &connected_views = all_vertex_component_views("connected", cm);

  EdgeComponentGraphView::vertex_iterator egv_vertex,egv_vertex_end;
  EdgeComponentGraphView::edge_iterator egv_edge,egv_edge_end;

  VertexComponentGraphView::vertex_iterator vgv_vertex,vgv_vertex_end;
  VertexComponentGraphView::edge_iterator vgv_edge,vgv_edge_end;


  list<EdgeComponentGraphView>::iterator egv;
  list<VertexComponentGraphView>::iterator vgv;

  for (egv = biconnected_views.begin(); egv != biconnected_views.end(); ++egv) {
    std::cout << std::endl;

    for(boost::tie(egv_vertex,egv_vertex_end) = vertices(*egv); egv_vertex != egv_vertex_end; ++egv_vertex)
      std::cout << vPoint[*egv_vertex] << std::endl;

    for(boost::tie(egv_edge,egv_edge_end) = edges(*egv); egv_edge != egv_edge_end; ++egv_edge)
      std::cout << "{" << vPoint[source(*egv_edge, *egv)]
                                 << " -> " << vPoint[target(*egv_edge, *egv)]
                                                     << "}" << eLength[*egv_edge] << std::endl;
  }

  for (vgv = connected_views.begin(); vgv != connected_views.end(); ++vgv) {
    std::cout << std::endl;

    for(boost::tie(vgv_vertex,vgv_vertex_end) = vertices(*vgv); vgv_vertex != vgv_vertex_end; ++vgv_vertex)
      std::cout << vPoint[*vgv_vertex] << std::endl;

    for(boost::tie(vgv_edge,vgv_edge_end) = edges(*vgv); vgv_edge != vgv_edge_end; ++vgv_edge)
      std::cout << "{" << vPoint[source(*vgv_edge, *vgv)]
                                 << " -> " << vPoint[target(*vgv_edge, *vgv)]
                                                     << "}" << eLength[*vgv_edge] << std::endl;
  }

  linear_ring<Point> lr;
  linestring<Point> ls;
  Point cp;

  vector<vector<Point> > geometryVector;
  geometryVector.push_back(lr);
  geometryVector.push_back(ls);

}
