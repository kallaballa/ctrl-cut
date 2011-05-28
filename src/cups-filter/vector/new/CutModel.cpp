#include "CutModel.h"
#include "GraphView.h"

#include <boost/graph/strong_components.hpp>
#include <iostream>


double CutModel::measure(CutSegment2D &segm) {
  return boost::geometry::distance(segm.first, segm.second);
}

void CutModel::createEdge(CutSegment2D &segm, LaserSettings& settings) {
  Vertex inV = this->findOrInsertVertex(segm.first);
  Vertex outV = this->findOrInsertVertex(segm.second);

  SegmentProperty segmProp(segm, LengthProperty(this->measure(segm)));
  GeomProperty geomProp(false, segmProp);
  CutProperty cutProp(settings, geomProp);

  add_edge(inV, outV, cutProp, this->graph);
}

void CutModel::createEdge(CutPoint2D &in, CutPoint2D &out, LaserSettings& settings) {
  this->createEdge(*(new CutSegment2D(in, out)), settings);
}

void CutModel::createEdge(uint32_t inX, uint32_t inY, uint32_t outX, uint32_t outY, LaserSettings& settings) {
  this->createEdge(*(new CutPoint2D(inX, inY)), *(new CutPoint2D(outX, outY)), settings);
}

Vertex CutModel::findOrInsertVertex(const CutPoint2D &point) {
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
  CutPoint2D p(0,0);

  cm.createEdge(0, 0, 1, 0, laserSettings);
  cm.createEdge(1, 0, 1, 1, laserSettings);
  cm.createEdge(1, 1, 0, 1, laserSettings);
  cm.createEdge(0, 1, 0, 0, laserSettings);

  cm.createEdge(1, 1, 1, 2, laserSettings);
  cm.createEdge(1, 2, 2, 2, laserSettings);
  cm.createEdge(2, 2, 2, 1, laserSettings);
  cm.createEdge(2, 1, 3, 3, laserSettings);

  Graph& graph = cm.getGraph();
  Graph::vertex_iterator v, vend;

  list<GraphView>::const_iterator gv, gvend;
  GraphView::vertex_iterator vv, vvend;
  GraphView::edge_iterator ev, evend;

  property_map<Graph, vertex_point2d_t>::type vPoint = get(vertex_point2d, cm.getGraph());
  property_map<Graph, edge_length_t>::type eLength = get(edge_length, cm.getGraph());
  property_map<Graph, edge_segment_t>::type eSegment = get(edge_segment, cm.getGraph());

  Indices& indices = Indices::getIndices(cm);
  indices.createComponentLookup("strong", strong_componentlookup);
  indices.createComponentLookup("connected", connected_componentlookup);

  list<GraphView> &strong_views = all_component_views("strong", cm);
  list<GraphView> &connected_views = all_component_views("connected", cm);

  ComponentLookup &compLookUp = indices.getComponentLookup("strong");
  for(boost::tie(v,vend) = vertices(graph); v != vend; ++v)
    std::cout << vPoint[*v] << ": " << compLookUp.second.at(*v) << std::endl;

  compLookUp = indices.getComponentLookup("connected");
  for(boost::tie(v,vend) = vertices(graph); v != vend; ++v)
    std::cout << vPoint[*v] << ": " << compLookUp.second.at(*v) << std::endl;

  for (gv = strong_views.begin(); gv != strong_views.end(); ++gv) {
    std::cout << std::endl;

    for(boost::tie(vv,vvend) = vertices(*gv); vv != vvend; ++vv)
      std::cout << vPoint[*vv] << std::endl;

    for(boost::tie(ev,evend) = edges(*gv); ev != evend; ++ev)
      std::cout << eSegment[*ev] << " = " << eLength[*ev] << std::endl;
  }

  for (gv = connected_views.begin(); gv != connected_views.end(); ++gv) {
    std::cout << std::endl;

    for(boost::tie(vv,vvend) = vertices(*gv); vv != vvend; ++vv)
      std::cout << vPoint[*vv] << std::endl;

    for(boost::tie(ev,evend) = edges(*gv); ev != evend; ++ev)
      std::cout << eSegment[*ev] << " = " << eLength[*ev] << std::endl;
  }
}
