
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
  std::map<SegmentGraph::Edge, graph_traits<SegmentGraph>::edges_size_type > e_index_map;
  graph_traits<SegmentGraph>::edges_size_type edge_count = 0;
  graph_traits<SegmentGraph>::edge_iterator ei, ei_end;

  for(tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei)
    e_index_map[*ei] = edge_count++;

  SegmentGraph::Embedding embedding(num_vertices(graph));
  assert(build_planar_embedding(embedding, graph));
}
