#include "CutGraph.h"

#include <boost/graph/strong_components.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <iostream>
#include <list>
CutGraph::CutGraph() {
}

CutGraph::~CutGraph() {
}

Vertex CutGraph::find_or_insert(const CutPoint2D &point) {
    map<CutPoint2D, Vertex>::const_iterator it = this->geomIndex.points.find(point);
    if (it == this->geomIndex.points.end()) {
        Vertex new_vertex = add_vertex(VertexProperty(point),graph);
        this->geomIndex.points[point] = new_vertex;
        return new_vertex;
    }
    return it->second;
}

void CutGraph::createEdge(CutPoint2D& in, CutPoint2D& out, LaserSettings& settings) {
  Vertex inV = find_or_insert(in);
  Vertex outV = find_or_insert(out);
  add_edge(inV, outV, EdgeProperty(settings), this->graph);
}

void CutGraph::createEdge(uint32_t inX, uint32_t inY, uint32_t outX, uint32_t outY,
    int power, int speed, int frequency) {
  this->createEdge(*(new CutPoint2D(inX, inY)), *(new CutPoint2D(outX, outY)), *(new LaserSettings(power, speed, frequency)));
}

void CutGraph::buildConnectedIndex() {
  using namespace boost;

  connected_components(
      this->graph,
      make_assoc_property_map(this->connIndex.component));
}

int main() {
  CutGraph cg;

  cg.createEdge(0, 0, 1, 0, 0, 0, 0);
  cg.createEdge(1, 0, 2, 0, 0, 0, 0);
  cg.createEdge(2, 0, 3, 0, 0, 0, 0);

  cg.createEdge(1, 1, 1, 2, 0, 0, 0);
  cg.createEdge(1, 2, 2, 2, 0, 0, 0);
  cg.createEdge(2, 2, 2, 1, 0, 0, 0);
  cg.createEdge(2, 1, 3, 3, 0, 0, 0);

  cg.buildConnectedIndex();

  Graph::vertex_iterator i, iend;
  ConnectedIndex connIndex = cg.getConnectedIndex();

  for (boost::tie(i,iend) = cg.getVertexIteratorPair(); i != iend; ++i)
    std::cout << connIndex.component[*i] << std::endl;
}
