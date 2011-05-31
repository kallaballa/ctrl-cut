#include "CutModel.h"

#include <iostream>
#include <vector>

using std::vector;

void CutModel::createEdge(Point &in, Point &out, LaserSettings& settings) {
  Vertex inV = this->findOrInsertVertex(in);
  Vertex outV = this->findOrInsertVertex(out);

  GeomProperty geomProp(0, LengthProperty(boost::geometry::distance(in, out), IndexProperty(edge_count++)));
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

  cm.createEdge(2, 1, 2, 2, laserSettings);
  cm.createEdge(2, 2, 2, 3, laserSettings);
  cm.createEdge(2, 3, 2, 4, laserSettings);
  cm.createEdge(2, 4, 2, 5, laserSettings);


  CutGraph::vertex_iterator v, vend;
  CutGraph& graph = cm.getCutGraph();
  build(graph, graph);

  create_biconnected_component_lookup(graph);
  list<EdgeComponentView<CutGraph> >& biconnected_views = all_edge_component_views(graph);

  EdgeComponentView<CutGraph>::vertex_iterator egv_vit,egv_vit_end;
  EdgeComponentView<CutGraph>::edge_iterator egv_eit,egv_eit_end;
  list<EdgeComponentView<CutGraph> >::iterator egv;

  std::cout << "biconnected components" << std::endl;
  for (egv = biconnected_views.begin(); egv != biconnected_views.end(); ++egv) {
    std::cout << std::endl;

    build(cm.getCutGraph(), *egv);

    for(boost::tie(egv_vit,egv_vit_end) = vertices(*egv); egv_vit != egv_vit_end; ++egv_vit)
      std::cout << get_point(*egv_vit, *egv) << std::endl;

    for(boost::tie(egv_eit,egv_eit_end) = edges(*egv); egv_eit != egv_eit_end; ++egv_eit)
      std::cout << segment(*egv_eit, *egv) << "=" << get_length(*egv_eit, *egv) << std::endl;
  }
/*
  indices.createVertexComponentLookup("connected", connected_componentlookup);
  list<VertexComponentGraphView> &connected_views = all_vertex_component_views("connected", cm);

  VertexComponentView<Undi::vertex_iterator vgv_vit,vgv_vit_end;
  VertexComponentGraphView::edge_iterator vgv_eit,vgv_eit_end;
  list<VertexComponentGraphView>::iterator vgv;

  std::cout << "connected components" << std::endl;
  for (vgv = connected_views.begin(); vgv != connected_views.end(); ++vgv) {
    std::cout << std::endl;

    build(cm.getUndirectedGraph(), *vgv);

    for(boost::tie(vgv_vit,vgv_vit_end) = vertices(*vgv); vgv_vit != vgv_vit_end; ++vgv_vit)
      std::cout << get_point(*vgv_vit, *vgv) << std::endl;

    for(boost::tie(vgv_eit,vgv_eit_end) = edges(*vgv); vgv_eit != vgv_eit_end; ++vgv_eit)
      std::cout << segment(*vgv_eit, *vgv) << "=" << get_length(*vgv_eit, *vgv) << std::endl;
  }

  LinearRing lr;
  Linestring ls;
  Point cp;

  vector<PolyLine> geometryVector;
  geometryVector.push_back(lr);
  geometryVector.push_back(ls);*/
}
