
#include "vector/model/CutModel.h"
#include "vector/graph/Traverse.h"
#include "vector/geom/Geometry.h"

int main() {
  CutModel planar;
  CutSettings settings(0,0,0);
  planar.createSegment(0, 0, 1, 0, settings);
  planar.createSegment(1, 0, 1, 1, settings);
  planar.createSegment(1, 1, 0, 1, settings);
  planar.createSegment(0, 1, 0, 0, settings);
/*
  StringList strings;
  make_linestrings(strings, planar.begin(), planar.end());
  dump_linestrings(std::cerr, strings.begin(), strings.end());
*/
  SegmentGraph graph;
  create_planar_graph(graph, planar.begin(), planar.end());
  SegmentGraph::Embedding embedding(num_vertices(graph));
  assert(boost::build_planar_embedding(embedding, graph));
}
