#include "CutGraph.h"

#include <iostream>
#include <vector>
/*
template<typename Graph>
const Graph::Edge findSteapest(const Vertex& v) {
  // Find next clockwise segment
  const Edge* found = NULL;
  float steapest = 2 * M_PI;
  boost::graph_traits<CutGraph>::out_edge_iterator oe_it, oe_end;

  bool invert;
  for (boost::tie(oe_it, oe_end) = boost::out_edges(v, *this); oe_it != oe_end; ++oe_it){
    if(this->getOwner(*oe_it) != NULL)
      continue;

    const Edge candidate = *oe_it;
    const Segment* seg = getSegment(*oe_it);

    // make sure we're pointing into the positive halfsphere
    if (seg->first[0] > seg->second[0])
      invert = false;
    else
      invert = true;

    float slope = seg->getSlope(invert);
    if (slope < steapest) {
      steapest = slope;
      found = &candidate;
    }
  }
  assert(found != NULL);
  return *found;
}
*/

