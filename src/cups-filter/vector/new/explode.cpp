#include "CutGraph.h"

int main() {
  CutGraph graph;
  LaserSettings settings(0,0,0);
  createEdge(graph, 0,0, 2,2, settings);
  createEdge(graph, 0,2, 2,0, settings);

  Index<CutGraph>::Linestrings& linestrings = (*new Index<CutGraph>::Linestrings());
  traverse_linestrings(graph, linestrings);
}
